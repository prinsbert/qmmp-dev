/***************************************************************************
 *   Copyright (C) 2008-2025 by Ilya Kotov                                 *
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

#include <QApplication>
#include <QString>
#include <QLocale>
#include <QTimer>
#include <QTranslator>
#include "playlistitem.h"
#include "qmmpuisettings.h"
#include "mediaplayer.h"

MediaPlayer *MediaPlayer::m_instance = nullptr;

MediaPlayer::MediaPlayer(QObject *parent)
        : QObject(parent)
{
    if(m_instance)
        qCFatal(core) << "only one instance is allowed";
    m_instance = this;

    QTranslator *translator = new QTranslator(qApp);
    if(translator->load(QStringLiteral(":/libqmmpui_") + Qmmp::systemLanguageID()))
        qApp->installTranslator(translator);
    else
        delete translator;

    m_core = new SoundCore(this);
    m_settings = new QmmpUiSettings(this);
    m_pl_manager = new PlayListManager(this);
    m_finishTimer = new QTimer(this);
    m_finishTimer->setSingleShot(true);
    m_finishTimer->setInterval(500);
    connect(m_finishTimer, &QTimer::timeout, this, &MediaPlayer::playbackFinished);
    connect(m_core, &SoundCore::nextTrackRequest, this, &MediaPlayer::updateNextUrl);
    connect(m_core, &SoundCore::finished, this, &MediaPlayer::playNext);
    connect(m_core, &SoundCore::stateChanged, this, &MediaPlayer::processState);
    connect(m_core, &SoundCore::trackInfoChanged, this, &MediaPlayer::updateMetaData);
    connect(m_pl_manager, &PlayListManager::currentTrackRemoved, this, &MediaPlayer::onCurrentTrackRemoved);
}

MediaPlayer::~MediaPlayer()
{
    m_instance = nullptr;
}

MediaPlayer* MediaPlayer::instance()
{
    return m_instance;
}

PlayListManager *MediaPlayer::playListManager()
{
    return m_pl_manager;
}

void MediaPlayer::play()
{
    playFromPosition(-1);
}

void MediaPlayer::playFromPosition(qint64 offset)
{
    m_pl_manager->currentPlayList()->doCurrentVisibleRequest();
    if (m_core->state() == Qmmp::Paused)
    {
        m_core->pause();
        return;
    }

    if (m_pl_manager->currentPlayList()->isEmpty())
        return;

    QString s = m_pl_manager->currentPlayList()->currentTrack()->path();
    if (s.isEmpty())
    {
        m_nextUrl.clear();
        return;
    }
    if(m_nextUrl == s)
    {
        m_nextUrl.clear();
        return;
    }
    m_core->play(s, false, offset);
}

void MediaPlayer::stop()
{
    m_core->stop();
    m_nextUrl.clear();
    m_skips = 0;
}

void MediaPlayer::pause()
{
    m_core->pause();
}

void MediaPlayer::next()
{
    bool playNext = m_core->state() != Qmmp::Stopped;
    stop();
    if (m_pl_manager->currentPlayList()->next() && playNext)
        play();
}

void MediaPlayer::previous()
{
    bool playNext = m_core->state() != Qmmp::Stopped;
    stop();
    if (m_pl_manager->currentPlayList()->previous() && playNext)
        play();
}

void MediaPlayer::playNext()
{
    if(m_settings->isRepeatableTrack())
    {
        play();
        return;
    }
    if(m_settings->isNoPlayListAdvance())
    {
        stop();
        return;
    }
    if (!m_pl_manager->currentPlayList()->next())
    {
        if(!m_settings->isPlayListTransitionEnabled())
        {
            stop();
            return;
        }
        //next playlist
        int index = m_pl_manager->currentPlayListIndex() + 1;
        PlayListModel *nextPlayList = index < m_pl_manager->count() ? m_pl_manager->playListAt(index) : nullptr;
        PlayListTrack *nextTrack = nextPlayList ? nextPlayList->currentTrack() : nullptr;
        if(nextTrack)
        {
            m_pl_manager->selectPlayList(nextPlayList);
            m_pl_manager->activatePlayList(nextPlayList);
            play();
        }
        else
        {
            stop();
        }
        return;
    }
    play();
}

void MediaPlayer::updateNextUrl()
{
    m_nextUrl.clear();
    PlayListTrack *track = nullptr;
    if(m_settings->isRepeatableTrack())
        track = m_pl_manager->currentPlayList()->currentTrack();
    else if(!m_settings->isNoPlayListAdvance())
        track = m_pl_manager->currentPlayList()->nextTrack();

    if(!track && m_settings->isPlayListTransitionEnabled())
    {
        int index = m_pl_manager->currentPlayListIndex() + 1;
        PlayListModel *nextPlayList = index < m_pl_manager->count() ? m_pl_manager->playListAt(index) : nullptr;
        track = nextPlayList ? nextPlayList->currentTrack() : nullptr;
    }

    if(track)
    {
        bool ok = m_core->play(track->path(), true);
        if(ok)
        {
            m_nextUrl = track->path();
            qCDebug(core) << "next track state: received";
        }
        else
            qCDebug(core) << "next track state: error";
    }
    else
        qCDebug(core) << "next track state: unknown";

}

void MediaPlayer::processState(Qmmp::State state)
{
    switch ((int) state)
    {
    case Qmmp::NormalError:
        m_core->stop();
        m_nextUrl.clear();
        if (m_skips <= m_pl_manager->currentPlayList()->trackCount())
        {
            m_skips++;
            playNext();
        }
        break;
    case Qmmp::FatalError:
        m_core->stop();
        m_nextUrl.clear();
        break;
    case Qmmp::Playing:
        m_finishTimer->stop();
        m_skips = 0;
        break;
    case Qmmp::Stopped:
        m_finishTimer->start();
        restoreMetaData(m_pl_manager->currentPlayList()->currentTrack());
        break;
    default:
        ;
    }
}

void MediaPlayer::updateMetaData()
{
    TrackInfo info = m_core->trackInfo();
    qCDebug(core) << "===== metadata ======";
    qCDebug(core) << "ARTIST =" << info.value(Qmmp::ARTIST);
    qCDebug(core) << "TITLE =" << info.value(Qmmp::TITLE);
    qCDebug(core) << "ALBUMARTIST =" << info.value(Qmmp::ALBUMARTIST);
    qCDebug(core) << "ALBUM =" << info.value(Qmmp::ALBUM);
    qCDebug(core) << "COMMENT =" << info.value(Qmmp::COMMENT);
    qCDebug(core) << "GENRE =" << info.value(Qmmp::GENRE);
    qCDebug(core) << "YEAR =" << info.value(Qmmp::YEAR);
    qCDebug(core) << "TRACK =" << info.value(Qmmp::TRACK);
    qCDebug(core) << "DISCNUMBER =" << info.value(Qmmp::DISCNUMBER);
    qCDebug(core) << "---------------------";
    qCDebug(core) << "BITRATE =" << info.value(Qmmp::BITRATE);
    qCDebug(core) << "SAMPLERATE =" << info.value(Qmmp::SAMPLERATE);
    qCDebug(core) << "CHANNELS =" << info.value(Qmmp::CHANNELS);
    qCDebug(core) << "BITS_PER_SAMPLE =" << info.value(Qmmp::BITS_PER_SAMPLE);
    qCDebug(core) << "FORMAT_NAME =" << info.value(Qmmp::FORMAT_NAME);
    qCDebug(core) << "DECODER =" << info.value(Qmmp::DECODER);
    qCDebug(core) << "FILE_SIZE =" << info.value(Qmmp::FILE_SIZE);
    qCDebug(core) << "---------------------";
    qCDebug(core) << "REPLAYGAIN_TRACK_GAIN =" << info.value(Qmmp::REPLAYGAIN_TRACK_GAIN);
    qCDebug(core) << "REPLAYGAIN_TRACK_PEAK =" << info.value(Qmmp::REPLAYGAIN_TRACK_PEAK);
    qCDebug(core) << "REPLAYGAIN_ALBUM_GAIN =" << info.value(Qmmp::REPLAYGAIN_ALBUM_GAIN);
    qCDebug(core) << "REPLAYGAIN_ALBUM_PEAK =" << info.value(Qmmp::REPLAYGAIN_ALBUM_PEAK);
    qCDebug(core) << "---------------------";
    qCDebug(core) << "DURATION =" << info.duration();
    qCDebug(core) << "== end of metadata ==";

    PlayListModel *pl = m_pl_manager->currentPlayList();
    PlayListTrack *currentTrack = pl->currentTrack();
    if(currentTrack && currentTrack->path() == info.path())
    {
        saveMetaData(currentTrack);
        currentTrack->updateMetaData(info);
        updatePlayListMetaData(currentTrack);
    }
}

void MediaPlayer::onCurrentTrackRemoved()
{
    if(m_settings->stopAfterRemovingOfCurrentTrack())
        m_core->stop();
}

void MediaPlayer::saveMetaData(const PlayListTrack *track)
{
    if(!track)
        return;

    m_savedInfo = *track;
}

void MediaPlayer::restoreMetaData(PlayListTrack *track)
{
    if(!track)
        return;

    //restore initial metadata for streams
    if(m_savedInfo.path().contains(u"://"_s) && !m_savedInfo.path().contains(QLatin1Char('#')) &&
            m_savedInfo.path() == track->path() &&
            !m_savedInfo.value(Qmmp::TITLE).isEmpty())
    {
        m_savedInfo.clear(TrackInfo::Properties | TrackInfo::ReplayGainInfo); //restore displaying metadata only
        track->updateMetaData(m_savedInfo);
        updatePlayListMetaData(track);
        m_savedInfo.clear();
    }
}

void MediaPlayer::updatePlayListMetaData(PlayListTrack *track)
{
    if(!track)
        return;

    PlayListModel *pl = m_pl_manager->currentPlayList();
    PlayListGroup *group = pl->group(track);
    //update group titles
    if(group && group->tracks().constFirst() == track)
        group->updateMetaData();

    pl->updateMetaData();
}
