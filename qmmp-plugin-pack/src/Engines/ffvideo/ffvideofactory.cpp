/***************************************************************************
 *   Copyright (C) 2008-2016 by Ilya Kotov                                 *
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

#include <QMessageBox>
#include <QTranslator>
#include <QtPlugin>
#include "ffvideofactory.h"
#include "ffmpegengine.h"
#include "ffvideometadatamodel.h"
#include "ffvideofactory.h"


// MplayerEngineFactory

const EngineProperties FFVideoFactory::properties() const
{
    EngineProperties properties;
    properties.name = tr("FFmpeg Video Plugin");
    properties.shortName = "ffvideo";
    properties.filters << "*.mp4";
    properties.description = tr("Video Files");
    //properties.contentType = "application/ogg;audio/x-vorbis+ogg";
    properties.protocols << "file";
    properties.hasAbout = false;
    properties.hasSettings = false;
    return properties;
}

bool FFVideoFactory::supports(const QString &source) const
{
    return true;
}

AbstractEngine *FFVideoFactory::create(QObject *parent)
{
    return new FFmpegEngine(this, parent);
}

QList<FileInfo *> FFVideoFactory::createPlayList(const QString &fileName, bool useMetaData, QStringList *)
{
    Q_UNUSED(useMetaData);
    QList<FileInfo *> info;
    info << new FileInfo(fileName);
    return info;
}

MetaDataModel* FFVideoFactory::createMetaDataModel(const QString &path, QObject *parent)
{
    return new FFVideoMetaDataModel(path, parent);
}

void FFVideoFactory::showSettings(QWidget *parent)
{
    //SettingsDialog *s = new SettingsDialog(parent);
    //s->show();
}

void FFVideoFactory::showAbout(QWidget *parent)
{

}

QTranslator *FFVideoFactory::createTranslator(QObject *parent)
{
    QTranslator *translator = new QTranslator(parent);
    QString locale = Qmmp::systemLanguageID();
    translator->load(QString(":/ffvideo_plugin_") + locale);
    return translator;
}

Q_EXPORT_PLUGIN2(ffvideo,FFVideoFactory)
