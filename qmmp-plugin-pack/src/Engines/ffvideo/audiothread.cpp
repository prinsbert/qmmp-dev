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
#include "ffvideodecoder.h"
#include "audiothread.h"


AudioThread::AudioThread(PacketBuffer *buf, QObject *parent) :
    QThread(parent)
{
    m_buffer = buf;
    m_output = 0;
    m_user_stop = false;
}

AudioThread::~AudioThread()
{
    close();
}

bool AudioThread::initialize(FFVideoDecoder *decoder)
{
    close();
    m_context = decoder->audioCodecContext();
    m_output = Output::create();

    if(!m_output)
    {
        qWarning("AudioThread: unable to create output");
        return false;
    }

    if(!m_output->initialize(44100, ChannelMap(2), Qmmp::PCM_S16LE))
    {
        close();
        qWarning("AudioThread: unable to initialize output");
        return false;
    }
    return true;
}

QMutex *AudioThread::mutex()
{
    return &m_mutex;
}

void AudioThread::stop()
{
    m_user_stop = true;
}

void AudioThread::close()
{
    if(isRunning())
    {
        qWarning("AudioThread: unable to close active output");
        return;
    }

    if(m_output)
    {
        delete m_output;
        m_output = 0;
    }
}

void AudioThread::run()
{
    bool done = false;
    m_user_stop = false;
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
        mutex()->lock ();
        m_buffer->mutex()->lock();
        while (m_buffer->empty() && !m_user_stop)
        {
            mutex()->unlock ();
            m_buffer->cond()->wait(m_buffer->mutex());
            mutex()->lock ();
        }

        if(m_user_stop)
        {
            done = true;
            m_buffer->mutex()->unlock();
            mutex()->unlock();
            continue;
        }

        mutex()->unlock();


        AVPacket *p = m_buffer->next();

        if(avcodec_send_packet(m_context, p) == 0)
        {
            m_buffer->done();
        }

        m_buffer->mutex()->unlock();
        m_buffer->cond()->wakeAll();


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
    qDebug("AudioThread: finished");
}
