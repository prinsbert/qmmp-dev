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
#include <qmmp/statehandler.h>
#include "packetbuffer.h"
#include "ffvideodecoder.h"
#include "audiothread.h"


AudioThread::AudioThread(PacketBuffer *buf, QObject *parent) :
    QThread(parent)
{
    m_buffer = buf;
    m_output = 0;
    m_user_stop = false;
    m_finish = false;
    m_pause = false;
    m_prev_pause = false;
    m_stream = 0;
    m_muted = false;
}

AudioThread::~AudioThread()
{
    close();
}

bool AudioThread::initialize(FFVideoDecoder *decoder)
{
    close();
    m_context = decoder->audioCodecContext();
    m_stream = decoder->formatContext()->streams[decoder->audioIndex()];
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

void AudioThread::stop()
{
    m_mutex.lock();
    m_user_stop = true;
    m_mutex.unlock();
}

void AudioThread::finish()
{
    m_mutex.lock();
    m_finish = true;
    m_mutex.unlock();
}

void AudioThread::pause()
{
    m_mutex.lock();
    m_pause = !m_pause;
    m_mutex.unlock();
    Qmmp::State state = m_pause ? Qmmp::Paused: Qmmp::Playing;
    StateHandler::instance()->dispatch(state);
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

void AudioThread::setMuted(bool muted)
{
    m_mutex.lock();
    m_muted = muted;
    m_mutex.unlock();
}

void AudioThread::run()
{
    bool done = false;
    int err = 0;
    char errbuf[AV_ERROR_MAX_STRING_SIZE] = {0};
    m_user_stop = false;
    m_finish = false;
    m_pause = false;
    m_prev_pause = false;
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
        m_mutex.lock ();
        if(m_pause != m_prev_pause)
        {
            if(m_pause)
            {
                //Visual::clearBuffer();
                m_output->suspend();
                m_mutex.unlock();
                m_prev_pause = m_pause;
                continue;
            }
            else
                m_output->resume();
            m_prev_pause = m_pause;
        }
        m_buffer->mutex()->lock();
        done = m_user_stop || (m_finish && m_buffer->empty());

        while (!done && (m_buffer->empty() || m_pause))
        {
            m_mutex.unlock ();
            m_buffer->cond()->wait(m_buffer->mutex());
            m_mutex.lock ();
            done = m_user_stop || m_finish;
        }

        if(m_user_stop)
        {
            done = true;
            m_buffer->mutex()->unlock();
            m_mutex.unlock();
            continue;
        }

        m_mutex.unlock();


        AVPacket *p = m_buffer->next();

        if(!p)
        {
            m_buffer->mutex()->unlock();
            m_buffer->cond()->wakeOne();
            continue;
        }

        if((err = avcodec_send_packet(m_context, p)) == 0)
        {
            m_buffer->done();
        }
        else
        {
            m_buffer->done();
            av_strerror(err, errbuf, sizeof(errbuf));
            qWarning("AudioThread: avcodec_send_packet failed: %s", errbuf);
        }

        m_buffer->mutex()->unlock();
        m_buffer->cond()->wakeAll();

        if((err = avcodec_receive_frame(m_context, frame)) == 0)
        {
            oframe->channel_layout = AV_CH_LAYOUT_STEREO;
            oframe->sample_rate = 44100;
            oframe->format = AV_SAMPLE_FMT_S16;
            oframe->pts = frame->pts;
            if((err = swr_convert_frame(swr, oframe, frame)) != 0)
            {
                av_strerror(err, errbuf, sizeof(errbuf));
                qWarning("AudioThread: swr_convert_frame failed: %s", errbuf);
                continue;
            }

            int size = oframe->nb_samples * 4;
            unsigned char *data = oframe->data[0];

            m_mutex.lock();
            if(m_muted)
                memset(data, 0, size);
            m_mutex.unlock();

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

            StateHandler::instance()->dispatch(frame->pts  * 1000 * av_q2d(m_stream->time_base),
                                               m_context->bit_rate / 1000, 44100, 16, 2);
            av_frame_unref(oframe);
        }
        else
        {
            //err = AVERROR(EAGAIN);
            av_strerror(err, errbuf, sizeof(errbuf));
            qWarning("AudioThread: avcodec_receive_frame failed: %s", errbuf);
        }
    }
    m_buffer->cond()->wakeAll();
    av_frame_free(&frame);
    if(m_finish)
        m_output->drain();
    qDebug("AudioThread: finished");
}
