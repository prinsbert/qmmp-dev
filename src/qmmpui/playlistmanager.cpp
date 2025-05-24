/***************************************************************************
 *   Copyright (C) 2009-2025 by Ilya Kotov                                 *
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

#include <QByteArray>
#include <QFile>
#include <QBuffer>
#include <QDir>
#include <QTimer>
#include <QSettings>
#include <QSaveFile>
#include <qmmp/trackinfo.h>
#include "qmmpuisettings.h"
#include "playlistmanager.h"

using namespace Qt::Literals::StringLiterals;

PlayListManager *PlayListManager::m_instance = nullptr;

//key names
const QMap<QString, Qmmp::MetaData> PlayListManager::m_metaKeys = {
    { u"title"_s, Qmmp::TITLE },
    { u"artist"_s, Qmmp::ARTIST },
    { u"albumartist"_s, Qmmp::ALBUMARTIST },
    { u"album"_s, Qmmp::ALBUM },
    { u"comment"_s, Qmmp::COMMENT },
    { u"genre"_s, Qmmp::GENRE },
    { u"composer"_s, Qmmp::COMPOSER },
    { u"year"_s, Qmmp::YEAR },
    { u"track"_s, Qmmp::TRACK },
    { u"disk"_s, Qmmp::DISCNUMBER }
};

const QMap<QString, Qmmp::TrackProperty> PlayListManager::m_propKeys = {
    { u"bitrate"_s, Qmmp::BITRATE },
    { u"samplerate"_s, Qmmp::SAMPLERATE },
    { u"channels"_s, Qmmp::CHANNELS },
    { u"bits_per_sample"_s, Qmmp::BITS_PER_SAMPLE },
    { u"format_name"_s, Qmmp::FORMAT_NAME },
    { u"decoder"_s, Qmmp::DECODER },
    { u"file_size"_s, Qmmp::FILE_SIZE }
};

PlayListManager::PlayListManager(QObject *parent) : QObject(parent)
{
    if(m_instance)
        qCFatal(core) << "only one instance is allowed";
    qRegisterMetaType<PlayListModel::SortMode>();
    m_instance = this;
    m_ui_settings = QmmpUiSettings::instance();
    m_header = new PlayListHeaderModel(this);
    m_timer = new QTimer(this);
    m_timer->setInterval(5000);
    m_timer->setSingleShot(true);
    connect(m_timer, &QTimer::timeout, this, &PlayListManager::writePlayLists);
    readPlayLists(); //read playlists
}

PlayListManager::~PlayListManager()
{
    writePlayLists();
    m_instance = nullptr;
}

PlayListManager* PlayListManager::instance()
{
    return m_instance;
}

PlayListModel *PlayListManager::selectedPlayList() const
{
    return m_selected;
}

PlayListModel *PlayListManager::currentPlayList() const
{
    return m_current;
}

int PlayListManager::selectedPlayListIndex() const
{
    return indexOf(m_selected);
}

int PlayListManager::currentPlayListIndex() const
{
    return indexOf(m_current);
}

QList<PlayListModel *> PlayListManager::playLists() const
{
    return m_models;
}

QStringList PlayListManager::playListNames() const
{
    QStringList names;
    for(const PlayListModel *model : std::as_const(m_models))
        names << model->name();
    return names;
}

void PlayListManager::selectPlayList(PlayListModel *model)
{
    if(model != m_selected && m_models.contains(model))
    {
        PlayListModel *prev = m_selected;
        m_selected = model;
        emit selectedPlayListChanged(model, prev);
        emit playListsChanged();
    }
}

void PlayListManager::selectPlayListIndex(int i)
{
    if(i < 0 || i > m_models.count() - 1)
        return;
    selectPlayList(playListAt(i));
}

void PlayListManager::selectPlayListName(const QString &name)
{
    int index = playListNames().indexOf(name);
    if(index >= 0)
        selectPlayList(playListAt(index));
}

void PlayListManager::selectNextPlayList()
{
    selectPlayListIndex(m_models.indexOf(m_selected) + 1);
}

void PlayListManager::selectPreviousPlayList()
{
    selectPlayListIndex(m_models.indexOf(m_selected) - 1);
}

void PlayListManager::activatePlayList(PlayListModel *model)
{
    if(model != m_current && m_models.contains(model))
    {
        PlayListModel *prev = m_current;
        m_current = model;
        emit currentPlayListChanged(model, prev);
        emit playListsChanged();
    }
}

void PlayListManager::activatePlayListIndex(int index)
{
    activatePlayList(playListAt(index));
}

void PlayListManager::activateSelectedPlayList()
{
    activatePlayList(selectedPlayList());
}

PlayListModel *PlayListManager::createPlayList(const QString &name)
{
    PlayListModel *model = new PlayListModel(name.isEmpty() ? tr("Playlist") : name, this);
    QStringList names = playListNames();
    QString uniqueName = model->name();
    int i = 0;

    while(names.contains(uniqueName))
        uniqueName = model->name() + QStringLiteral(" (%1)").arg(++i);

    model->setName(uniqueName);

    m_models.append(model);
    connect(model, &PlayListModel::nameChanged, this, &PlayListManager::playListsChanged);
    connect(model, &PlayListModel::listChanged, this, &PlayListManager::onListChanged);
    connect(model, &PlayListModel::currentTrackRemoved, this, &PlayListManager::onCurrentTrackRemoved);
    emit playListAdded(m_models.indexOf(model));
    selectPlayList(model);
    return model;
}

void PlayListManager::removePlayList(PlayListModel *model)
{
     if(m_models.count() < 2 || !m_models.contains(model))
        return;

     int i = m_models.indexOf(model);

     if(m_current == model)
     {
         m_current = m_models.at((i > 0) ? (i - 1) : (i + 1));
         emit currentPlayListChanged(m_current, model);
         emit currentTrackRemoved();
     }
     if(m_selected == model)
     {
         m_selected = m_models.at((i > 0) ? (i - 1) : (i + 1));
         emit selectedPlayListChanged(m_selected, model);
     }
     m_models.removeAt(i);
     model->deleteLater();
     emit playListRemoved(i);
     emit playListsChanged();
}

void PlayListManager::removePlayListIndex(int index)
{
    removePlayList(playListAt(index));
}

void PlayListManager::move(int i, int j)
{
    if(i < 0 || j < 0 || i == j)
        return;
    if(i < m_models.count() && j < m_models.count())
    {
        m_models.move(i,j);
        emit playListMoved(i,j);
        emit playListsChanged();
    }
}

int PlayListManager::count() const
{
    return m_models.count();
}

int PlayListManager::indexOf(PlayListModel *model) const
{
    return m_models.indexOf(model);
}

PlayListModel *PlayListManager::playListAt(int i) const
{
    if(i >= 0 && i < m_models.count())
        return m_models.at(i);
    return nullptr;
}

PlayListHeaderModel *PlayListManager::headerModel()
{
    return m_header;
}

void PlayListManager::readPlayLists()
{
    Qmmp::MetaData metaKey;
    Qmmp::TrackProperty propKey;
    QString line, key, value;
    int current = 0, pl = 0;
    QList<PlayListTrack *> tracks;
    QFile file(Qmmp::configDir() + u"/playlist.txt"_s);
    if(file.open(QIODevice::ReadOnly) && file.size() > 0)
    {
        QByteArray array = file.readAll();
        file.close();
        QBuffer buffer(&array);
        buffer.open(QIODevice::ReadOnly);

        while(!buffer.atEnd())
        {
            line = QString::fromUtf8(buffer.readLine().constData()).trimmed();
            int s = line.indexOf(QLatin1Char('='));
            if (s < 0)
                continue;

            key = line.left(s);
            value = line.right(line.size() - s - 1);

            if(key == "current_playlist"_L1)
                pl = value.toInt();
            else if(key == "playlist"_L1)
            {
                if(!m_models.isEmpty() && !tracks.isEmpty())
                {
                    m_models.last()->addTracks(tracks);
                    m_models.last()->setCurrent(tracks.at(qBound(0, current, tracks.count() - 1)));
                }
                tracks.clear();
                current = 0;
                m_models << new PlayListModel(value, this);
            }
            else if (key == "current"_L1)
            {
                current = value.toInt();
            }
            else if (key == "file"_L1)
            {
                tracks << new PlayListTrack();
                tracks.last()->setPath(value);
            }
            else if (tracks.isEmpty())
                continue;
            else if (key == "duration"_L1)
                tracks.last()->setDuration(value.toInt());
            else if (key == "length"_L1)
                tracks.last()->setDuration(value.toInt() * 1000);
            else if((metaKey = m_metaKeys.value(key, Qmmp::UNKNOWN)) != Qmmp::UNKNOWN)
            {
                if(metaKey == Qmmp::COMMENT)
                {
                    value.replace(QStringLiteral("\\n"), QStringLiteral("\n"));
                    value.replace(QStringLiteral("\\r"), QStringLiteral("\r"));
                }
                tracks.last()->setValue(metaKey, value);
            }
            else if((propKey = m_propKeys.value(key, Qmmp::UNKNOWN_PROPERTY)) != Qmmp::UNKNOWN_PROPERTY)
                tracks.last()->setValue(propKey, value);
        }
        buffer.close();
    }

    if(m_models.isEmpty())
    {
        m_models << new PlayListModel(tr("Playlist"), this);
    }
    else if(!tracks.isEmpty())
    {
        m_models.last()->addTracks(tracks);
        m_models.last()->setCurrent(tracks.at(qBound(0, current, tracks.count() - 1)));
    }
    if(pl < 0 || pl >= m_models.count())
        pl = 0;
    m_selected = m_models.at(pl);
    m_current = m_models.at(pl);
    for(const PlayListModel *model : std::as_const(m_models))
    {
        connect(model, &PlayListModel::nameChanged, this, &PlayListManager::playListsChanged);
        connect(model, &PlayListModel::listChanged, this, &PlayListManager::onListChanged);
        connect(model, &PlayListModel::currentTrackRemoved, this, &PlayListManager::onCurrentTrackRemoved);
    }
}

void PlayListManager::writePlayLists()
{
    qCDebug(core) << "saving playlists...";
    QString value;
    QString plFilePath = Qmmp::configDir() + u"/playlist.txt"_s;
    QSaveFile plFile(plFilePath);
    if(!plFile.open(QIODevice::WriteOnly))
    {
        qCDebug(core) << "error: %s" << plFile.errorString();
        return;
    }
    plFile.write(QStringLiteral("current_playlist=%1\n").arg(m_models.indexOf(m_current)).toUtf8());
    for(const PlayListModel *model : std::as_const(m_models))
    {
        plFile.write(QStringLiteral("playlist=%1\n").arg(model->name()).toUtf8());
        if(model->isEmpty())
            continue;
        const QList<PlayListTrack *> tracks = model->tracks();
        plFile.write(QStringLiteral("current=%1\n").arg(model->currentIndex()).toLatin1());
        for(PlayListTrack *t : std::as_const(tracks))
        {
            plFile.write(QStringLiteral("file=%1\n").arg(t->path()).toUtf8());

            for(QMap<QString, Qmmp::MetaData>::const_iterator it = m_metaKeys.constBegin(); it != m_metaKeys.constEnd(); ++it)
            {
                if(!(value = t->value(it.value())).isEmpty())
                {
                    if(it.value() == Qmmp::COMMENT)
                    {
                        value.replace(QChar::LineFeed, QStringLiteral("\\n"));
                        value.replace(QChar::CarriageReturn, QStringLiteral("\\r"));
                    }

                    plFile.write(QStringLiteral("%1=%2\n").arg(it.key(), value).toUtf8());
                }
            }

            for(QMap<QString, Qmmp::TrackProperty>::const_iterator it = m_propKeys.constBegin(); it != m_propKeys.constEnd(); ++it)
            {
                if(!(value = t->value(it.value())).isEmpty())
                    plFile.write(QStringLiteral("%1=%2\n").arg(it.key(), value).toLatin1());
            }

            if(t->duration() > 0)
                plFile.write(QStringLiteral("duration=%1\n").arg(t->duration()).toLatin1());
        }
    }
    plFile.commit();
}

void PlayListManager::onListChanged(int flags)
{
    if((flags & PlayListModel::STRUCTURE) && m_ui_settings->autoSavePlayList())
        m_timer->start();
}

void PlayListManager::onCurrentTrackRemoved()
{
    if(sender() == m_current)
        emit currentTrackRemoved();
}

void PlayListManager::clear()
{
    m_selected->clear();
}

void PlayListManager::clearSelection()
{
    m_selected->clearSelection();
}

void PlayListManager::removeSelected()
{
    m_selected->removeSelected();
}

void PlayListManager::removeUnselected()
{
    m_selected->removeUnselected();
}

void PlayListManager::removeTrack(int i)
{
    m_selected->removeTrack(i);
}

void PlayListManager::removeTrack(PlayListTrack *track)
{
    m_selected->removeTrack(track);
}

void PlayListManager::invertSelection()
{
    m_selected->invertSelection();
}

void PlayListManager::selectAll()
{
    m_selected->selectAll();
}

void PlayListManager::showDetails()
{
    m_selected->showDetails();
}

void PlayListManager::addTracks(const QList<PlayListTrack *> &tracks)
{
    m_selected->addTracks(tracks);
}

void PlayListManager::addPath(const QString &path)
{
    m_selected->addPath(path);
}

void PlayListManager::addPaths(const QStringList &paths)
{
    m_selected->addPaths(paths);
}

void PlayListManager::randomizeList()
{
    m_selected->randomizeList();
}

void PlayListManager::reverseList()
{
    m_selected->reverseList();
}

void PlayListManager::sortSelection(PlayListModel::SortMode mode)
{
    m_selected->sortSelection(mode);
}

void PlayListManager::sort(PlayListModel::SortMode mode)
{
    m_selected->sort(mode);
}

void PlayListManager::addToQueue()
{
    m_selected->addToQueue();
}

void PlayListManager::removeInvalidTracks()
{
    m_selected->removeInvalidTracks();
}

void PlayListManager::removeDuplicates()
{
    m_selected->removeDuplicates();
}

void PlayListManager::refresh()
{
    m_selected->refresh();
}

void PlayListManager::clearQueue()
{
    m_selected->clearQueue();
}

void PlayListManager::stopAfterSelected()
{
    m_selected->stopAfterSelected();
}

void PlayListManager::rebuildGroups()
{
    for(PlayListModel *model : std::as_const(m_models))
        model->rebuildGroups();
}
