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

#include <QtDebug>
#include <qmmp/output.h>
#include "packetbuffer.h"
#include "audiothread.h"


AudioThread::AudioThread(PacketBuffer *buf, QObject *parent) :
    QThread(parent)
{
    m_buffer = buf;
    m_output = 0;
}

bool AudioThread::initialize(AVCodecContext *ctx)
{
    m_context = ctx;
    m_output = Output::create();

    if(!m_output)
    {
        qWarning("AudioThread: unable to create output");
        return false;
    }

    Qmmp::AudioFormat format = Qmmp::PCM_UNKNOWM;

    switch(m_context->sample_fmt)
    {
    case AV_SAMPLE_FMT_U8:
    case AV_SAMPLE_FMT_U8P:
        format = Qmmp::PCM_S8;
        break;
    case AV_SAMPLE_FMT_S16:
    case AV_SAMPLE_FMT_S16P:
        format = Qmmp::PCM_S16LE;
        break;
    case AV_SAMPLE_FMT_S32:
    case AV_SAMPLE_FMT_S32P:
        format = Qmmp::PCM_S32LE;
        break;
    case AV_SAMPLE_FMT_FLT:
    case AV_SAMPLE_FMT_FLTP:
        format = Qmmp::PCM_FLOAT;
        break;
    default:
        qWarning("AudioThread: unsupported audio format");
        return false;
    }

    qDebug() << m_context->sample_rate << m_context->channels << format << Qmmp::PCM_FLOAT;

    bool ok = m_output->initialize(44100, ChannelMap(2), Qmmp::PCM_S16LE);
    if(!ok)
    {
        qWarning("AudioThread: unable to initialize output");
        return false;
    }

    return ok;
}

void AudioThread::run()
{
    qDebug("%s", Q_FUNC_INFO);
    bool done = false;
    AVFrame *frame = av_frame_alloc();
    AVFrame *oframe = av_frame_alloc();
    SwrContext *swr = swr_alloc_set_opts(NULL,                      // we're allocating a new context
                                         AV_CH_LAYOUT_STEREO,       // out_ch_layout
                                         AV_SAMPLE_FMT_S16,         // out_sample_fmt
                                         44100,                     // out_sample_rate
                                         m_context->channel_layout, // in_ch_layout
                                         m_context->sample_fmt,     // in_sample_fmt
                                         m_context->sample_rate,    // in_sample_rate
                                         0, NULL);

    while (!done)
    {
        m_buffer->mutex()->lock();

        while (m_buffer->empty() && !done)
        {
            qDebug("++++");
            m_buffer->cond()->wakeOne();
            m_buffer->cond()->wait(m_buffer->mutex());
            qDebug("++++1");
        }

        AVPacket *p = m_buffer->next();

        if(avcodec_send_packet(m_context, p) == 0)
        {
            m_buffer->done();
        }

        m_buffer->cond()->wakeAll();
        m_buffer->mutex()->unlock();

        if(avcodec_receive_frame(m_context, frame) == 0)
        {
            oframe->channel_layout = AV_CH_LAYOUT_STEREO;
            oframe->sample_rate = 44100;
            oframe->format = AV_SAMPLE_FMT_S16;
            if(swr_convert_frame(swr, oframe, frame) != 0)
            {
                qWarning("AudioThread: swr_convert_frame failed!");
                break;
            }

            int size = oframe->nb_samples * 4;
            unsigned char *data = oframe->data[0];

            while (size > 0)
            {
                int l = m_output->writeAudio(data, size);
                if(l > 0)
                {
                    size -= l;
                    data += l;
                }
                else if(l == 0)
                    usleep(50);
                else
                {
                    qWarning("error!");
                    break;
                }
            }

            av_frame_unref(oframe);
        }
    }
    av_frame_free(&frame);
}
