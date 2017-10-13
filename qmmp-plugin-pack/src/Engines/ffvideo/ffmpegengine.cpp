/***************************************************************************
 *   Copyright (C) 2008-2016 by Ilya Kotov                                 *
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

#include <QObject>
#include <QFile>
#include <QApplication>
#include <QAction>
#include <QKeyEvent>
#include <QMenu>
#include <QRegExp>
#include <QSettings>
#include <qmmp/fileinfo.h>
#include <qmmp/inputsource.h>
#include "packetbuffer.h"
#include "audiothread.h"
#include "videothread.h"
#include "videowindow.h"
#include "ffmpegengine.h"

#define AUDIO_BUFFER_SIZE 50
#define VIDEO_BUFFER_SIZE 50

FFmpegEngine::FFmpegEngine(EngineFactory *factory, QObject *parent)
    : AbstractEngine(parent)
{
    m_factory = factory;
    m_audioBuffer = new PacketBuffer(AUDIO_BUFFER_SIZE);
    m_videoBuffer = new PacketBuffer(VIDEO_BUFFER_SIZE);
    m_audioThread = new AudioThread(m_audioBuffer, this);
    m_videoThread = new VideoThread(m_videoBuffer, this);
    m_videoWindow = new VideoWindow(qApp->activeWindow());
    m_decoder = 0;
    reset();
    connect(m_videoWindow, SIGNAL(resizeRequest(QSize)), m_videoThread, SLOT(setWindowSize(QSize)));
    connect(m_videoWindow, SIGNAL(stopRequest()), SLOT(onStopRequest()));
}

FFmpegEngine::~FFmpegEngine()
{
    stop();
    delete m_audioBuffer;
    delete m_videoBuffer;
    if(!m_videoWindow.isNull())
        m_videoWindow->deleteLater();
}

bool FFmpegEngine::play()
{
    if(isRunning() || m_decoders.isEmpty() || m_audioThread->isRunning() || m_videoThread->isRunning())
        return false;

    if(!m_audioThread->initialize(m_decoders.head()))
        return false;

    if(!m_videoThread->initialize(m_decoders.head(), m_videoWindow.data()))
        return false;

    if(!m_videoWindow.isNull())
        m_videoWindow->show();
    start();
    return true;
}

bool FFmpegEngine::enqueue(InputSource *source)
{
    QStringList filters = m_factory->properties().filters;
    bool supports = false;
    foreach(QString filter, filters)
    {
        QRegExp regexp(filter, Qt::CaseInsensitive, QRegExp::Wildcard);
        if((supports = regexp.exactMatch(source->url())))
            break;
    }
    if(!supports)
        return false;

    FFVideoDecoder *decoder = new FFVideoDecoder();
    if(!decoder->initialize(source->url()))
        return false;

    if(source->ioDevice())
        source->ioDevice()->close();

    mutex()->lock();

    m_decoders.enqueue(decoder);
    m_inputs.insert(decoder, source);
    mutex()->unlock();
    if(!decoder->totalTime())
        source->setOffset(-1);
    source->setParent(this);
    return true;
}

void FFmpegEngine::seek(qint64 pos)
{
    if (isRunning())
    {
        mutex()->lock ();
        m_seekTime = pos;
        mutex()->unlock();
    }
}

void FFmpegEngine::stop()
{
    qDebug("%s", Q_FUNC_INFO);
    mutex()->lock ();
    m_user_stop = true;
    mutex()->unlock();

    if (m_audioThread->isRunning())
        m_audioBuffer->cond()->wakeAll();

    if (m_videoThread->isRunning())
        m_videoBuffer->cond()->wakeAll();


    if(isRunning())
        wait();

    if(!m_videoWindow.isNull())
        m_videoWindow->hide();

    m_audioThread->close();
    clearDecoders();
    reset();
}

void FFmpegEngine::pause()
{
    if(m_audioThread->isRunning() && m_videoThread->isRunning())
    {
        m_audioThread->pause();
        m_videoThread->pause();
        m_videoBuffer->cond()->wakeAll();
        m_audioBuffer->cond()->wakeAll();
    }
}

void FFmpegEngine::setMuted(bool muted)
{}

void FFmpegEngine::onStopRequest()
{
    stop();
}

void FFmpegEngine::run()
{
    AVPacket pkt;
    mutex()->lock ();
    m_metaData.clear();
    if(m_decoders.isEmpty())
    {
        mutex()->unlock ();
        return;
    }
    m_decoder = m_decoders.dequeue();
    m_seekTime = m_inputs.value(m_decoder)->offset(); //offset
    mutex()->unlock();
    m_audioThread->start();
    m_videoThread->start();
    StateHandler::instance()->dispatch(Qmmp::Buffering);
    StateHandler::instance()->dispatch(m_decoder->totalTime());
    StateHandler::instance()->dispatch(Qmmp::Playing);
    sendMetaData();

    while (!m_done && !m_finish)
    {
        mutex()->lock ();
        //seek
        if (m_seekTime >= 0)
        {
            m_audioBuffer->mutex()->lock ();
            m_audioBuffer->clear();
            m_audioBuffer->mutex()->unlock ();
            m_videoBuffer->mutex()->lock ();
            m_videoBuffer->clear();
            m_videoBuffer->mutex()->unlock ();
            m_decoder->seek(m_seekTime);
            m_videoThread->sync();
            m_seekTime = -1;
        }
        //metadata
        if(m_inputs[m_decoder]->hasMetaData())
        {
            QMap<Qmmp::MetaData, QString> m = m_inputs[m_decoder]->takeMetaData();
            m[Qmmp::URL] = m_inputs[m_decoder]->url();
            StateHandler::instance()->dispatch(m);
            m_metaData = QSharedPointer<QMap<Qmmp::MetaData, QString> >(new QMap<Qmmp::MetaData, QString>(m));
        }
        if(m_inputs[m_decoder]->hasStreamInfo())
            StateHandler::instance()->dispatch(m_inputs[m_decoder]->takeStreamInfo());

        // decode
        av_init_packet(&pkt);

        mutex()->unlock();
        int r = av_read_frame(m_decoder->formatContext(), &pkt);
        mutex()->lock();

        if(r == 0)
        {
            if(pkt.stream_index == m_decoder->audioIndex())
            {
                m_audioBuffer->mutex()->lock();
                while (m_audioBuffer->full() && !m_user_stop && !m_done)
                {
                    mutex()->unlock();
                    m_audioBuffer->cond()->wait(m_audioBuffer->mutex());
                    mutex()->lock();
                    m_done = !m_videoThread->isRunning();
                }

                if(m_user_stop)
                {
                    av_packet_unref(&pkt);
                    m_done = true;
                    m_audioBuffer->mutex()->unlock();
                    mutex()->unlock();
                    continue;
                }

                *m_audioBuffer->get() = pkt;
                m_audioBuffer->add();
                m_audioBuffer->mutex()->unlock();
                m_audioBuffer->cond()->wakeAll();
            }
            else if(pkt.stream_index == m_decoder->videoIndex())
            {
                m_videoBuffer->mutex()->lock();
                while (m_videoBuffer->full() && !m_user_stop && !m_done)
                {
                    mutex()->unlock();
                    m_videoBuffer->cond()->wait(m_videoBuffer->mutex());
                    mutex()->lock();
                    m_done = !m_videoThread->isRunning();
                }

                if(m_user_stop)
                {
                    av_packet_unref(&pkt);
                    m_done = true;
                    m_videoBuffer->mutex()->unlock();
                    mutex()->unlock();
                    continue;
                }

                *m_videoBuffer->get() = pkt;
                m_videoBuffer->add();
                m_videoBuffer->mutex()->unlock();
                m_videoBuffer->cond()->wakeAll();
            }
            else
            {
                av_packet_unref(&pkt);
            }
        }
        else if(!m_decoders.isEmpty())
        {
            m_audioThread->finish();
            m_videoThread->finish();

            m_audioBuffer->cond()->wakeAll();
            m_videoBuffer->cond()->wakeAll();

            if(m_audioThread->isRunning())
                m_audioThread->wait();

            if(m_videoThread->isRunning())
                m_videoThread->wait();

            QMetaObject::invokeMethod(m_videoWindow, "hide", Qt::QueuedConnection);

            StateHandler::instance()->sendFinished();
            StateHandler::instance()->dispatch(Qmmp::Stopped);

            m_inputs.take(m_decoder)->deleteLater ();
            delete m_decoder;
            m_decoder = m_decoders.dequeue();

            StateHandler::instance()->dispatch(Qmmp::Buffering);

            m_audioThread->initialize(m_decoder);
            m_videoThread->initialize(m_decoder, m_videoWindow);
            QMetaObject::invokeMethod(m_videoWindow, "show", Qt::QueuedConnection);


            m_audioThread->start();
            m_videoThread->start();

            StateHandler::instance()->dispatch(m_decoder->totalTime());
            StateHandler::instance()->dispatch(Qmmp::Playing);
            sendMetaData();
            m_seekTime = m_inputs.value(m_decoder)->offset(); //offset
        }
        else //end of stream
        {
            //write remaining audio data
            m_audioBuffer->mutex()->lock();
            while (!m_audioBuffer->empty() && !m_user_stop)
            {
                mutex()->unlock();
                m_audioBuffer->cond()->wait(m_audioBuffer->mutex());
                mutex()->lock();
            }
            m_audioBuffer->mutex()->unlock();
            m_audioBuffer->cond()->wakeAll();

            m_videoBuffer->mutex()->lock();
            while (!m_videoBuffer->empty() && m_user_stop)
            {
                mutex()->unlock();
                m_videoBuffer->cond()->wait(m_videoBuffer->mutex());
                mutex()->lock();
            }
            m_videoBuffer->mutex()->unlock();
            m_videoBuffer->cond()->wakeAll();

            m_done = true;
            m_finish = !m_user_stop;

        }
        mutex()->unlock();
    }
    mutex()->lock ();

    if(m_user_stop || (m_done && !m_finish))
    {
        m_audioThread->stop();
        m_videoThread->stop();
    }
    else if(m_finish)
    {
        m_audioThread->finish();
        m_videoThread->finish();
    }

    m_audioBuffer->cond()->wakeAll();
    m_videoBuffer->cond()->wakeAll();

    mutex()->unlock();

    if(m_audioThread->isRunning())
        m_audioThread->wait();

    if(m_videoThread->isRunning())
        m_videoThread->wait();

    m_audioThread->close();
    clearDecoders();
    if(m_finish && !m_user_stop)
        StateHandler::instance()->sendFinished();
    StateHandler::instance()->dispatch(Qmmp::Stopped);
    qDebug("FFmpegEngine: thread finished");
}

void FFmpegEngine::sendMetaData()
{
    if(!m_decoder || m_inputs.isEmpty())
        return;
    QString url = m_inputs.value(m_decoder)->url();
    if (QFile::exists(url)) //send metadata for local files only
    {
        QList <FileInfo *> list = m_factory->createPlayList(url, true, 0);
        if (!list.isEmpty())
        {
            StateHandler::instance()->dispatch(list[0]->metaData());
            m_metaData = QSharedPointer<QMap<Qmmp::MetaData, QString> >(new QMap<Qmmp::MetaData, QString>(list[0]->metaData()));
            while (!list.isEmpty())
                delete list.takeFirst();
        }
    }
}

void FFmpegEngine::clearDecoders()
{
    m_audioBuffer->clear();
    m_videoBuffer->clear();

    if(m_decoder)
    {
        m_inputs.take(m_decoder)->deleteLater ();
        delete m_decoder;
        m_decoder = 0;
    }
    while(!m_decoders.isEmpty())
    {
        FFVideoDecoder *d = m_decoders.dequeue();
        m_inputs.take(d)->deleteLater ();
        delete d;
    }
}

void FFmpegEngine::reset()
{
    m_done = false;
    m_finish = false;
    m_seekTime = -1;
    m_user_stop = false;
}
