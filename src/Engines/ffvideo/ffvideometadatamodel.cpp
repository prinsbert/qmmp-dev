/***************************************************************************
 *   Copyright (C) 2017-2018 by Ilya Kotov                                 *
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

#include <stdint.h>
#include "ffvideometadatamodel.h"

FFVideoMetaDataModel::FFVideoMetaDataModel(const QString &path)
    : MetaDataModel(true, MetaDataModel::COMPLETE_PROPERTY_LIST)
{
    m_in = 0;
#ifdef Q_OS_WIN
    if (avformat_open_input(&m_in, path.toUtf8().constData(), 0, 0) < 0)
#else
    if (avformat_open_input(&m_in, path.toLocal8Bit().constData(), 0, 0) < 0)
#endif
        return;
    avformat_find_stream_info(m_in, 0);
    av_read_play(m_in);
}

FFVideoMetaDataModel::~FFVideoMetaDataModel()
{
    if(m_in)
        avformat_close_input(&m_in);
}

QList<MetaDataItem> FFVideoMetaDataModel::extraProperties() const
{
    QList<MetaDataItem> ep;
    if(!m_in)
        return ep;
    QString text = QString("%1").arg(int(m_in->duration/AV_TIME_BASE)/60);
    text +=":"+QString("%1").arg(int(m_in->duration/AV_TIME_BASE)%60,2,10,QChar('0'));
    ep << MetaDataItem(tr("Length"), text);
    ep << MetaDataItem(tr("File size"), quint64(avio_size(m_in->pb) / 1024), tr("KiB"));
    ep << MetaDataItem(tr("Bitrate"), quint64(m_in->bit_rate / 1000), tr("kbps"));

    int audioIndex = av_find_best_stream(m_in, AVMEDIA_TYPE_AUDIO, -1, -1, 0, 0);
    int videoIndex = av_find_best_stream(m_in, AVMEDIA_TYPE_VIDEO, -1, -1, 0, 0);

    if(audioIndex >= 0)
    {
         AVCodecParameters *c = m_in->streams[audioIndex]->codecpar;
         ep << MetaDataItem(tr("Audio bitrate"), qint64(c->bit_rate / 1000), tr("kbps"));
         ep << MetaDataItem(tr("Audio sample rate"), c->sample_rate, tr("Hz"));
         ep << MetaDataItem(tr("Audio channels"), c->channels);
    }

    if(videoIndex >= 0)
    {
         AVCodecParameters *c = m_in->streams[videoIndex]->codecpar;
         ep << MetaDataItem(tr("Video size"), QString("%1x%2").arg(c->width).arg(c->height));
         ep << MetaDataItem(tr("Video bitrate"), qint64(c->bit_rate / 1000), tr("kbps"));
    }
    return ep;
}
