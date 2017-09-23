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
#include <QTime>
#include "packetbuffer.h"
#include "videowindow.h"
#include "videothread.h"

VideoThread::VideoThread(PacketBuffer *buf, QObject *parent) :
    QThread(parent)
{
    m_buffer = buf;
    m_output = 0;
}

bool VideoThread::initialize(AVCodecContext *ctx, AVStream *s, VideoWindow *w)
{
    m_context = ctx;
    m_w = w;
    m_stream = s;
    return true;
}

void VideoThread::run()
{
    qDebug("%s", Q_FUNC_INFO);

    //img.fill(Qt::red);


    AVFrame* frameRGB = av_frame_alloc();
    av_image_alloc(frameRGB->data, frameRGB->linesize, m_context->width, m_context->height, AV_PIX_FMT_RGB24, 32);

//    avpicture_alloc((AVPicture*)frameRGB,
//                    AV_PIX_FMT_ARGB,
//                    m_context->width,
//                    m_context->height);




    QTime t;
    t.start();


    bool done = false;
    AVFrame *frame = av_frame_alloc();
    //AVFrame *oframe = av_frame_alloc();
    SwsContext *sws = sws_getContext (
                m_context->width,
                m_context->height,
                m_context->pix_fmt,
                m_context->width,
                m_context->height,
                AV_PIX_FMT_RGB24,
                SWS_BICUBIC, 0, 0, 0);

    while (!done)
    {
        qDebug("lock");
        m_buffer->mutex()->lock();

        while (m_buffer->empty() && !done)
        {
            m_buffer->cond()->wakeOne();
            qDebug("wait 1");
            m_buffer->cond()->wait(m_buffer->mutex());
            qDebug("wait 2");
        }

        AVPacket *p = m_buffer->next();





        if(p->pts * 1000 * av_q2d(m_stream->time_base) > t.elapsed())
        {

            m_buffer->cond()->wakeAll();
            m_buffer->mutex()->unlock();
            usleep(50);
            qDebug("+++c");
            continue;
        }

        qDebug("1");


        if(avcodec_send_packet(m_context, p) == 0)
        {
            m_buffer->done();
        }
        else
            qDebug("fail 1");

        m_buffer->cond()->wakeAll();
        m_buffer->mutex()->unlock();

        qDebug("2");

        if(avcodec_receive_frame(m_context, frame) == 0)
        {
            //const int pitch[] = { 1000 };
            //uint8_t *pix[] = { img.bits() };
            int r = sws_scale(sws, frame->data, frame->linesize, 0, frame->height, frameRGB->data, frameRGB->linesize);
            QImage img(frameRGB->data[0], m_context->width, m_context->height, frameRGB->linesize[0], QImage::Format_RGB888);
            m_w->addImage(img);
            av_frame_unref(frame);
        }
        else
            qDebug("fail 2");
        qDebug("3");
    }
    av_frame_free(&frame);
    sws_freeContext(sws);
}
