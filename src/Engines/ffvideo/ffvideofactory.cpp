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

EngineProperties FFVideoFactory::properties() const
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
    TrackInfo *info = new TrackInfo(path);

    if(parts == TrackInfo::NoParts)
        return QList<TrackInfo*>() << info;

    AVFormatContext *in = nullptr;

#ifdef Q_OS_WIN
    if(avformat_open_input(&in, path.toUtf8().constData(), nullptr, nullptr) < 0)
#else
    if(avformat_open_input(&in, path.toLocal8Bit().constData(), nullptr, nullptr) < 0)
#endif
    {
        qDebug("DecoderFFmpegFactory: unable to open file");
        delete info;
        return  QList<TrackInfo*>();
    }

    avformat_find_stream_info(in, nullptr);

    if(parts & TrackInfo::MetaData)
    {
        AVDictionaryEntry *album = av_dict_get(in->metadata,"album",nullptr,0);
        if(!album)
            album = av_dict_get(in->metadata,"WM/AlbumTitle",nullptr,0);
        AVDictionaryEntry *artist = av_dict_get(in->metadata,"artist",nullptr,0);
        if(!artist)
            artist = av_dict_get(in->metadata,"author",nullptr,0);
        AVDictionaryEntry *comment = av_dict_get(in->metadata,"comment",nullptr,0);
        AVDictionaryEntry *genre = av_dict_get(in->metadata,"genre",nullptr,0);
        AVDictionaryEntry *title = av_dict_get(in->metadata,"title",nullptr,0);
        AVDictionaryEntry *year = av_dict_get(in->metadata,"WM/Year",nullptr,0);
        if(!year)
            year = av_dict_get(in->metadata,"year",nullptr,0);
        if(!year)
            year = av_dict_get(in->metadata,"date",nullptr,0);
        AVDictionaryEntry *track = av_dict_get(in->metadata,"track",nullptr,0);
        if(!track)
            track = av_dict_get(in->metadata,"WM/Track",nullptr,0);
        if(!track)
            track = av_dict_get(in->metadata,"WM/TrackNumber",nullptr,0);

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

    if(parts & TrackInfo::Properties)
    {
        int idx = av_find_best_stream(in, AVMEDIA_TYPE_AUDIO, -1, -1, nullptr, 0);
        if(idx >= 0)
        {
#if (LIBAVCODEC_VERSION_INT >= AV_VERSION_INT(57,48,0)) //ffmpeg-3.1:  57.48.101
            AVCodecParameters *c = in->streams[idx]->codecpar;
#else
            AVCodecContext *c = in->streams[idx]->codec;
#endif
            info->setValue(Qmmp::BITRATE, int(c->bit_rate) / 1000);
            info->setValue(Qmmp::SAMPLERATE, c->sample_rate);
            info->setValue(Qmmp::CHANNELS, c->channels);
            info->setValue(Qmmp::BITS_PER_SAMPLE, c->bits_per_raw_sample);

            //info->setValue(Qmmp::FORMAT_NAME, QString::fromLatin1(avcodec_get_name(c->codec_id)));
            info->setDuration(in->duration * 1000 / AV_TIME_BASE);
        }
    }

    avformat_close_input(&in);
    return QList<TrackInfo*>() << info;
}

MetaDataModel* FFVideoFactory::createMetaDataModel(const QString &path, bool readOnly)
{
    Q_UNUSED(readOnly);
    return new FFVideoMetaDataModel(path);
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

QString FFVideoFactory::translation() const
{
    return QLatin1String(":/ffvideo_plugin_");
}
