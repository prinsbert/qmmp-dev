/***************************************************************************
 *   Copyright (C) 2013-2024 by Ilya Kotov                                 *
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
#include <QPaintEvent>
#include <QAction>
#include <qmmp/qmmp.h>
#include <qmmpui/filedialog.h>
#include "coverwidget.h"

CoverWidget::CoverWidget(QWidget *parent)
        : QWidget(parent)
{
    setContextMenuPolicy(Qt::ActionsContextMenu);
    QAction *saveAsAction = new QAction(tr("&Save As..."), this);
    connect(saveAsAction, SIGNAL(triggered()), SLOT(saveAs()));
    addAction(saveAsAction);
    m_image = QImage(":/qsui/ui_no_cover.png");
}

CoverWidget::~CoverWidget()
{}

void CoverWidget::setCover(const QImage &img)
{
    m_image = img.isNull() ? QImage(":/qsui/ui_no_cover.png") : img;
    update();
}

void CoverWidget::clearCover()
{
    setCover(QImage());
    update();
}

void CoverWidget::paintEvent(QPaintEvent *)
{
    if(!m_image.isNull())
    {
        QPainter paint(this);
        QImage img = m_image.scaled(size(), Qt::KeepAspectRatio, Qt::SmoothTransformation);
        paint.drawImage((width() - img.width()) / 2, (height() - img.height()) / 2, img);
    }
}

void CoverWidget::saveAs()
{
    QString path = FileDialog::getSaveFileName(this, tr("Save Cover As"),
                                                 QDir::homePath() + "/cover.jpg",
                                                 tr("Images") +" (*.png *.jpg)");

    if (!path.isEmpty())
        m_image.save(path);
}
