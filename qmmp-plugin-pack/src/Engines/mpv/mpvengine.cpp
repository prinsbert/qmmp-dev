/***************************************************************************
 *   Copyright (C) 2008-2026 by Ilya Kotov                                 *
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

#include <QObject>
#include <QFile>
#include <QApplication>
#include <QAction>
#include <QKeyEvent>
#include <QMenu>
#include <QRegularExpression>
#include <QSettings>
#include <QFileInfo>
#include <QDir>
#include <QMetaObject>
#include <qmmp/trackinfo.h>
#include <qmmp/inputsource.h>
#include <qmmp/volumehandler.h>
#include "mpvengine.h"

MpvEngine::MpvEngine(EngineFactory *factory, QObject *parent)
        : AbstractEngine(parent),
          m_factory(factory)
{
    connect(VolumeHandler::instance(), &VolumeHandler::mutedChanged, this, &MpvEngine::setMuted);
}

MpvEngine::~MpvEngine()
{
    delete m_source;

    while(!m_sources.isEmpty())
        m_sources.dequeue()->deleteLater();
}

bool MpvEngine::play()
{
    m_user_stop = false;

    if(!m_source)
        return false;

    if(!m_ctx)
        initialize();

    QByteArray path = m_source->path().toLocal8Bit();
    const char *cmd[] = { "loadfile", path.constData(), nullptr };
    mpv_command(m_ctx, cmd);
    return true;
}

bool MpvEngine::enqueue(InputSource *source)
{
    if(!m_factory->supports(source->path()))
        return false;

    if(!m_ctx)
        m_source = source;
    else
        m_sources.enqueue(source);
    return true;
}

bool MpvEngine::initialize()
{
    setlocale(LC_NUMERIC, "C");

    m_ctx = mpv_create();

    mpv_set_option_string(m_ctx, "input-default-bindings", "yes");
    mpv_set_option_string(m_ctx, "input-vo-keyboard", "yes");
    int val = 1;
    mpv_set_option(m_ctx, "osc", MPV_FORMAT_FLAG, &val);
    mpv_observe_property(m_ctx, 0, "time-pos", MPV_FORMAT_DOUBLE);
    mpv_observe_property(m_ctx, 0, "duration", MPV_FORMAT_DOUBLE);
    mpv_observe_property(m_ctx, 0, "pause", MPV_FORMAT_FLAG);
    mpv_observe_property(m_ctx, 0, "audio-bitrate", MPV_FORMAT_DOUBLE);

    mpv_observe_property(m_ctx, 0, "track-list", MPV_FORMAT_NODE);
    mpv_observe_property(m_ctx, 0, "chapter-list", MPV_FORMAT_NODE);
    mpv_set_wakeup_callback(m_ctx, MpvEngine::wakeup, this);
    mpv_request_log_messages(m_ctx, "error");

    mpv_initialize(m_ctx);

    return true;
}

void MpvEngine::seek(qint64 pos)
{
    if(m_ctx)
    {
        const char *cmd[] = { "seek",  QString::number(pos / 1000).toLatin1().constData(), "absolute", nullptr };
        mpv_command(m_ctx, cmd);
    }
}

void MpvEngine::stop()
{
    if(m_ctx)
    {
        mpv_terminate_destroy(m_ctx);
        m_ctx = nullptr;
        StateHandler::instance()->dispatch(Qmmp::Stopped);
    }

    delete m_source;
    m_source = nullptr;

    while(!m_sources.isEmpty())
        m_sources.dequeue()->deleteLater();
}

void MpvEngine::pause()
{
    if(m_ctx)
    {
        int flag = 0;
        mpv_get_property(m_ctx, "pause",  MPV_FORMAT_FLAG, &flag);
        flag = flag == 1 ? 0 : 1;
        mpv_set_property(m_ctx, "pause",  MPV_FORMAT_FLAG, &flag);
    }
}

void MpvEngine::setMuted(bool muted)
{
    if(m_ctx)
    {
        mpv_set_property_string(m_ctx, "mute", muted ? "yes" : "no");
    }
}

void MpvEngine::onError(QProcess::ProcessError error)
{
    if(error == QProcess::FailedToStart || error == QProcess::Crashed)
        StateHandler::instance()->dispatch(Qmmp::FatalError);
    qCWarning(plugin, "process error: %d", error);
}

void MpvEngine::sendMetaData()
{
    QList<TrackInfo> infoList = m_factory->createPlayList(m_source->path(), TrackInfo::AllParts, nullptr);
    if(!infoList.isEmpty() && QFileInfo::exists(infoList.constFirst().path()))
    {
        TrackInfo info = infoList.takeFirst();
        info.setValue(Qmmp::DECODER, m_factory->properties().shortName);
        info.setValue(Qmmp::FILE_SIZE, QFileInfo(info.path()).size());
        StateHandler::instance()->dispatch(info);
        AudioParameters ap(info.value(Qmmp::SAMPLERATE).toInt(), ChannelMap(info.value(Qmmp::CHANNELS).toInt()),
                           AudioParameters::findAudioFormat(info.value(Qmmp::BITS_PER_SAMPLE).toInt()));
        StateHandler::instance()->dispatch(ap);
    }
}

void MpvEngine::processEvents()
{
    while(m_ctx)
    {
        mpv_event *event = mpv_wait_event(m_ctx, 0);
        if(event->event_id == MPV_EVENT_NONE)
            break;

        switch(event->event_id)
        {
        case MPV_EVENT_LOG_MESSAGE:
        {
            mpv_event_property *prop = (mpv_event_property *)event->data;
            qCDebug(plugin) << (char *)prop->data;
            break;
        }
        case MPV_EVENT_PROPERTY_CHANGE:
        {
            mpv_event_property *prop = (mpv_event_property *)event->data;

            if(!strcmp(prop->name, "time-pos"))
            {
                if(prop->format == MPV_FORMAT_DOUBLE)
                {
                    m_currentTime = *(double *)prop->data * 1000.0;
                    StateHandler::instance()->dispatch(m_currentTime, m_bitrate);
                }
            }
            else if(!strcmp(prop->name, "pause"))
            {
                if(prop->format == MPV_FORMAT_FLAG && (StateHandler::instance()->state() == Qmmp::Paused ||
                                                       StateHandler::instance()->state() == Qmmp::Playing))
                {
                    int paused = *(int *)prop->data;
                    StateHandler::instance()->dispatch(paused ? Qmmp::Paused : Qmmp::Playing);
                }
            }
            else if(!strcmp(prop->name, "duration"))
            {
                if(prop->format == MPV_FORMAT_DOUBLE)
                {
                    m_duration = (*(double *)prop->data) * 1000;
                    StateHandler::instance()->dispatch(m_duration);
                }
            }
            else if(!strcmp(prop->name, "audio-bitrate"))
            {
                if(prop->format == MPV_FORMAT_DOUBLE)
                {
                    m_bitrate = (*(double *)prop->data) / 1000;
                    StateHandler::instance()->dispatch(m_currentTime, m_bitrate);
                }
            }
            break;
        }
        case MPV_EVENT_START_FILE:
            StateHandler::instance()->dispatch(Qmmp::Buffering);
            setMuted(VolumeHandler::instance()->isMuted());
            break;
        case MPV_EVENT_FILE_LOADED:
            StateHandler::instance()->dispatch(Qmmp::Playing);
            sendMetaData();
            break;
        case MPV_EVENT_END_FILE:
        {
            if(m_sources.isEmpty())
            {
                stop();
            }
            else
            {
                StateHandler::instance()->sendFinished();
                StateHandler::instance()->dispatch(Qmmp::Stopped);
                delete m_source;
                m_source = m_sources.dequeue();
                const char *cmd[] = { "loadfile", m_source->path().toLocal8Bit().constData(), nullptr };
                mpv_command(m_ctx, cmd);
            }
            break;
        }
        case MPV_EVENT_AUDIO_RECONFIG:
        {
            int64_t channels = 0;
            int64_t samplerate = 0;
            mpv_get_property(m_ctx, "audio-out-params/channel-count", MPV_FORMAT_INT64, &channels);
            mpv_get_property(m_ctx, "audio-out-params/samplerate", MPV_FORMAT_INT64, &samplerate);
            char *format = mpv_get_property_string(m_ctx, "audio-out-params/format");
            if(format)
            {
                AudioParameters ap(samplerate, ChannelMap(channels), findFormat(format));
                StateHandler::instance()->dispatch(ap);
                mpv_free(format);
            }
            break;
        }

        default:
            ;
        }
    }
}

Qmmp::AudioFormat MpvEngine::findFormat(const char *name) const
{
    //for displaying only
    if(!strcasecmp(name, "u8") || !strcasecmp(name, "u8p"))
        return Qmmp::PCM_U8;
    if(!strcasecmp(name, "s16") || !strcasecmp(name, "s16p"))
        return Qmmp::PCM_S16LE;
    if(!strcasecmp(name, "s32") || !strcasecmp(name, "s32p"))
        return Qmmp::PCM_S32LE;
    if(!strcasecmp(name, "float") || !strcasecmp(name, "floatp"))
        return Qmmp::PCM_FLOAT;

    return Qmmp::PCM_UNKNOWN;
}

void MpvEngine::wakeup(void *data)
{
    MpvEngine *engine = static_cast<MpvEngine *>(data);
    QMetaObject::invokeMethod(engine, [engine]{ engine->processEvents(); }, Qt::QueuedConnection );
}
