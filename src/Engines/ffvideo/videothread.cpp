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

#include <QImage>
#include <QElapsedTimer>
#include "packetbuffer.h"
#include "videowindow.h"
#include "ffvideodecoder.h"
#include "videothread.h"

VideoThread::VideoThread(PacketBuffer *buf, QObject *parent) :
    QThread(parent)
{
    m_buffer = buf;
    m_output = 0;
    m_user_stop = false;
    m_finish = false;
    m_pause = false;
    m_prev_pause = false;
}

bool VideoThread::initialize(FFVideoDecoder *decoder, VideoWindow *w)
{
    m_context = decoder->videoCodecContext();
    m_stream = decoder->formatContext()->streams[decoder->videoIndex()];
    m_videoWindow = w;
    m_window_size = QSize(m_context->width, m_context->height);
    m_videoWindow->resize(m_window_size); //TODO thread safe
    return true;
}

void VideoThread::stop()
{
    m_mutex.lock();
    m_user_stop = true;
    m_mutex.unlock();
}

void VideoThread::finish()
{
    m_mutex.lock();
    m_finish = true;
    m_mutex.unlock();
}

void VideoThread::pause()
{
    m_mutex.lock();
    m_pause = !m_pause;
    m_mutex.unlock();
}

void VideoThread::run()
{
    QElapsedTimer t;
    bool done = false;
    m_user_stop = false;
    m_finish = false;
    m_pause = false;
    m_prev_pause = false;
    int ms = 0;

    AVFrame *frameRGB = av_frame_alloc();
    AVFrame *frame = av_frame_alloc();

    double ratio = 1;//qMin(double(m_window_size.width()) / m_context->width, double(m_window_size.height()) / m_context->height);

    av_image_alloc(frameRGB->data, frameRGB->linesize, m_context->width * ratio, m_context->height * ratio, AV_PIX_FMT_RGB24, 32);
    t.start();

    SwsContext *sws = sws_getContext (
                m_context->width,
                m_context->height,
                m_context->pix_fmt,
                m_context->width * ratio,
                m_context->height * ratio,
                AV_PIX_FMT_RGB24,
                SWS_BICUBIC, 0, 0, 0);

    while (!done)
    {
        m_mutex.lock ();
        if(m_pause != m_prev_pause)
        {
            if(m_pause)
            {
                m_mutex.unlock();
                m_prev_pause = m_pause;
                ms += t.elapsed();
                continue;
            }
            else
            {
                t.restart();
            }
            m_prev_pause = m_pause;
        }
        m_buffer->mutex()->lock();
        done = m_user_stop || (m_finish && m_buffer->empty());

        while (!done && (m_buffer->empty() || m_pause))
        {
            m_mutex.unlock ();
            m_buffer->cond()->wait(m_buffer->mutex());
            m_mutex.lock();
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

        if(p->pts * 1000 * av_q2d(m_stream->time_base) > ms + t.elapsed())
        {
            m_buffer->mutex()->unlock();
            m_buffer->cond()->wakeAll();
            usleep(50);
            continue;
        }

        if(avcodec_send_packet(m_context, p) == 0)
        {
            m_buffer->done();
        }
        else
            qDebug("fail 1 %s", Q_FUNC_INFO);

        m_buffer->mutex()->unlock();
        m_buffer->cond()->wakeAll();

        if(avcodec_receive_frame(m_context, frame) == 0)
        {
            int r = sws_scale(sws, frame->data, frame->linesize, 0, frame->height, frameRGB->data, frameRGB->linesize);
            QImage img(frameRGB->data[0], m_context->width * ratio, m_context->height * ratio, frameRGB->linesize[0], QImage::Format_RGB888);
            m_videoWindow->addImage(img);
            av_frame_unref(frame);

        }
    }

    av_frame_free(&frame);
    av_frame_free(&frameRGB);
    sws_freeContext(sws);
    qDebug("VideoThread: finished");
}
QSize VideoThread::windowSize() const
{
    return m_window_size;
}

void VideoThread::setWindowSize(const QSize &windowSize)
{
    m_window_size = windowSize;
}

