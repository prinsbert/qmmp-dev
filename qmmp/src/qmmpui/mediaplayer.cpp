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

#include <QApplication>
#include <QString>
#include <QLocale>
#include <QTimer>
#include <QTranslator>
#include "qmmpuisettings.h"
#include "mediaplayer.h"

class MediaPlayerPrivate
{
    Q_DECLARE_PUBLIC(MediaPlayer)
public:
    MediaPlayerPrivate(MediaPlayer *q) : q_ptr(q)
    {
        if(m_instance)
            qCFatal(core) << "only one instance is allowed";
        m_instance = q;

        m_core = new SoundCore(q);
        m_settings = new QmmpUiSettings(q);
        m_pl_manager = new PlayListManager(q);
        m_finishTimer = new QTimer(q);
        m_finishTimer->setSingleShot(q);
        m_finishTimer->setInterval(500);
    }

    ~MediaPlayerPrivate()
    {
        m_instance = nullptr;
    }

private:
    MediaPlayer *q_ptr;

    void playNext()
    {
        Q_Q(MediaPlayer);
        if(m_settings->isRepeatableTrack())
        {
            q->play();
            return;
        }
        if(m_settings->isNoPlayListAdvance())
        {
            q->stop();
            return;
        }
        if (!m_pl_manager->currentPlayList()->next())
        {
            if(!m_settings->isPlayListTransitionEnabled())
            {
                q->stop();
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
                q->play();
            }
            else
            {
                q->stop();
            }
            return;
        }
        q->play();
    }

    void updateNextUrl()
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

    void processState(Qmmp::State state)
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

    void updateMetaData()
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

    void onCurrentTrackRemoved()
    {
        if(m_settings->stopAfterRemovingOfCurrentTrack())
            m_core->stop();
    }

    void saveMetaData(const PlayListTrack *track)
    {
        if(!track)
            return;

        m_savedInfo = *track;
    }

    void restoreMetaData(PlayListTrack *track)
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

    void updatePlayListMetaData(PlayListTrack *track)
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

    QmmpUiSettings *m_settings;
    PlayListManager *m_pl_manager;
    SoundCore *m_core;
    static MediaPlayer *m_instance;
    int m_skips = 0;
    QString m_nextUrl;
    TrackInfo m_savedInfo;
    QTimer *m_finishTimer;

};

MediaPlayer *MediaPlayerPrivate::m_instance = nullptr;

MediaPlayer::MediaPlayer(QObject *parent):
    QObject(parent),
    d_ptr(new MediaPlayerPrivate(this))
{
    Q_D(MediaPlayer);

    QTranslator *translator = new QTranslator(qApp);
    if(translator->load(QStringLiteral(":/libqmmpui_") + Qmmp::systemLanguageID()))
        qApp->installTranslator(translator);
    else
        delete translator;

    connect(d->m_finishTimer, &QTimer::timeout, this, &MediaPlayer::playbackFinished);
    connect(d->m_core, &SoundCore::nextTrackRequest, this, [d] { d->updateNextUrl(); });
    connect(d->m_core, &SoundCore::finished, this, [d] { d->playNext(); });
    connect(d->m_core, &SoundCore::stateChanged, this, [d](Qmmp::State newState) { d->processState(newState); });
    connect(d->m_core, &SoundCore::trackInfoChanged, this, [d] { d->updateMetaData(); });
    connect(d->m_pl_manager, &PlayListManager::currentTrackRemoved, this, [d] { d->onCurrentTrackRemoved(); });
}

MediaPlayer::~MediaPlayer()
{
    delete d_ptr;
}

MediaPlayer* MediaPlayer::instance()
{
    return MediaPlayerPrivate::m_instance;
}

PlayListManager *MediaPlayer::playListManager()
{
    return d_ptr->m_pl_manager;
}

void MediaPlayer::play()
{
    playFromPosition(-1);
}

void MediaPlayer::playFromPosition(qint64 offset)
{
    Q_D(MediaPlayer);
    d->m_pl_manager->currentPlayList()->doCurrentVisibleRequest();
    if(d->m_core->state() == Qmmp::Paused)
    {
        d->m_core->pause();
        return;
    }

    if(d->m_pl_manager->currentPlayList()->isEmpty())
        return;

    QString s = d->m_pl_manager->currentPlayList()->currentTrack()->path();
    if(s.isEmpty())
    {
        d->m_nextUrl.clear();
        return;
    }
    if(d->m_nextUrl == s)
    {
        d->m_nextUrl.clear();
        return;
    }
    d->m_core->play(s, false, offset);
}

void MediaPlayer::stop()
{
    Q_D(MediaPlayer);
    d->m_core->stop();
    d->m_nextUrl.clear();
    d->m_skips = 0;
}

void MediaPlayer::pause()
{
    d_ptr->m_core->pause();
}

void MediaPlayer::next()
{
    Q_D(MediaPlayer);
    bool playNext = d->m_core->state() != Qmmp::Stopped;
    stop();
    if(d->m_pl_manager->currentPlayList()->next() && playNext)
        play();
}

void MediaPlayer::previous()
{
    Q_D(MediaPlayer);
    bool playNext = d->m_core->state() != Qmmp::Stopped;
    stop();
    if(d->m_pl_manager->currentPlayList()->previous() && playNext)
        play();
}
