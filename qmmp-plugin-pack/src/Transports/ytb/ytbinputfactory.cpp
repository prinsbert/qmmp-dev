/***************************************************************************
 *   Copyright (C) 2019-2023 by Ilya Kotov                                 *
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
    properties.protocols = QStringList { "ytb" };
    properties.regExps = QList<QRegularExpression> {
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

QDialog *YtbInputFactory::createSettings(QWidget *parent)
{
    Q_UNUSED(parent);
    return nullptr;
}

void YtbInputFactory::showAbout(QWidget *parent)
{
    QString version;
    QString backend = YtbInputSource::findBackend(&version);
    QString backendName = QString("<b>%1</b>").arg(backend);
    if(backend.isEmpty() || version.isEmpty())
    {
        qWarning("YtbInputFactory: unable to find backend");
        return;
    }

    if(backend == QLatin1String("yt-dlp"))
        backendName = QLatin1String("<a href=\"https://github.com/yt-dlp/yt-dlp\">yt-dlp</a>");
    else if(backend == QLatin1String("youtube-dl"))
        backendName = QLatin1String("<a href=\"https://youtube-dl.org\">youtube-dl</a>");

    QMessageBox::about(parent, tr("About Youtube Transport Plugin"),
                       tr("Qmmp Youtube Transport Plugin") + "<br>" +
                       tr("This plugin adds feature to play audio from Youtube using %1 "
                          "utility").arg(backendName) + "<br>" +
                       tr("%1 version: %2").arg(backend, version) + "<br>" +
                       tr("Written by: Ilya Kotov &lt;forkotov02@ya.ru&gt;"));
}

QString YtbInputFactory::translation() const
{
    return QLatin1String(":/ytb_plugin_");
}
