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
#include <QProcess>
#include <qmmp/qmmp.h>
#include "ytbinputsource.h"
#include "ytbinputfactory.h"

InputSourceProperties YtbInputFactory::properties() const
{
    InputSourceProperties properties;
    properties.protocols << "ytb";
    properties.regExps = {
        QRegularExpression("^https\\:\\/\\/www.youtube.com\\/.*"),
        QRegularExpression("^https\\:\\/\\/youtu.be\\/.*")
    };
    properties.name = tr("Youtube Plugin");
    properties.shortName = "ytb";
    properties.hasAbout = true;
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
    QProcess p;
    p.start("youtube-dl --version");
    p.waitForFinished();
    QString version = QString::fromLatin1(p.readAll()).trimmed();
    if(version.isEmpty())
        version = tr("not found");

    QMessageBox::about(parent, tr("About Youtube Transport Plugin"),
                       tr("Qmmp Youtube Transport Plugin") + "<br>" +
                       tr("This plugin adds feature to play audio from Youtube using "
                          "<a href=\"https://youtube-dl.org/\">youtube-dl</a> utility") + "<br>"+
                       tr("youtube-dl version: %1").arg(version) + "<br>" +
                       tr("Written by: Ilya Kotov &lt;forkotov02@ya.ru&gt;"));
}

QString YtbInputFactory::translation() const
{
    return QLatin1String(":/ytb_plugin_");
}
