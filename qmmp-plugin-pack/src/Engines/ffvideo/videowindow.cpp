/***************************************************************************
 *   Copyright (C) 2017-2026 by Ilya Kotov                                 *
 *   forkotov02@ya.ru                                                      *
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

#include <QPainter>
#include <QApplication>
#include <QPaintEvent>
#include <QResizeEvent>
#include <QSettings>
#include <QMainWindow>
#include <QAction>
#include <QMenu>
#include <QIcon>
#include <qmmp/qmmp.h>
#include <qmmp/soundcore.h>
#include "videowindow.h"

VideoWindow::VideoWindow(QWidget *parent) :
    QWidget(parent)
{
    setWindowFlags(Qt::Window);
    setAutoFillBackground(true);
    setMinimumSize(100, 100);
    setWindowTitle(tr("FFmpeg Video"));
    QSettings settings;
    restoreGeometry(settings.value("FFVideo/geometry"_L1).toByteArray());
    m_core = SoundCore::instance();
    m_menu = new QMenu(this);
#if QT_VERSION < QT_VERSION_CHECK(6, 3, 0)
    QAction *togglePauseAction = m_menu->addAction(QIcon::fromTheme(u"media-playback-pause"_s), tr("&Pause"), m_core, &SoundCore::pause, tr("Space"));
    m_menu->addAction(QIcon::fromTheme(u"media-playback-stop"_s), tr("&Stop"), m_core, &SoundCore::stop, tr("V"));
    m_menu->addSeparator();
    m_menu->addAction(tr("&Fullscreen"), this, &VideoWindow::setFullScreen, tr("F"))->setCheckable(true);
#else
    QAction *togglePauseAction = m_menu->addAction(QIcon::fromTheme(u"media-playback-pause"_s), tr("&Pause"), tr("Space"), m_core, &SoundCore::pause);
    m_menu->addAction(QIcon::fromTheme(u"media-playback-stop"_s), tr("&Stop"), tr("V"), m_core, &SoundCore::stop);
    m_menu->addSeparator();
    m_menu->addAction(tr("&Fullscreen"), tr("F"), this, &VideoWindow::setFullScreen)->setCheckable(true);
#endif
    addActions(m_menu->actions());
    //seeking
    QAction *forwardAction = new QAction(this);
    forwardAction->setShortcut(QKeySequence(Qt::Key_Right));
    connect(forwardAction, &QAction::triggered, this, &VideoWindow::forward);
    QAction *backwardAction = new QAction(this);
    backwardAction->setShortcut(QKeySequence(Qt::Key_Left));
    connect(backwardAction, &QAction::triggered, this, &VideoWindow::backward);
    addActions({ forwardAction, backwardAction });

    connect(m_core, &SoundCore::stateChanged, this, [togglePauseAction] (Qmmp::State state) {
        if(state == Qmmp::Playing || state == Qmmp::Buffering)
        {
            togglePauseAction->setIcon(QIcon::fromTheme(u"media-playback-pause"_s));
            togglePauseAction->setText(tr("&Pause"));
        }
        else if(state == Qmmp::Paused)
        {
            togglePauseAction->setIcon(QIcon::fromTheme(u"media-playback-start"_s));
            togglePauseAction->setText(tr("&Resume"));
        }
        else
        {
            togglePauseAction->setIcon(QIcon::fromTheme(u"media-playback-start"_s));
            togglePauseAction->setText(tr("&Play"));
        }
    });
}

void VideoWindow::addImage(const QImage &img)
{
    m_mutex.lock();
    m_image = img;
    m_mutex.unlock();
    QMetaObject::invokeMethod(this, "update", Qt::QueuedConnection);
}

void VideoWindow::setFullScreen(bool enabled)
{
    if(enabled)
        setWindowState(windowState() | Qt::WindowFullScreen);
    else
        setWindowState(windowState() & ~Qt::WindowFullScreen);
}

void VideoWindow::forward()
{
    m_core->seek(qMin(m_core->elapsed() + 10000, m_core->duration()));
}

void VideoWindow::backward()
{
    m_core->seek(qMax(0LL, m_core->elapsed() - 10000));
}

void VideoWindow::paintEvent(QPaintEvent *)
{
    QPainter p(this);
    p.fillRect(QRect(0, 0, width(), height()), Qt::black);
    m_mutex.lock();
    p.drawImage((width() - m_image.width()) / 2, (height() - m_image.height()) / 2, m_image);
    m_mutex.unlock();
}

bool VideoWindow::event(QEvent *e)
{
    if(e->type() == QEvent::Resize && e->spontaneous())
    {
        emit resizeRequest(static_cast<QResizeEvent*>(e)->size());
    }
    else if(e->type() == QEvent::Close && e->spontaneous())
    {
        emit stopRequest();
    }
    return QWidget::event(e);
}

void VideoWindow::closeEvent(QCloseEvent *)
{
    QSettings settings;
    settings.setValue("FFVideo/geometry"_L1, saveGeometry());
}

void VideoWindow::contextMenuEvent(QContextMenuEvent *event)
{
    m_menu->popup(mapToGlobal(event->pos()));
}
