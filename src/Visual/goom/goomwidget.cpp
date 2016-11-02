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
#include "inlines.h"
#include "goomwidget.h"

#define VISUAL_NODE_SIZE 512 //samples
#define VISUAL_BUFFER_SIZE (5*VISUAL_NODE_SIZE)

GoomWidget::GoomWidget (QWidget *parent) : Visual (parent)
{
    m_intern_vis_data = 0;
    m_peaks = 0;
    m_x_scale = 0;
    m_buffer_at = 0;
    m_rows = 0;
    m_cols = 0;
    m_update = false;
    m_info = 0;

    setWindowTitle (tr("Qmmp Analyzer"));
    setMinimumSize(2*300-30,105);
    m_timer = new QTimer (this);
    connect(m_timer, SIGNAL (timeout()), this, SLOT (timeout()));
    m_left_buffer = new float[VISUAL_BUFFER_SIZE];
    m_right_buffer = new float[VISUAL_BUFFER_SIZE];

    clear();
    createMenu();
    readSettings();

    PluginInfo *init = goom_init(50, 50);
}

GoomWidget::~GoomWidget()
{
    delete [] m_left_buffer;
    delete [] m_right_buffer;

    if(m_peaks)
        delete [] m_peaks;
    if(m_intern_vis_data)
        delete [] m_intern_vis_data;
    if(m_x_scale)
        delete [] m_x_scale;
}

void GoomWidget::add (float *data, size_t samples, int chan)
{
    if (!m_timer->isActive ())
        return;

    if(VISUAL_BUFFER_SIZE == m_buffer_at)
    {
        m_buffer_at -= VISUAL_NODE_SIZE;
        memmove(m_left_buffer, m_left_buffer + VISUAL_NODE_SIZE, m_buffer_at * sizeof(float));
        memmove(m_right_buffer, m_right_buffer + VISUAL_NODE_SIZE, m_buffer_at * sizeof(float));
        return;
    }

    int frames = qMin(int(samples/chan), VISUAL_BUFFER_SIZE - m_buffer_at);

    stereo_from_multichannel(m_left_buffer + m_buffer_at,
                             m_right_buffer + m_buffer_at, data, frames, chan);

    m_buffer_at += frames;
}

void GoomWidget::clear()
{
    m_buffer_at = 0;
    m_rows = 0;
    m_cols = 0;
    update();
}


void GoomWidget::timeout()
{
    mutex()->lock();
    if(m_buffer_at < VISUAL_NODE_SIZE)
    {
        mutex()->unlock ();
        return;
    }

    process (m_left_buffer, m_right_buffer);
    m_buffer_at -= VISUAL_NODE_SIZE;
    memmove(m_left_buffer, m_left_buffer + VISUAL_NODE_SIZE, m_buffer_at * sizeof(float));
    memmove(m_right_buffer, m_right_buffer + VISUAL_NODE_SIZE, m_buffer_at * sizeof(float));
    mutex()->unlock ();
    update();
}

void GoomWidget::toggleFullScreen()
{
    setWindowState(windowState() ^Qt::WindowFullScreen);
}

void GoomWidget::readSettings()
{
    QSettings settings(Qmmp::configFile(), QSettings::IniFormat);
    settings.beginGroup("Analyzer");
    m_peaks_falloff = settings.value("peak_falloff", 0.2).toDouble();
    m_analyzer_falloff = settings.value("analyzer_falloff", 2.2).toDouble();
    m_show_peaks = settings.value("show_peaks", true).toBool();
    m_timer->setInterval(1000 / settings.value("refresh_rate", 25).toInt());
    m_color1.setNamedColor(settings.value("color1", "Green").toString());
    m_color2.setNamedColor(settings.value("color2", "Yellow").toString());
    m_color3.setNamedColor(settings.value("color3", "Red").toString());
    m_bgColor.setNamedColor(settings.value("bg_color", "Black").toString());
    m_peakColor.setNamedColor(settings.value("peak_color", "Cyan").toString());
    m_cell_size = settings.value("cells_size", QSize(15, 6)).toSize();


    if(!m_update)
    {
        m_update = true;
        m_peaksAction->setChecked(m_show_peaks);

        foreach(QAction *act, m_fpsGroup->actions ())
        {
            if (m_timer->interval() == 1000 / act->data().toInt())
                act->setChecked(true);
        }
        foreach(QAction *act, m_peaksFalloffGroup->actions ())
        {
            if (m_peaks_falloff == act->data().toDouble())
                act->setChecked(true);
        }
        foreach(QAction *act, m_analyzerFalloffGroup->actions ())
        {
            if (m_analyzer_falloff == act->data().toDouble())
                act->setChecked(true);
        }

        //fallback
        if(!m_fpsGroup->checkedAction())
        {
            m_fpsGroup->actions().at(1)->setChecked(true);
            m_timer->setInterval(1000 / 25);
        }
        if(!m_peaksFalloffGroup->checkedAction())
        {
            m_peaksFalloffGroup->actions().at(1)->setChecked(2);
            m_peaks_falloff = 0.2;
        }
        if(!m_peaksFalloffGroup->checkedAction())
        {
            m_peaksFalloffGroup->actions().at(1)->setChecked(2);
            m_analyzer_falloff = 2.2;
        }

        restoreGeometry(settings.value("geometry").toByteArray());
    }
}

