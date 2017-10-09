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

// FFVideoFactory
FFVideoFactory::FFVideoFactory()
{
    avcodec_register_all();
    avformat_network_init();
    av_register_all();
}

const EngineProperties FFVideoFactory::properties() const
{
    EngineProperties properties;
    properties.name = tr("FFmpeg Video Plugin");
    properties.shortName = "ffvideo";
    properties.filters << "*.avi" << "*.mpg" << "*.mpeg" << "*.divx" << "*.qt"
                       << "*.mov" << "*.wmv" << "*.asf" << "*.flv" << "*.3gp"
                       << "*.mkv" << "*.mp4" << "*.webm";
    properties.description = tr("Video Files");
    //properties.contentType = "application/ogg;audio/x-vorbis+ogg";
    properties.protocols << "file";
    properties.hasAbout = false;
    properties.hasSettings = false;
    return properties;
}

bool FFVideoFactory::supports(const QString &source) const
{
    foreach(QString filter,  properties().filters)
    {
        QRegExp regexp(filter, Qt::CaseInsensitive, QRegExp::Wildcard);
        if (regexp.exactMatch(source))
            return true;
    }
    return false;
}

AbstractEngine *FFVideoFactory::create(QObject *parent)
{
    return new FFmpegEngine(this, parent);
}

QList<FileInfo *> FFVideoFactory::createPlayList(const QString &fileName, bool useMetaData, QStringList *)
{
    QList <FileInfo*> list;
    AVFormatContext *in = 0;

#ifdef Q_OS_WIN
    if (avformat_open_input(&in,fileName.toUtf8().constData(), 0, 0) < 0)
#else
    if (avformat_open_input(&in,fileName.toLocal8Bit().constData(), 0, 0) < 0)
#endif
    {
        qDebug("DecoderFFmpegFactory: unable to open file");
        return list;
    }
    FileInfo *info = new FileInfo(fileName);
    avformat_find_stream_info(in, 0);

    if (useMetaData)
    {
        AVDictionaryEntry *album = av_dict_get(in->metadata,"album",0,0);
        if(!album)
            album = av_dict_get(in->metadata,"WM/AlbumTitle",0,0);
        AVDictionaryEntry *artist = av_dict_get(in->metadata,"artist",0,0);
        if(!artist)
            artist = av_dict_get(in->metadata,"author",0,0);
        AVDictionaryEntry *comment = av_dict_get(in->metadata,"comment",0,0);
        AVDictionaryEntry *genre = av_dict_get(in->metadata,"genre",0,0);
        AVDictionaryEntry *title = av_dict_get(in->metadata,"title",0,0);
        AVDictionaryEntry *year = av_dict_get(in->metadata,"WM/Year",0,0);
        if(!year)
            year = av_dict_get(in->metadata,"year",0,0);
        if(!year)
            year = av_dict_get(in->metadata,"date",0,0);
        AVDictionaryEntry *track = av_dict_get(in->metadata,"track",0,0);
        if(!track)
            track = av_dict_get(in->metadata,"WM/Track",0,0);
        if(!track)
            track = av_dict_get(in->metadata,"WM/TrackNumber",0,0);

        if(album)
            info->setMetaData(Qmmp::ALBUM, QString::fromUtf8(album->value).trimmed());
        if(artist)
            info->setMetaData(Qmmp::ARTIST, QString::fromUtf8(artist->value).trimmed());
        if(comment)
            info->setMetaData(Qmmp::COMMENT, QString::fromUtf8(comment->value).trimmed());
        if(genre)
            info->setMetaData(Qmmp::GENRE, QString::fromUtf8(genre->value).trimmed());
        if(title)
            info->setMetaData(Qmmp::TITLE, QString::fromUtf8(title->value).trimmed());
        if(year)
            info->setMetaData(Qmmp::YEAR, year->value);
        if(track)
            info->setMetaData(Qmmp::TRACK, track->value);
    }
    info->setLength(in->duration/AV_TIME_BASE);
    avformat_close_input(&in);
    list << info;
    return list;
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
