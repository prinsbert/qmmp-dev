/***************************************************************************
 *   Copyright (C) 2008-2026 by Ilya Kotov                                 *
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

#include <QMessageBox>
#include "mplayermetadatamodel.h"
#include "mplayersettingsdialog.h"
#include "mplayerengine.h"
#include "mplayerenginefactory.h"


// MplayerEngineFactory

EngineProperties MplayerEngineFactory::properties() const
{
    EngineProperties properties;
    properties.name = tr("Mplayer Plugin");
    properties.shortName = "mplayer"_L1;
    properties.filters = MplayerInfo::filters();
    properties.description = tr("Video Files");
    //properties.contentType = "application/ogg;audio/x-vorbis+ogg";
    properties.protocols = QStringList { u"file"_s };
    properties.hasAbout = true;
    properties.hasSettings = true;
    properties.priority = 10;
    return properties;
}

bool MplayerEngineFactory::supports(const QString &source) const
{
    return MplayerInfo::supports(source);
}

AbstractEngine *MplayerEngineFactory::create(QObject *parent)
{
    return new MplayerEngine(parent);
}

QList<TrackInfo> MplayerEngineFactory::createPlayList(const QString &path, TrackInfo::Parts parts, QStringList *)
{
    Q_UNUSED(parts);
    return { MplayerInfo::createTrackInfo(path) };
}

MetaDataModel* MplayerEngineFactory::createMetaDataModel(const QString &path, bool readOnly)
{
    Q_UNUSED(readOnly);
    return new MplayerMetaDataModel(path);
}

QDialog *MplayerEngineFactory::createSettings(QWidget *parent)
{
    return new MplayerSettingsDialog(parent);
}

void MplayerEngineFactory::showAbout(QWidget *parent)
{
    QMessageBox::about(parent, tr("About MPlayer Plugin"),
                       tr("Qmmp MPlayer Plugin") + QChar::LineFeed +
                       tr("This plugin uses MPlayer as backend") + QChar::LineFeed +
                       tr("Written by: Ilya Kotov <forkotov02@ya.ru>"));
}

QString MplayerEngineFactory::translation() const
{
    return QLatin1String(":/mplayer_plugin_");
}
