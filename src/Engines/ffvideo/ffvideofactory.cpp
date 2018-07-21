/***************************************************************************
 *   Copyright (C) 2008-2018 by Ilya Kotov                                 *
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
#include <QtPlugin>
#include "ffvideofactory.h"
#include "ffmpegengine.h"
#include "ffvideometadatamodel.h"
#include "ffvideofactory.h"

// FFVideoFactory
FFVideoFactory::FFVideoFactory()
{
#if (LIBAVCODEC_VERSION_INT < AV_VERSION_INT(58,10,100)) //ffmpeg-3.5
    avcodec_register_all();
    avformat_network_init();
    av_register_all();
#endif
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
    properties.hasAbout = true;
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

QList<TrackInfo *> FFVideoFactory::createPlayList(const QString &path, TrackInfo::Parts parts, QStringList *)
{
    QList<TrackInfo*> list;
    AVFormatContext *in = 0;

#ifdef Q_OS_WIN
    if (avformat_open_input(&in,path.toUtf8().constData(), 0, 0) < 0)
#else
    if (avformat_open_input(&in,path.toLocal8Bit().constData(), 0, 0) < 0)
#endif
    {
        qDebug("DecoderFFmpegFactory: unable to open file");
        return list;
    }
    TrackInfo *info = new TrackInfo(path);
    avformat_find_stream_info(in, 0);

    if (parts & TrackInfo::MetaData)
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
            info->setValue(Qmmp::ALBUM, QString::fromUtf8(album->value));
        if(artist)
            info->setValue(Qmmp::ARTIST, QString::fromUtf8(artist->value));
        if(comment)
            info->setValue(Qmmp::COMMENT, QString::fromUtf8(comment->value));
        if(genre)
            info->setValue(Qmmp::GENRE, QString::fromUtf8(genre->value));
        if(title)
            info->setValue(Qmmp::TITLE, QString::fromUtf8(title->value));
        if(year)
            info->setValue(Qmmp::YEAR, year->value);
        if(track)
            info->setValue(Qmmp::TRACK, track->value);
    }
    info->setDuration(in->duration * 1000 / AV_TIME_BASE);
    avformat_close_input(&in);
    list << info;
    return list;
}

MetaDataModel* FFVideoFactory::createMetaDataModel(const QString &path, QObject *parent)
{
    return new FFVideoMetaDataModel(path, parent);
}

void FFVideoFactory::showSettings(QWidget *)
{}

void FFVideoFactory::showAbout(QWidget *parent)
{
    QMessageBox::about (parent, tr("About FFVideo Plugin"),
                        tr("FFmpeg-based video plugin for Qmmp")+"\n"+
                        tr("Compiled against:") + "\n"+
                        QString("libavformat-%1\n"
                                "libavcodec-%2\n"
                                "libavutil-%3\n"
                                "libswscale-%4")
                        .arg(AV_STRINGIFY(LIBAVFORMAT_VERSION))
                        .arg(AV_STRINGIFY(LIBAVCODEC_VERSION))
                        .arg(AV_STRINGIFY(LIBAVUTIL_VERSION))
                        .arg(AV_STRINGIFY(LIBSWSCALE_VERSION))
                        +"\n"+
                        tr("Written by: Ilya Kotov <forkotov02@ya.ru>"));
}

QTranslator *FFVideoFactory::createTranslator(QObject *parent)
{
    QTranslator *translator = new QTranslator(parent);
    QString locale = Qmmp::systemLanguageID();
    translator->load(QString(":/ffvideo_plugin_") + locale);
    return translator;
}