void GoomWidget::writeSettings()
{
    QSettings settings(Qmmp::configFile(), QSettings::IniFormat);
    settings.beginGroup("Analyzer");
    QAction *act = m_fpsGroup->checkedAction ();
    settings.setValue("refresh_rate", act ? act->data().toInt() : 25);
    act = m_peaksFalloffGroup->checkedAction ();
    settings.setValue("peak_falloff", act ? act->data().toDouble() : 0.2);
    act = m_analyzerFalloffGroup->checkedAction ();
    settings.setValue("analyzer_falloff", act ? act->data().toDouble() : 2.2);
    settings.setValue("show_peaks", m_peaksAction->isChecked());
    settings.endGroup();
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
    settings.setValue("Analyzer/geometry", saveGeometry());
    Visual::closeEvent(event); //removes visualization object
}

void GoomWidget::paintEvent (QPaintEvent * e)
{
    QPainter painter (this);
    mutex()->lock();
    painter.drawImage(0,0, m_image);
    mutex()->unlock();
    //painter.fillRect(e->rect(),m_bgColor);
    //draw(&painter);
}

void GoomWidget::mousePressEvent(QMouseEvent *e)
{
    //if (e->button() == Qt::RightButton)
      //  m_menu->exec(e->globalPos());
}

void GoomWidget::process (float *left, float *right)
{
    if(m_image.size() != size() || !m_info)
    {
        if(!m_info)
            m_info = goom_init(width(), height());
        m_image = QImage(size(), QImage::Format_RGB32);
        goom_set_resolution(m_info, width(), height());
        goom_set_screenbuffer(m_info, m_image.bits());
    }



    gint16 data[2][512];

    for(int i = 0; i < 512; ++i)
    {
        data[0][i] = left[i] * 32767.0;
        data[1][i] = right[i] * 32767.0;
    }

    goom_update (m_info, data, 0, 0, "test", "test");
}

void GoomWidget::createMenu()
{
    m_menu = new QMenu (this);
    connect(m_menu, SIGNAL(triggered (QAction *)),SLOT(writeSettings()));
    connect(m_menu, SIGNAL(triggered (QAction *)),SLOT(readSettings()));

    m_peaksAction = m_menu->addAction(tr("Peaks"));
    m_peaksAction->setCheckable(true);

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

    QMenu *analyzerFalloff = m_menu->addMenu(tr("Analyzer Falloff"));
    m_analyzerFalloffGroup = new QActionGroup(this);
    m_analyzerFalloffGroup->setExclusive(true);
    m_analyzerFalloffGroup->addAction(tr("Slowest"))->setData(1.2);
    m_analyzerFalloffGroup->addAction(tr("Slow"))->setData(1.8);
    m_analyzerFalloffGroup->addAction(tr("Medium"))->setData(2.2);
    m_analyzerFalloffGroup->addAction(tr("Fast"))->setData(2.4);
    m_analyzerFalloffGroup->addAction(tr("Fastest"))->setData(2.8);
    foreach(QAction *act, m_analyzerFalloffGroup->actions ())
    {
        act->setCheckable(true);
        analyzerFalloff->addAction(act);
    }

    QMenu *peaksFalloff = m_menu->addMenu(tr("Peaks Falloff"));
    m_peaksFalloffGroup = new QActionGroup(this);
    m_peaksFalloffGroup->setExclusive(true);
    m_peaksFalloffGroup->addAction(tr("Slowest"))->setData(0.05);
    m_peaksFalloffGroup->addAction(tr("Slow"))->setData(0.1);
    m_peaksFalloffGroup->addAction(tr("Medium"))->setData(0.2);
    m_peaksFalloffGroup->addAction(tr("Fast"))->setData(0.4);
    m_peaksFalloffGroup->addAction(tr("Fastest"))->setData(0.8);
    foreach(QAction *act, m_peaksFalloffGroup->actions ())
    {
        act->setCheckable(true);
        peaksFalloff->addAction(act);
    }
    m_menu->addSeparator();
    QAction *fullScreenAction = m_menu->addAction(tr("&Full Screen"), this, SLOT(toggleFullScreen()), tr("F"));
    addAction(fullScreenAction);
    update();
}
