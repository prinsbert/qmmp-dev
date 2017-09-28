/***************************************************************************
 *   Copyright (C) 2017 by Ilya Kotov                                      *
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

#include <QPainter>
#include <QApplication>
#include <QPaintEvent>
#include "videowindow.h"

VideoWindow::VideoWindow(QWidget *parent) :
    QWidget(0)
{
    setAutoFillBackground(false);
    resize(1027, 758);
}

void VideoWindow::addImage(const QImage &img)
{
    m.lock();
    m_image = img;
    m.unlock();
    QMetaObject::invokeMethod(this, "update", Qt::QueuedConnection);
}

void VideoWindow::paintEvent(QPaintEvent *)
{
    QPainter p(this);
    p.fillRect(QRect(0, 0, width(), height()), Qt::black);
    m.lock();
    p.drawImage(0,0, m_image);
    m.unlock();
}
