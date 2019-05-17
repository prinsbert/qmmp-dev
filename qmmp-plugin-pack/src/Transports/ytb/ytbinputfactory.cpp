/***************************************************************************
 *   Copyright (C) 2019 by Ilya Kotov                                      *
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

#include <QtPlugin>
#include <QMessageBox>
#include <qmmp/qmmp.h>
#include "ytbinputsource.h"
#include "ytbinputfactory.h"

InputSourceProperties YtbInputFactory::properties() const
{
    InputSourceProperties properties;
    properties.protocols << "ytb";
    properties.name = tr("Youtube Plugin");
    properties.shortName = "http";
    properties.hasAbout = false;
    properties.hasSettings = false;
    return properties;
}

InputSource *YtbInputFactory::create(const QString &url, QObject *parent)
{
    return new YtbInputSource(url, parent);
}

void YtbInputFactory::showSettings(QWidget *parent)
{
    Q_UNUSED(parent);
}

void YtbInputFactory::showAbout(QWidget *parent)
{
    /*QMessageBox::about (parent, tr("About HTTP Transport Plugin"),
                        tr("Qmmp HTTP Transport Plugin")+"\n"+
                        tr("Compiled against libcurl-%1").arg(LIBCURL_VERSION) + "\n" +
                        tr("Written by: Ilya Kotov <forkotov02@ya.ru>"));*/
}

QString YtbInputFactory::translation() const
{
    return QLatin1String(":/ytb_plugin_");
}
