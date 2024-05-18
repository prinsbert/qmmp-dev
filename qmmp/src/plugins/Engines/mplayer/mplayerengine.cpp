/***************************************************************************
 *   Copyright (C) 2008-2024 by Ilya Kotov                                 *
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
#include <QDebug>
#include <QDir>
#include <qmmp/trackinfo.h>
#include <qmmp/inputsource.h>
#include <qmmp/volumehandler.h>
#include "mplayerengine.h"

//#define MPLAYER_DEBUG

TrackInfo *MplayerInfo::createTrackInfo(const QString &path)
{
    static const QRegularExpression rx_id_length(u"^ID_LENGTH=([0-9,.]+)*"_s);
    static const QRegularExpression rx_id_audio_bitrate(u"^ID_AUDIO_BITRATE=([0-9,.]+)*"_s);
    static const QRegularExpression rx_id_audio_rate(u"^ID_AUDIO_RATE=([0-9,.]+)*"_s);
    static const QRegularExpression rx_id_audio_nch(u"^ID_AUDIO_NCH=([0-9,.]+)*"_s);
    static const QRegularExpression rx_id_audio_codec(u"^ID_AUDIO_CODEC=(.*)"_s);
    const QStringList args = { u"-slave"_s, u"-identify"_s, u"-frames"_s, u"0"_s, u"-vo"_s, u"null"_s, u"-ao"_s, u"null"_s, path };
    QProcess mplayer_process;
    mplayer_process.start(u"mplayer"_s, args);
    mplayer_process.waitForFinished(1500);
    mplayer_process.kill();
    QString str = QString::fromLocal8Bit(mplayer_process.readAll()).trimmed();
    TrackInfo *info = new TrackInfo(path);
    const QStringList lines = str.split(QChar::LineFeed);
    for(const QString &line : qAsConst(lines))
    {
        QRegularExpressionMatch match;

        if((match = rx_id_length.match(line)).hasMatch())
            info->setDuration(match.captured(1).toDouble() * 1000);
        else if((match = rx_id_audio_bitrate.match(line)).hasMatch())
            info->setValue(Qmmp::BITRATE, match.captured(1).toDouble());
        else if((match = rx_id_audio_rate.match(line)).hasMatch())
            info->setValue(Qmmp::SAMPLERATE, match.captured(1).toDouble());
        else if((match = rx_id_audio_nch.match(line)).hasMatch())
            info->setValue(Qmmp::CHANNELS, match.captured(1).toInt());
        else if((match = rx_id_audio_codec.match(line)).hasMatch())
            info->setValue(Qmmp::FORMAT_NAME, match.captured(1));
    }
    info->setValue(Qmmp::BITS_PER_SAMPLE, 32);
    info->setValue(Qmmp::DECODER, u"mplayer"_s);
    info->setValue(Qmmp::FILE_SIZE, QFileInfo(path).size());
#ifdef MPLAYER_DEBUG
    qCDebug(plugin) << str;
#endif
    return info;
}

const QStringList &MplayerInfo::filters()
{
    static const QStringList filters = { u"*.avi"_s, u"*.mpg"_s, u"*.mpeg"_s, u"*.divx"_s, u"*.qt"_s,
                                         u"*.mov"_s, u"*.wmv"_s, u"*.asf"_s, u"*.flv"_s, u"*.3gp"_s,
                                         u"*.mkv"_s, u"*.mp4"_s, u"*.webm"_s };
    return filters;
}

bool MplayerInfo::supports(const QString &path)
{
    return QDir::match(filters(), path.section(QLatin1Char('/'), -1));
}

MplayerEngine::MplayerEngine(QObject *parent)
        : AbstractEngine(parent)
{
    connect(VolumeHandler::instance(), &VolumeHandler::mutedChanged, this, &MplayerEngine::setMuted);
}

MplayerEngine::~MplayerEngine()
{
    qCDebug(plugin) << Q_FUNC_INFO;
    if(m_process)
        m_process->kill();
    while(!m_sources.isEmpty())
        m_sources.dequeue()->deleteLater();
}

bool MplayerEngine::play()
{
    m_user_stop = false;
    if(m_process && m_process->state() != QProcess::NotRunning)
        return false;
    startMplayerProcess();
    return true;
}

bool MplayerEngine::enqueue(InputSource *source)
{
    if(!MplayerInfo::supports(source->path()))
        return false;

    if(!m_process || m_process->state() == QProcess::NotRunning)
        m_source = source;
    else
        m_sources.enqueue(source);
    return true;
}

bool MplayerEngine::initialize()
{
    TrackInfo *info = MplayerInfo::createTrackInfo(m_source->path());
    m_length = info->duration();
    delete info;
    m_args.clear();
    m_args << u"-slave"_s;
    QSettings settings;
    QString ao_str = settings.value(u"mplayer/ao"_s, u"default"_s).toString();
    QString vo_str = settings.value(u"mplayer/vo"_s, u"default"_s).toString();
    if (ao_str != "default"_L1)
        m_args << u"-ao"_s << ao_str;
    if (vo_str != "default"_L1)
        m_args << u"-vo"_s << vo_str;

    if (settings.value(u"autosync"_s, false).toBool())
        m_args << u"-autosync"_s << QString::number(settings.value(u"autosync_factor"_s, 100).toInt());

    m_args << settings.value(u"cmd_options"_s).toString().split(QChar::Space, Qt::SkipEmptyParts);

    if(m_source->offset() > 0)
        m_args << u"-ss"_s << QString::number(m_source->offset() / 1000);
    m_args << m_source->path();
    return true;
}

void MplayerEngine::seek(qint64 pos)
{
    if (m_process && m_process->state() == QProcess::Running)
        m_process->write(QStringLiteral("seek %1\n").arg(pos/1000 - m_currentTime).toLocal8Bit());
}

void MplayerEngine::stop()
{
    while(!m_sources.isEmpty())
        m_sources.dequeue()->deleteLater();
    if(m_process && m_process->state() == QProcess::Running)
    {
        m_user_stop = true;
        m_process->write("quit\n");
        m_process->waitForFinished(3500);
        m_process->kill();
        StateHandler::instance()->dispatch(Qmmp::Stopped);
    }
}

void MplayerEngine::pause()
{
    if(m_process)
        m_process->write("pause\n");
}

void MplayerEngine::setMuted(bool muted)
{
    if(m_process && m_process->state() == QProcess::Running)
    {
        m_process->write(muted ? "mute 1\n" : "mute 0\n");
    }
}

void MplayerEngine::readStdOut()
{
    static const QRegularExpression rx_av(u"^[AV]: *([0-9,:.-]+)"_s);
    static const QRegularExpression rx_pause(u"^(.*)=(.*)PAUSE(.*)"_s);
    static const QRegularExpression rx_end(u"^(.*)End of file(.*)"_s);
    static const QRegularExpression rx_quit(u"^(.*)Quit(.*)"_s);
    static const QRegularExpression rx_audio(u"^AUDIO: *([0-9,.]+) *Hz.*([0-9,.]+) *ch.*([0-9]+).* ([0-9,.]+) *kbit.*"_s);
    static const QRegularExpression rx_audio2(u"^AUDIO: *([0-9,.]+) *Hz.*([0-9,.]+) *ch.*([a-z]+).* ([0-9,.]+) *kbit.*"_s);

    const QStringList lines = QString::fromLocal8Bit(m_process->readAll()).trimmed().split(QChar::LineFeed);
    for(const QString &line : qAsConst(lines))
    {
        QRegularExpressionMatch match;

        if ((match = rx_av.match(line)).hasMatch())
        {
            StateHandler::instance()->dispatch(Qmmp::Playing);
            m_currentTime = (qint64) match.captured(1).toDouble();
            StateHandler::instance()->dispatch(m_currentTime * 1000, m_bitrate);
        }
        else if ((match = rx_pause.match(line)).hasMatch())
        {
            StateHandler::instance()->dispatch(Qmmp::Paused);
        }
        else if ((match = rx_end.match(line)).hasMatch())
        {
            if (m_process->state() == QProcess::Running)
                m_process->waitForFinished(3500);
            StateHandler::instance()->sendFinished();
            if(!m_sources.isEmpty())
            {
                StateHandler::instance()->dispatch(Qmmp::Stopped);
                m_source = m_sources.dequeue();
                startMplayerProcess();
            }
            else
            {
                StateHandler::instance()->dispatch(Qmmp::Stopped);
                return;
            }
        }
        else if ((match = rx_quit.match(line)).hasMatch() && !m_user_stop)
        {
            if (m_process->state() == QProcess::Running)
            {
                m_process->waitForFinished(1500);
                m_process->kill();
            }
            StateHandler::instance()->dispatch(Qmmp::Stopped);
        }
        else if ((match = rx_audio.match(line)).hasMatch())
        {
            m_samplerate = match.captured(1).toInt();
            m_channels = match.captured(2).toInt();
            m_bitsPerSample = match.captured(3).toDouble();
            m_bitrate = match.captured(4).toDouble();
            AudioParameters ap(m_samplerate, ChannelMap(m_channels), AudioParameters::findAudioFormat(m_bitsPerSample));
            StateHandler::instance()->dispatch(ap);
        }
        else if ((match = rx_audio2.match(line)).hasMatch())
        {
            m_samplerate = match.captured(1).toInt();
            m_channels = match.captured(2).toInt();
            m_bitsPerSample = 32;
            m_bitrate = match.captured(4).toDouble();
            AudioParameters ap(m_samplerate, ChannelMap(m_channels), AudioParameters::findAudioFormat(m_bitsPerSample));
            StateHandler::instance()->dispatch(ap);
        }
#ifdef MPLAYER_DEBUG
        else
            qCDebug(plugin) << line;
#endif
    }
}

void MplayerEngine::onError(QProcess::ProcessError error)
{
    if(error == QProcess::FailedToStart || error == QProcess::Crashed)
        StateHandler::instance()->dispatch(Qmmp::FatalError);
    qCWarning(plugin, "process error: %d", error);
}

void MplayerEngine::onStateChanged(QProcess::ProcessState state)
{
    if(state == QProcess::Running)
        setMuted(VolumeHandler::instance()->isMuted());
}

void MplayerEngine::startMplayerProcess()
{
    initialize();
    delete m_process;
    m_process = new QProcess(this);
    connect(m_process, &QProcess::readyReadStandardOutput, this, &MplayerEngine::readStdOut);
    connect(m_process, &QProcess::errorOccurred, this, &MplayerEngine::onError);
    connect(m_process, &QProcess::stateChanged, this, &MplayerEngine::onStateChanged);
    m_process->start(u"mplayer"_s, m_args);
    StateHandler::instance()->dispatch(Qmmp::Playing);
    StateHandler::instance()->dispatch(m_length);
    TrackInfo *info = MplayerInfo::createTrackInfo(m_source->path());
    StateHandler::instance()->dispatch(*info);
    delete info;
    m_source->deleteLater();
    m_source = nullptr;
    m_currentTime = 0;
}
