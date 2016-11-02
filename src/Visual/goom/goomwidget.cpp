/***************************************************************************
 *   Copyright (C) 2016 by Ilya Kotov                                      *
 *   forkotov02@hotmail.ru                                                 *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.         *
 ***************************************************************************/
#include <QTimer>
#include <QSettings>
#include <QPainter>
#include <QMenu>
#include <QActionGroup>
#include <QPaintEvent>
#include <math.h>
#include <stdlib.h>
#include <qmmp/buffer.h>
#include <qmmp/output.h>
#include <qmmp/soundcore.h>
#include <QTime>
#include "goomwidget.h"

#define VISUAL_NODE_SIZE 512 //samples
#define VISUAL_BUFFER_SIZE (5*VISUAL_NODE_SIZE)

GoomWidget::GoomWidget(QWidget *parent) : Visual (parent)
{
    m_core = SoundCore::instance();
    m_update = false;
    m_goom = 0;
    m_fps = 25;
    m_buf_at = 0;
    connect(m_core, SIGNAL(metaDataChanged()), SLOT(updateTitle()));

    setWindowTitle ("Goom");
    setMinimumSize(150,150);
    m_timer = new QTimer (this);
    connect(m_timer, SIGNAL (timeout()), SLOT(timeout()));
    clear();
    createMenu();
    readSettings();
    if(m_core->state() != Qmmp::Stopped)
        updateTitle();
}

GoomWidget::~GoomWidget()
{
    if(m_goom)
        goom_close(m_goom);
    m_goom = 0;
}

void GoomWidget::add (float *data, size_t samples, int chan)
{
    size_t frames = qMin(samples / chan, size_t(4096));

    if(4096 - m_buf_at < frames)
    {
        size_t l = (frames + m_buf_at) - 4096;
        m_buf_at -= l;
        memmove(m_buf[0], m_buf[0] + l,  m_buf_at*sizeof(gint16));
        memmove(m_buf[1], m_buf[1] + l,  m_buf_at*sizeof(gint16));
    }

    if(chan == 1)
    {
        for(size_t i = 0; i < frames; i++)
        {
            m_buf[0][m_buf_at + i] = data[i*chan] * 32767.0;
            m_buf[1][m_buf_at + i] = data[i*chan] * 32767.0;
        }

    }
    else
    {
        for(size_t i = 0; i < frames; i++)
        {
            m_buf[0][m_buf_at + i] = data[i*chan] * 32767.0;
            m_buf[1][m_buf_at + i] = data[i*chan+1] * 32767.0;
        }
    }
    m_buf_at += frames;
}

void GoomWidget::timeout()
{
    if(m_image.size() != size() || !m_goom)
    {
        if(!m_goom)
            m_goom = goom_init(width(), height());
        m_image = QImage(size(), QImage::Format_RGB32);
        goom_set_resolution(m_goom, width(), height());
        goom_set_screenbuffer(m_goom, m_image.bits());
    }
    if(m_buf_at >= 512)
    {
        memcpy(m_out[0], m_buf[0], 512 * sizeof(gint16));
        memcpy(m_out[1], m_buf[1], 512 * sizeof(gint16));
        m_buf_at -= 512;
        memmove(m_buf[0], m_buf[0] + 512, m_buf_at * sizeof(gint16));
        memmove(m_buf[1], m_buf[1] + 512, m_buf_at * sizeof(gint16));
        goom_update (m_goom, m_out, 0, m_fps, qPrintable(m_title), "");
        update();
    }
}

void GoomWidget::toggleFullScreen()
{
    setWindowState(windowState() ^Qt::WindowFullScreen);
}

void GoomWidget::readSettings()
{
    QSettings settings(Qmmp::configFile(), QSettings::IniFormat);
    settings.beginGroup("Goom");
    m_fps = settings.value("refresh_rate", 25).toInt();
    m_timer->setInterval(1000 / m_fps);
    if(!m_update)
    {
        m_update = true;
        foreach(QAction *act, m_fpsGroup->actions ())
        {
            if (m_fps == act->data().toInt())
            {
                act->setChecked(true);
                break;
            }
        }
        restoreGeometry(settings.value("geometry").toByteArray());
    }
    m_showTitleAction->setChecked(settings.value("show_title", false).toBool());
}

void GoomWidget::writeSettings()
{
    QSettings settings(Qmmp::configFile(), QSettings::IniFormat);
    settings.beginGroup("Goom");
    QAction *act = m_fpsGroup->checkedAction ();
    settings.setValue("refresh_rate", act ? act->data().toInt() : 25);
    settings.setValue("show_title", m_showTitleAction->isChecked());
    settings.endGroup();
}

void GoomWidget::updateTitle()
{
    if(m_showTitleAction->isChecked())
        m_title = tr("%1 - %2").arg(m_core->metaData(Qmmp::ARTIST),
                                    m_core->metaData(Qmmp::TITLE));
    else
        m_title.clear();
}

void GoomWidget::hideEvent (QHideEvent *)
{
    m_timer->stop();
}

void GoomWidget::showEvent (QShowEvent *)
{
    m_timer->start();
}

void GoomWidget::closeEvent (QCloseEvent *event)
{
    //save geometry
    QSettings settings(Qmmp::configFile(), QSettings::IniFormat);
    settings.setValue("Goom/geometry", saveGeometry());
    Visual::closeEvent(event); //removes visualization object
}

void GoomWidget::paintEvent (QPaintEvent *)
{
    QPainter painter (this);
    mutex()->lock();
    painter.drawImage(0,0, m_image);
    mutex()->unlock();
}

void GoomWidget::mousePressEvent(QMouseEvent *e)
{
    if (e->button() == Qt::RightButton)
        m_menu->exec(e->globalPos());
}

void GoomWidget::clear()
{
    m_buf_at = 0;
    m_image.fill(Qt::black);
    update();
}

void GoomWidget::createMenu()
{
    m_menu = new QMenu (this);
    connect(m_menu, SIGNAL(triggered (QAction *)),SLOT(writeSettings()));
    connect(m_menu, SIGNAL(triggered (QAction *)),SLOT(readSettings()));

    QMenu *refreshRate = m_menu->addMenu(tr("Refresh Rate"));
    m_fpsGroup = new QActionGroup(this);
    m_fpsGroup->setExclusive(true);
    m_fpsGroup->addAction(tr("50 fps"))->setData(50);
    m_fpsGroup->addAction(tr("25 fps"))->setData(25);
    m_fpsGroup->addAction(tr("10 fps"))->setData(10);
    m_fpsGroup->addAction(tr("5 fps"))->setData(5);
    foreach(QAction *act, m_fpsGroup->actions ())
    {
        act->setCheckable(true);
        refreshRate->addAction(act);
    }
    m_showTitleAction = m_menu->addAction(tr("&Show Title"), this, SLOT(updateTitle()));
    m_showTitleAction->setCheckable(true);
    m_menu->addSeparator();
    QAction *fullScreenAction = m_menu->addAction(tr("&Full Screen"), this, SLOT(toggleFullScreen()), tr("F"));
    addAction(fullScreenAction);
}
