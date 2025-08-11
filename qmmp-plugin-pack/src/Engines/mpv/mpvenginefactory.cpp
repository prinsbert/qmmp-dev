/***************************************************************************
 *   Copyright (C) 2025 by Ilya Kotov                                      *
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
#include <QDir>
extern "C"{
#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
#include <libavutil/dict.h>
}
#include "mpvengine.h"
#include "mpvmetadatamodel.h"
#include "mpvenginefactory.h"


// MpvEngineFactory

MpvEngineFactory::MpvEngineFactory()
{
#if (LIBAVCODEC_VERSION_INT < AV_VERSION_INT(58,10,100)) //ffmpeg-3.5
    avcodec_register_all();
    avformat_network_init();
    av_register_all();
#endif
}

EngineProperties MpvEngineFactory::properties() const
{
    EngineProperties properties;
    properties.name = tr("MPV Plugin");
    properties.shortName = "mpv"_L1;
    properties.filters = QStringList { u"*.avi"_s, u"*.mpg"_s, u"*.mpeg"_s, u"*.divx"_s, u"*.qt"_s,
            u"*.mov"_s, u"*.wmv"_s, u"*.asf"_s, u"*.flv"_s, u"*.3gp"_s, u"*.mkv"_s, u"*.mp4"_s, u"*.webm"_s };
    properties.description = tr("Video Files");
    //properties.contentType = "application/ogg;audio/x-vorbis+ogg";
    properties.protocols = QStringList { u"file"_s };
    properties.hasAbout = true;
    properties.hasSettings = false;
    return properties;
}

bool MpvEngineFactory::supports(const QString &source) const
{
    return QDir::match(properties().filters, source.section(QLatin1Char('/'), -1));
}

AbstractEngine *MpvEngineFactory::create(QObject *parent)
{
    return new MpvEngine(this, parent);
}

QList<TrackInfo *> MpvEngineFactory::createPlayList(const QString &path, TrackInfo::Parts parts, QStringList *)
{
    TrackInfo *info = new TrackInfo(path);

    if(parts == TrackInfo::Parts())
        return QList<TrackInfo*>() << info;

    AVFormatContext *in = nullptr;

#ifdef Q_OS_WIN
    if(avformat_open_input(&in, path.toUtf8().constData(), nullptr, nullptr) < 0)
#else
    if(avformat_open_input(&in, path.toLocal8Bit().constData(), nullptr, nullptr) < 0)
#endif
    {
        qCDebug(plugin) << "unable to open file";
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
        int videoIndex = av_find_best_stream(in, AVMEDIA_TYPE_VIDEO, -1, -1, nullptr, 0);
        int audioIndex = av_find_best_stream(in, AVMEDIA_TYPE_AUDIO, -1, -1, nullptr, 0);

        //select default stream for audio
        for(unsigned int i = 0; i < in->nb_streams; ++i)
        {
            if(in->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_AUDIO &&
                    in->streams[i]->disposition & AV_DISPOSITION_DEFAULT)
            {
                audioIndex = i;
                break;
            }
        }

        QStringList codecs;

        if(videoIndex >= 0)
        {
            AVCodecParameters *c = in->streams[videoIndex]->codecpar;
            const AVCodec *codec = avcodec_find_decoder(c->codec_id);
            if(codec)
                codecs << QString::fromLatin1(codec->name);
        }

        if(audioIndex >= 0)
        {
            AVCodecParameters *c = in->streams[audioIndex]->codecpar;
            info->setValue(Qmmp::BITRATE, int(c->bit_rate) / 1000);
            info->setValue(Qmmp::SAMPLERATE, c->sample_rate);
#if (LIBAVCODEC_VERSION_INT >= AV_VERSION_INT(59,37,100)) //ffmpeg-5.1
            info->setValue(Qmmp::CHANNELS, c->ch_layout.nb_channels);
#else
            info->setValue(Qmmp::CHANNELS, c->channels);
#endif
            info->setValue(Qmmp::BITS_PER_SAMPLE, av_get_bytes_per_sample(static_cast<AVSampleFormat>(c->format)) * 8);
            info->setDuration(in->duration * 1000 / AV_TIME_BASE);

            const AVCodec *codec = avcodec_find_decoder(c->codec_id);
            if(codec)
                codecs << QString::fromLatin1(codec->name);
        }

        info->setValue(Qmmp::FORMAT_NAME, codecs.join(QLatin1Char('+')));
        info->setValue(Qmmp::DECODER, u"mpv"_s);
    }

    avformat_close_input(&in);
    return QList<TrackInfo*>() << info;
}

MetaDataModel* MpvEngineFactory::createMetaDataModel(const QString &path, bool readOnly)
{
    Q_UNUSED(readOnly);
    return new MpvMetaDataModel(path);
}

QDialog *MpvEngineFactory::createSettings(QWidget *parent)
{
    Q_UNUSED(parent);
    return nullptr;
}

void MpvEngineFactory::showAbout(QWidget *parent)
{
    QMessageBox::about(parent, tr("About MPV Plugin"),
                       tr("Qmmp MPV Plugin") + QChar::LineFeed +
                       tr("This plugin uses mpv as backend") + QChar::LineFeed +
                       tr("Written by: Ilya Kotov <forkotov02@ya.ru>"));
}

QString MpvEngineFactory::translation() const
{
    return QLatin1String(":/mpv_plugin_");
}
