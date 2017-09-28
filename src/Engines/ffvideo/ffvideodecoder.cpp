/***************************************************************************
 *   Copyright (C) 2017 by Ilya Kotov                                      *
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

#include "ffvideodecoder.h"

FFVideoDecoder::FFVideoDecoder()
{
   m_formatContext = 0;
   m_audioCodecContext = 0;
   m_videoCodecContext = 0;
   m_audioIndex = 0;
   m_videoIndex = 0;
   m_totalTime = 0;
}

FFVideoDecoder::~FFVideoDecoder()
{
    if(m_audioCodecContext)
        avcodec_free_context(&m_audioCodecContext);
    if(m_videoCodecContext)
        avcodec_free_context(&m_videoCodecContext);
    if(m_formatContext)
        avformat_free_context(m_formatContext);
}

bool FFVideoDecoder::initialize(const QString &path)
{
    int err = 0;
    char errbuf[AV_ERROR_MAX_STRING_SIZE] = {0};

    if((err = avformat_open_input(&m_formatContext, path.toLocal8Bit().constData(), 0, 0)) != 0)
    {
        av_strerror(err, errbuf, sizeof(errbuf));
        qWarning("FFVideoDecoder: avformat_open_input() failed: %s", errbuf);
        return false;
    }
    if((err = avformat_find_stream_info(m_formatContext, 0)) < 0)
    {
        av_strerror(err, errbuf, sizeof(errbuf));
        qWarning("FFVideoDecoder: avformat_find_stream_info() failed: %s", errbuf);
        return false;
    }

    av_dump_format(m_formatContext,0,0,0);

    m_audioIndex = av_find_best_stream(m_formatContext, AVMEDIA_TYPE_AUDIO, -1, -1, 0, 0);
    m_videoIndex = av_find_best_stream(m_formatContext, AVMEDIA_TYPE_VIDEO, -1, -1, 0, 0);

    if((err = m_audioIndex) < 0)
    {
        av_strerror(err, errbuf, sizeof(errbuf));
        qWarning("FFVideoDecoder: unable to find audio stream: %s", errbuf);
        return false;
    }
    if((err = m_videoIndex) < 0)
    {
        av_strerror(err, errbuf, sizeof(errbuf));
        qWarning("FFVideoDecoder: unable to find video stream: %s", errbuf);
        return false;
    }

    AVCodec *audioCodec = avcodec_find_decoder (m_formatContext->streams[m_audioIndex]->codecpar->codec_id);
    AVCodec *videoCodec = avcodec_find_decoder (m_formatContext->streams[m_videoIndex]->codecpar->codec_id);

    if(!audioCodec || !videoCodec)
    {
        qWarning("FFVideoDecoder: unable to find codec");
        return false;
    }

    m_audioCodecContext = avcodec_alloc_context3(NULL);
    avcodec_parameters_to_context (m_audioCodecContext, m_formatContext->streams[m_audioIndex]->codecpar);

    m_videoCodecContext = avcodec_alloc_context3(NULL);
    avcodec_parameters_to_context (m_videoCodecContext, m_formatContext->streams[m_videoIndex]->codecpar);

    if ((err = avcodec_open2(m_audioCodecContext, audioCodec, 0)) < 0)
    {
        av_strerror(err, errbuf, sizeof(errbuf));
        qWarning("FFVideoDecoder: avcodec_open2() failed: %s", errbuf);
        return false;
    }

    if ((err = avcodec_open2(m_videoCodecContext, videoCodec, 0)) < 0)
    {
        av_strerror(err, errbuf, sizeof(errbuf));
        qWarning("FFVideoDecoder: avcodec_open2() failed: %s", errbuf);
        return false;
    }

    m_totalTime = m_formatContext->duration * 1000 / AV_TIME_BASE;
    return true;
}

qint64 FFVideoDecoder::totalTime() const
{
    return m_totalTime;
}

AVFormatContext *FFVideoDecoder::formatContext() const
{
    return m_formatContext;
}

AVCodecContext *FFVideoDecoder::audioCodecContext() const
{
    return m_audioCodecContext;
}

AVCodecContext *FFVideoDecoder::videoCodecContext() const
{
    return m_videoCodecContext;
}

int FFVideoDecoder::audioIndex() const
{
    return m_audioIndex;
}

int FFVideoDecoder::videoIndex() const
{
    return m_videoIndex;
}
