/***************************************************************************
 *   Copyright(C) 2006-2025 by Ilya Kotov                                  *
 *   forkotov02@ya.ru                                                      *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *  (at your option) any later version.                                    *
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
#include <QTextStream>
#include <algorithm>
#include <qmmp/metadatamanager.h>
#include "metadatahelper_p.h"
#include "playlistparser.h"
#include "playlistformat.h"
#include "playlistcontainer_p.h"
#include "groupedcontainer_p.h"
#include "normalcontainer_p.h"
#include "playlisttask_p.h"
#include "fileloader_p.h"
#include "playstate_p.h"
#include "detailsdialog.h"
#include "qmmpuisettings.h"
#include "playlistmodel.h"

PlayListModel::PlayListModel(const QString &name, QObject *parent)
    : QObject(parent), m_name(name)
{
    m_ui_settings = QmmpUiSettings::instance();

    m_loader = new FileLoader(this);
    m_coverLoder = new CoverLoader(this);
    m_task = new PlayListTask(this);

    if(m_ui_settings->isGroupsEnabled())
        m_container = new GroupedContainer;
    else
        m_container = new NormalContainer;

    m_container->setLinesPerGroup(m_ui_settings->linesPerGroup());

    if(m_ui_settings->isShuffle())
        m_play_state = new ShufflePlayState(this);
    else
        m_play_state = new NormalPlayState(this);
    connect(m_ui_settings, &QmmpUiSettings::groupsChanged, this, &PlayListModel::prepareGroups);
    connect(m_ui_settings, &QmmpUiSettings::shuffleChanged, this, &PlayListModel::prepareForShufflePlaying);
    connect(m_loader, &FileLoader::newTracksToInsert, this, &PlayListModel::insertTracksInternal, Qt::QueuedConnection);
    connect(m_loader, &FileLoader::finished, this, &PlayListModel::preparePlayState);
    connect(m_loader, &FileLoader::finished, this, &PlayListModel::loaderFinished);
    connect(m_loader, &FileLoader::finished, this, &PlayListModel::startCoverLoader);
    connect(m_coverLoder, &CoverLoader::ready, this, &PlayListModel::setCover);
    connect(m_task, &PlayListTask::finished, this, &PlayListModel::onTaskFinished);
    connect(m_task, &PlayListTask::finished, this, &PlayListModel::startCoverLoader);
}

PlayListModel::~PlayListModel()
{
    blockSignals(true);
    m_loader->finish();
    m_coverLoder->finish();
    clear();
    delete m_play_state;
    delete m_container;
}

QString PlayListModel::name() const
{
    return m_name;
}

void PlayListModel::setName(const QString &name)
{
    if(m_name != name)
    {
        m_name = name;
        emit nameChanged(name);
    }
}

void PlayListModel::addTrack(PlayListTrack *track)
{
    m_container->addTrack(track);
    m_total_duration += track->duration();

    int flags = 0;

    if(m_container->trackCount() == 1)
    {
        m_current_track = track;
        m_current = m_container->indexOf(track);
        flags |= CURRENT;
    }
    else if(m_ui_settings->isGroupsEnabled())
    {
        //update current index for grouped container only
        m_current = m_container->indexOf(m_current_track);
    }
    if(sender() != m_loader)
    {
        preparePlayState();
        startCoverLoader();
    }
    flags |= STRUCTURE;
    emit tracksAdded({ track });
    emit listChanged(flags);
}

void PlayListModel::addTracks(const QList<PlayListTrack *> &tracks)
{
    if(tracks.isEmpty())
        return;

    int flags = 0;

    m_container->addTracks(tracks);

    if(m_container->trackCount() == tracks.count())
    {
        m_current_track = tracks.constFirst();
        m_current = m_container->indexOf(m_current_track);
        flags |= CURRENT;
    }
    else if(m_ui_settings->isGroupsEnabled())
    {
        //update current index for grouped container only
        m_current = m_container->indexOf(m_current_track);
    }

    for(PlayListTrack *track : std::as_const(tracks))
    {
        m_total_duration += track->duration();
    }
    emit tracksAdded(tracks);

    if(sender() != m_loader)
    {
        preparePlayState();
        startCoverLoader();
    }
    flags |= STRUCTURE;
    emit listChanged(flags);
}

void PlayListModel::addPath(const QString &path)
{
    m_loader->add(path);
}

void PlayListModel::addPaths(const QStringList &paths)
{
    m_loader->add(paths);
}

void PlayListModel::insertTrack(int index, PlayListTrack *track)
{
    m_container->insertTrack(index, track);
    m_total_duration += track->duration();

    int flags = 0;

    if(m_container->trackCount() == 1)
    {
        m_current_track = track;
        m_current = m_container->indexOf(track);
        flags |= CURRENT;
    }
    else
    {
        //update current index
        m_current = m_container->indexOf(m_current_track);
    }
    if(sender() != m_loader)
    {
        preparePlayState();
        startCoverLoader();
    }
    emit tracksAdded({ track });
    flags |= STRUCTURE;
    emit listChanged(flags);
}

void PlayListModel::insertTracks(int index, const QList<PlayListTrack *> &tracks)
{
    if(tracks.isEmpty())
        return;

    int flags = 0;

    for(PlayListTrack *track : std::as_const(tracks))
    {
        index = m_container->insertTrack(index, track) + 1;
        m_total_duration += track->duration();
        if(m_container->trackCount() == 1)
        {
            m_current_track = track;
            m_current = m_container->indexOf(track);
            flags |= CURRENT;
        }
    }
    emit tracksAdded(tracks);

    //update current index
    m_current = m_container->indexOf(m_current_track);
    if (sender() != m_loader)
    {
        preparePlayState();
        startCoverLoader();
    }
    flags |= STRUCTURE;
    emit listChanged(flags);
}

void PlayListModel::insertJson(int index, const QByteArray &json)
{
    insertTracks(index, PlayListParser::deserialize(json));
}

void PlayListModel::insertPath(int index, const QString &path)
{
    insertPaths(index, QStringList() << path);
}

void PlayListModel::insertPaths(int index, const QStringList &paths)
{
    if(index < 0 || index >= m_container->trackCount())
        addPaths(paths);
    else
    {
        PlayListTrack *before = m_container->track(index);
        m_loader->insert(before, paths);
    }
}

void PlayListModel::insertUrls(int index, const QList<QUrl> &urls)
{
    QStringList paths;
    for(const QUrl &url : std::as_const(urls))
    {
        if(url.scheme() == "file"_L1)
            paths.append(QFileInfo(url.toLocalFile()).canonicalFilePath());
        else
            paths.append(url.toString());
    }
    insertPaths(index, paths);
}

int PlayListModel::groupCount() const
{
    return m_container->groupCount();
}

int PlayListModel::trackCount() const
{
    return m_container->trackCount();
}

bool PlayListModel::isEmpty() const
{
    return m_container->isEmpty();
}

int PlayListModel::columnCount() const
{
    return MetaDataHelper::instance()->columnCount();
}

PlayListTrack *PlayListModel::currentTrack() const
{
    return m_container->isEmpty() ? nullptr : m_current_track;
}

PlayListTrack *PlayListModel::nextTrack() const
{
    if(m_container->isEmpty() || !m_play_state)
        return nullptr;
    if(m_stop_track && m_stop_track == currentTrack())
        return nullptr;
    if(!isEmptyQueue())
        return m_container->queuedTracks().constFirst();
    int index = m_play_state->nextIndex();
    if(index < 0 || (index + 1 > m_container->trackCount()))
        return nullptr;
    return m_container->track(index);
}

int PlayListModel::indexOf(PlayListItem *item) const
{
    return m_container->indexOf(item);
}

PlayListTrack* PlayListModel::track(int index) const
{
    return m_container->track(index);
}

PlayListGroup* PlayListModel::group(int index) const
{
    return m_container->group(index);
}

PlayListGroup *PlayListModel::group(const PlayListTrack *track) const
{
    for(int i = 0; i < m_container->groupCount(); ++i)
    {
        if(m_container->group(i)->contains(track))
            return m_container->group(i);
    }

    return nullptr;
}

QList<PlayListGroup *> PlayListModel::groups() const
{
    return m_container->groups();
}

int PlayListModel::currentIndex() const
{
    return m_current;
}

bool PlayListModel::setCurrent(int index)
{
    if(index > trackCount() - 1 || index < 0)
        return false;
    PlayListTrack *track = m_container->track(index);
    m_current = index;
    m_current_track = track;
    emit listChanged(CURRENT);
    return true;
}

bool PlayListModel::setCurrent(PlayListTrack *track)
{
    return setCurrent(m_container->indexOf(track));
}

bool PlayListModel::next()
{
    if(m_stop_track == currentTrack())
    {
        m_stop_track = nullptr;
        emit listChanged(STOP_AFTER);
        return false;
    }
    if(!isEmptyQueue())
    {
        m_current_track = m_container->dequeue();
        m_current = m_container->indexOf(m_current_track);
        emit listChanged(CURRENT | QUEUE);
        return true;
    }

    if(m_loader->isRunning())
        m_play_state->prepare();
    return m_play_state->next();
}

bool PlayListModel::previous()
{
    if(m_loader->isRunning())
        m_play_state->prepare();
    return m_play_state->previous();
}

int PlayListModel::lineCount() const
{
    return m_container->lineCount();
}

PlayListItem *PlayListModel::itemAtLine(int lineIndex) const
{
    return m_container->itemAtLine(lineIndex);
}

PlayListTrack *PlayListModel::trackAtLine(int lineIndex) const
{
    int l = m_container->trackIndexAtLine(lineIndex);
    return l >= 0 ? m_container->track(l) : nullptr;
}

QList<PlayListItem *> PlayListModel::itemsAtLines(int pos, int count) const
{
    return m_container->itemsAtLines(pos, count);
}

int PlayListModel::findLine(const PlayListItem *item) const
{
    if(!item)
        return -1;

    for(int i = 0; i < m_container->lineCount(); ++i)
    {
        if(m_container->itemAtLine(i) == item)
            return i;
    }
    return -1;
}

int PlayListModel::findLine(int trackIndex) const
{
    return findLine(m_container->track(trackIndex));
}

int PlayListModel::subIndexOfLine(int lineIndex) const
{
    return m_container->subIndexOfLine(lineIndex);
}

int PlayListModel::trackIndexAtLine(int lineIndex) const
{
    return m_container->trackIndexAtLine(lineIndex);
}

bool PlayListModel::alternateColor(int lineIndex) const
{
    return m_container->alternateColor(lineIndex);
}

int PlayListModel::linesPerGroup() const
{
    return m_container->linesPerGroup();
}

void PlayListModel::clear()
{
    m_loader->finish();
    m_coverLoder->finish();
    m_current = -1;
    if(m_current_track)
    {
        m_current_track = nullptr;
        emit currentTrackRemoved();
    }
    m_stop_track = nullptr;
    m_container->clear();
    m_total_duration = 0;
    m_play_state->resetState();
    emit listChanged(STRUCTURE | QUEUE | STOP_AFTER | CURRENT | SELECTION);
}

void PlayListModel::clearSelection()
{
    m_container->clearSelection();
    emit listChanged(SELECTION);
}

bool PlayListModel::contains(const QString &url)
{
    for(int i = 0; i < m_container->trackCount(); ++i)
    {
        PlayListTrack *t = m_container->track(i);
        if(t->path() == url)
            return true;
    }
    return false;
}

PlayListTrack *PlayListModel::findTrack(int trackIndex) const
{
    return m_container->track(trackIndex);
}

QList<PlayListItem *> PlayListModel::findTracks(const QString &str) const
{
    QList<PlayListItem *> items;
    PlayListItem *item = nullptr;
    if(str.isEmpty())
        return items;

    for(int i = 0; i < m_container->trackCount(); ++i)
    {
        item = m_container->track(i);

        if(!item->formattedTitles().filter(str, Qt::CaseInsensitive).isEmpty())
            items.append(item);
    }
    return items;
}

void PlayListModel::setSelected(PlayListItem *item, bool selected)
{
    if(item)
    {
        item->setSelected(selected);
        emit listChanged(SELECTION);
    }
}

void PlayListModel::setSelected(const QList<PlayListTrack *> &tracks, bool selected)
{
    for(PlayListTrack *t : std::as_const(tracks))
        t->setSelected(selected);
    emit listChanged(SELECTION);
}

void PlayListModel::setSelected(const QList<PlayListItem *> &items, bool selected)
{
    for(PlayListItem *i : std::as_const(items))
        i->setSelected(selected);
    emit listChanged(SELECTION);
}

void PlayListModel::setSelectedLines(int firstLine, int lastLine, bool selected)
{
    if(firstLine > lastLine)
    {
        setSelectedLines(lastLine, firstLine, selected);
        return;
    }

    for(int index = firstLine; index <= lastLine; ++index)
    {
        PlayListItem *item = m_container->itemAtLine(index);
        if(item)
            item->setSelected(selected);
    }

    emit listChanged(SELECTION);
}

void PlayListModel::removeSelected()
{
    removeSelection(false);
}

void PlayListModel::removeUnselected()
{
    removeSelection(true);
}

void PlayListModel::removeTrack(int i)
{
    int flags = removeTrackInternal(i);
    if(flags)
    {
        emit listChanged(flags);
        preparePlayState();
    }
}

void PlayListModel::removeTrack(PlayListTrack *track)
{
    if(m_container->contains(track))
        removeTrack(m_container->indexOf(track));
}

void PlayListModel::removeTracks(const QList<PlayListItem *> &items)
{
    int i = 0;
    int select_after_delete = -1;
    int flags = 0;

    while (!m_container->isEmpty() && i < m_container->trackCount())
    {
        PlayListItem *item = m_container->track(i);
        if(!item->isGroup() && items.contains(item))
        {
            flags |= removeTrackInternal(i);

            if(m_container->isEmpty())
                continue;

            select_after_delete = i;
        }
        else
        {
            i++;
        }
    }

    select_after_delete = qMin(select_after_delete, m_container->trackCount() - 1);

    if(select_after_delete >= 0)
    {
        m_container->track(select_after_delete)->setSelected(true);
        flags |= SELECTION;
    }

    preparePlayState();

    if(flags)
        emit listChanged(flags);
}

void PlayListModel::removeTracks(const QList<PlayListTrack *> &tracks)
{
    QList<PlayListItem *> items;
    for(PlayListTrack *track : tracks)
        items << static_cast<PlayListItem *>(track);

    removeTracks(items);
}

void PlayListModel::removeSelection(bool inverted)
{
    QList<PlayListItem *> tracksToRemove;

    for(PlayListTrack *t : m_container->tracks())
    {
        if(t->isSelected() ^ inverted)
            tracksToRemove << t;
    }

    removeTracks(tracksToRemove);
}

int PlayListModel::removeTrackInternal(int i)
{
    if((i < 0) || (i >= m_container->trackCount()))
        return 0;

    int flags = 0;
    PlayListTrack* track = m_container->track(i);
    if(!track)
        return flags;
    if(track->isQueued())
        flags |= QUEUE;
    m_container->removeTrack(track);
    if(m_stop_track == track)
    {
        flags |= STOP_AFTER;
        m_stop_track = nullptr;
    }
    if(track->isSelected())
        flags |= SELECTION;

    m_total_duration -= track->duration();
    m_total_duration = qMax(0LL, m_total_duration);

    if(m_current_track == track)
    {
        flags |= CURRENT;
        if(m_container->isEmpty())
            m_current_track = nullptr;
        else
        {
            m_current = i > 0 ? qMin(i - 1, m_container->trackCount() - 1) : 0;
            m_current_track = m_container->track(m_current);
            emit currentTrackRemoved();
        }
    }

    if(track->isUsed())
        track->deleteLater();
    else
        delete track;

    m_current = m_current_track ? m_container->indexOf(m_current_track) : -1;

    flags |= STRUCTURE;
    return flags;
}

void PlayListModel::invertSelection()
{
    for(int i = 0; i < m_container->trackCount(); ++i)
    {
        PlayListTrack *track = m_container->track(i);
        track->setSelected(!track->isSelected());
    }
    for(int i = 0; i < m_container->groupCount(); ++i)
    {
        PlayListGroup *group = m_container->group(i);
        group->setSelected(!group->isSelected());
    }
    emit listChanged(SELECTION);
}

void PlayListModel::selectAll()
{
    for(int i = 0; i < m_container->trackCount(); ++i)
        m_container->track(i)->setSelected(true);
    for(int i = 0; i < m_container->groupCount(); ++i)
        m_container->group(i)->setSelected(true);
    emit listChanged(SELECTION);
}

void PlayListModel::showDetails(QWidget *parent)
{
    QList<PlayListTrack *> selected_tracks = selectedTracks();

    if(!selected_tracks.isEmpty())
    {
        DetailsDialog *d = new DetailsDialog(selected_tracks, parent);
        d->setAttribute(Qt::WA_DeleteOnClose, true);
        connect(d, &DetailsDialog::metaDataChanged, this, qOverload<const QStringList &>(&PlayListModel::updateMetaData));
        d->show();
    }
}

void PlayListModel::showDetailsForCurrent(QWidget *parent)
{
    if(m_current_track)
    {
        DetailsDialog *d = new DetailsDialog(QList<PlayListTrack *>() << m_current_track, parent);
        d->setAttribute(Qt::WA_DeleteOnClose, true);
        connect(d, &DetailsDialog::metaDataChanged, this, qOverload<const QStringList &>(&PlayListModel::updateMetaData));
        d->show();
    }
}

int PlayListModel::firstSelectedLine() const
{
    for(int i = 0; i < m_container->lineCount(); i++)
    {
        if(m_container->itemAtLine(i)->isSelected())
            return i;
    }
    return -1;
}

int PlayListModel::lastSelectedLine() const
{
    for(int i = m_container->lineCount() - 1; i >= 0; i--)
    {
        if(m_container->itemAtLine(i)->isSelected())
            return i;
    }
    return -1;
}

qint64 PlayListModel::totalDuration() const
{
    return m_total_duration;
}

void PlayListModel::moveTracks(int from, int to)
{
    // Get rid of useless work
    if(from == to || from < 0 || to < 0)
        return;

    QList<int> selected_indexes = selectedTrackIndexes();
    QList<PlayListGroup *> groups = m_container->groups();

    if(selected_indexes.isEmpty())
        return;

    if(std::any_of(groups.cbegin(), groups.cend(), [](PlayListGroup *g){ return g->isSelected(); }))
        return;

    if(from == -1 || bottommostInSelection(from) == -1 || topmostInSelection(from) == -1)
        return;

    if(m_container->move(selected_indexes, from, to))
    {
        m_current = m_container->indexOf(m_current_track);
        emit listChanged(STRUCTURE);
    }
}

int PlayListModel::topmostInSelection(int row)
{
    if(row == 0)
        return 0;

    for(int i = row - 1; i >= 0; i--)
    {
        if(m_container->track(i)->isSelected())
            continue;

        return i + 1;
    }
    return 0;
}

int PlayListModel::bottommostInSelection(int row)
{
    if(row >= trackCount() - 1)
        return row;

    for(int i = row + 1; i < trackCount(); i++)
    {
        if(m_container->track(i)->isSelected())
            continue;

        return i - 1;
    }
    return trackCount() - 1;
}

SimpleSelection PlayListModel::getSelection(int trackIndex)
{
    SimpleSelection sel = { .top = topmostInSelection(trackIndex), .bottom = bottommostInSelection(trackIndex) };
    return sel;
}

QList<int> PlayListModel::selectedLines() const
{
    QList<int> lines;
    for(int i = 0; i < m_container->lineCount(); i++)
    {
        if(m_container->itemAtLine(i)->isSelected())
            lines.append(i);
    }
    return lines;
}

void PlayListModel::setSelectedLine(int line, bool selected)
{
    PlayListItem *item = m_container->itemAtLine(line);
    if(item)
    {
        item->setSelected(selected);
        emit listChanged(SELECTION);
    }
}

QList<int> PlayListModel::selectedTrackIndexes() const
{
    QList<int> selected_rows;
    for(int i = 0; i < m_container->trackCount(); i++)
    {
        if(m_container->track(i)->isSelected())
        {
            selected_rows.append(i);
        }
    }
    return selected_rows;
}

QList<PlayListTrack *> PlayListModel::selectedTracks() const
{
    QList<PlayListTrack *> selected_tracks;
    for(PlayListTrack *track : m_container->tracks())
    {
        if(track->isSelected())
            selected_tracks << track;
    }
    return selected_tracks;
}

QList<PlayListTrack *> PlayListModel::tracks() const
{
    return m_container->tracks();
}

void PlayListModel::addToQueue()
{
    const QList<PlayListTrack*> selected_tracks = selectedTracks();
    blockSignals(true);
    for(PlayListTrack *track : std::as_const(selected_tracks))
        setQueued(track);
    blockSignals(false);
    emit listChanged(QUEUE);
}

void PlayListModel::setQueued(PlayListTrack *t)
{
    if(t->isQueued())
        m_container->removeFromQueue(t);
    else
        m_container->enqueue(t);
    emit listChanged(QUEUE);
}

const QList<PlayListTrack *> &PlayListModel::queuedTracks() const
{
    return m_container->queuedTracks();
}

bool PlayListModel::isEmptyQueue() const
{
    return m_container->queuedTracks().isEmpty();
}

int PlayListModel::queueSize() const
{
    return m_container->queuedTracks().count();
}

bool PlayListModel::isStopAfter(const PlayListItem *track) const
{
    return m_stop_track == track;
}

void PlayListModel::randomizeList()
{
    if(m_container->isEmpty())
        return;
    m_container->randomizeList();
    m_current = m_container->indexOf(m_current_track);
    emit listChanged(STRUCTURE);
}

void PlayListModel::reverseList()
{
    if(m_container->isEmpty())
        return;
    m_container->reverseList();
    m_current = m_container->indexOf(m_current_track);
    emit listChanged(STRUCTURE);
}

void PlayListModel::sortSelection(PlayListModel::SortMode mode)
{
    if(m_container->isEmpty())
        return;

    m_task->sortSelection(m_container->tracks(), mode);
}

void PlayListModel::sort(PlayListModel::SortMode mode)
{
    if(m_container->isEmpty())
        return;

    m_task->sort(m_container->tracks(), mode);
}

void PlayListModel::sortByColumn(int column)
{
    if(m_container->isEmpty())
        return;

    if(column < 0 || column >= columnCount())
        return;

    m_task->sortByColumn(m_container->tracks(), column);
}

void PlayListModel::prepareForShufflePlaying(bool val)
{
    delete m_play_state;

    if(val)
        m_play_state = new ShufflePlayState(this);
    else
        m_play_state = new NormalPlayState(this);
}

void PlayListModel::prepareGroups(bool enabled)
{
    PlayListContainer *container = nullptr;
    if(enabled)
        container = new GroupedContainer;
    else
        container = new NormalContainer;
    container->setLinesPerGroup(m_ui_settings->linesPerGroup());
    container->addTracks(m_container->takeAllTracks());
    delete m_container;
    m_container = container;
    if(!m_container->isEmpty())
        m_current = m_container->indexOf(m_current_track);
    emit listChanged(STRUCTURE);
    startCoverLoader();
}

void PlayListModel::updateMetaData()
{
    emit listChanged(METADATA);
}

void PlayListModel::onTaskFinished()
{
    if(m_task->isChanged(m_container)) //update unchanged container only
    {
        m_task->clear();
        return;
    }

    QList<PlayListTrack *> queuedTracks = m_container->queuedTracks();

    if(m_task->type() == PlayListTask::SORT || m_task->type() == PlayListTask::SORT_SELECTION)
    {
        m_container->replaceTracks(m_task->takeResults(&m_current_track));
        m_container->restoreQueue(queuedTracks);
        m_current = m_container->indexOf(m_current_track);
        emit listChanged(STRUCTURE);
    }
    else if(m_task->type() == PlayListTask::SORT_BY_COLUMN)
    {
        m_container->replaceTracks(m_task->takeResults(&m_current_track));
        m_container->restoreQueue(queuedTracks);
        m_current = m_container->indexOf(m_current_track);
        emit listChanged(STRUCTURE);
        emit sortingByColumnFinished(m_task->column(), m_task->isReverted());
    }
    else if(m_task->type() == PlayListTask::REMOVE_INVALID
            || m_task->type() == PlayListTask::REMOVE_DUPLICATES
            || m_task->type() == PlayListTask::REFRESH)
    {
        PlayListTrack *prev_current_track = m_current_track;
        int prev_count = m_container->trackCount();

        m_container->replaceTracks(m_task->takeResults(&m_current_track));

        int flags = METADATA;
        if(prev_count != m_container->trackCount())
        {
            flags = STRUCTURE;
            m_current = m_current_track ? m_container->indexOf(m_current_track) : -1;
            if(prev_current_track != m_current_track)
            {
                flags |= CURRENT;
                emit currentTrackRemoved();
            }

            if(m_stop_track && !m_container->contains(m_stop_track))
            {
                m_stop_track = nullptr;
                flags |= STOP_AFTER;
            }

            //remove deleted tracks from queue
            QList<PlayListTrack *>::iterator it = queuedTracks.begin();
            while(it != queuedTracks.end())
            {
                if(!m_container->contains(*it))
                {
                    flags |= QUEUE;
                    it = queuedTracks.erase(it);
                }
                else
                {
                    ++it;
                }
            }

            preparePlayState();
        }
        m_container->restoreQueue(queuedTracks);
        emit listChanged(flags);
    }
}

void PlayListModel::updateMetaData(const QStringList &paths)
{
    if(m_container->isEmpty())
        return;

    QList<PlayListTrack *> tracksToRemove;
    QList<PlayListTrack *> tracksToAdd;

    QHash<QString, TrackInfo *> cache; //cache for tracks
    QSet<QString> multiTrackFiles; //files with multiple tracks

    for(const QString &path : std::as_const(paths))
    {
        bool missing = false; //track is missing

        //update cache
        if(!cache.contains(path))
        {
            //is it track of local file?
            if(path.contains(u"://"_s) && path.contains(QLatin1Char('#')) && !cache.contains(path))
            {
                QString filePath = TrackInfo::pathFromUrl(path);
                if(multiTrackFiles.contains(filePath)) //looks like local file has been already scanned, but does not contain this track
                {
                    missing = true;
                }
                else if(QFileInfo(filePath).isFile())
                {
                    const QList<TrackInfo *> list = MetaDataManager::instance()->createPlayList(filePath);
                    for(TrackInfo *info : std::as_const(list))
                        cache.insert(info->path(), info);

                    multiTrackFiles << path;
                }
            }
            else if(QFileInfo(path).isFile()) //is it local file?
            {
                const QList<TrackInfo *> list = MetaDataManager::instance()->createPlayList(path);
                for(TrackInfo *info : std::as_const(list))
                    cache.insert(info->path(), info);
            }
        }

        QList<TrackInfo *> list;
        if(cache.contains(path)) //using TrackInfo object from cache
            list << cache.value(path);
        else if(!missing)
            list << MetaDataManager::instance()->createPlayList(path);

        for(int i = 0; i < m_container->trackCount(); ++i)
        {
            PlayListTrack *track = m_container->track(i);
            if(!track)
                continue;

            if(track->path() == path)
            {
                if(list.isEmpty()) //track is not available
                    tracksToRemove << track;
                else if(list.count() == 1)
                {
                    track->updateMetaData(list.constFirst()); //update single track
                }
                else //replace single track by multiple tracks
                {
                    track->updateMetaData(list.constFirst()); //update existing track
                    delete list.takeFirst();
                    for(const TrackInfo *info : std::as_const(list)) //add remaining tracks
                        tracksToAdd << new PlayListTrack(info);
                }
            }
        }

        if(!cache.contains(path))
            qDeleteAll(list);
    }

    qDeleteAll(cache);
    cache.clear();

    if(!tracksToRemove.isEmpty())
        removeTracks(tracksToRemove);
    if(!tracksToAdd.isEmpty())
        addTracks(tracksToAdd);

    updateMetaData();
}

void PlayListModel::startCoverLoader()
{
    if(m_container->groupCount() > 0 && m_container->linesPerGroup() > 1)
    {
        const QList<PlayListGroup *> groups = m_container->groups();
        QStringList paths;
        for(const PlayListGroup *g : std::as_const(groups))
        {
            if(!g->isCoverLoaded() && !g->firstTrackPath().isEmpty())
                paths << g->firstTrackPath();
        }

        m_coverLoder->add(paths);
    }
}

void PlayListModel::setCover(const QString &path, const QImage &img)
{
    for(PlayListGroup *g : m_container->groups())
    {
        if(g->firstTrackPath() == path)
            g->setCover(img);
    }
    emit listChanged(METADATA);
}

void PlayListModel::insertTracksInternal(PlayListTrack *before, const QList<PlayListTrack *> &tracks)
{
    if(m_ui_settings->skipExistingTracks() && sender() == m_loader)
    {
        if(m_uniquePaths.isEmpty())
        {
            m_uniquePaths.reserve(m_container->trackCount());
            for(const PlayListTrack *track : m_container->tracks())
            {
                m_uniquePaths.insert(track->path());
            }
        }

        QList<PlayListTrack *> uniqueTracks;
        for(PlayListTrack *track : std::as_const(tracks))
        {
            if(!m_uniquePaths.contains(track->path()))
            {
                m_uniquePaths.insert(track->path());
                uniqueTracks << track;
            }
        }

        if(before)
            insertTracks(m_container->indexOf(before), uniqueTracks);
        else
            addTracks(uniqueTracks);
    }
    else
    {
        if(before)
            insertTracks(m_container->indexOf(before), tracks);
        else
            addTracks(tracks);
    }
}

void PlayListModel::doCurrentVisibleRequest()
{
    if(!m_container->isEmpty() && m_current >= 0)
        emit scrollToRequest(m_current);
}

void PlayListModel::scrollTo(int trackIndex)
{
    if(trackIndex >= 0 && trackIndex < m_container->trackCount())
        emit scrollToRequest(trackIndex);
}

void PlayListModel::loadPlaylist(const QString &f_name)
{
    m_loader->add(f_name);
}

void PlayListModel::loadPlaylist(const QString &fmt, const QByteArray &data)
{
    m_loader->addPlayList(fmt, data);
}

void PlayListModel::savePlaylist(const QString &f_name)
{
    PlayListParser::savePlayList(m_container->tracks(), f_name);
}

bool PlayListModel::isLoaderRunning() const
{
    return m_loader->isRunning();
}

void PlayListModel::preparePlayState()
{
    m_play_state->prepare();
    m_uniquePaths.clear();
    m_uniquePaths.squeeze();
}

void PlayListModel::removeInvalidTracks()
{
    m_task->removeInvalidTracks(m_container->tracks(), m_current_track);
}

void PlayListModel::removeDuplicates()
{
    m_task->removeDuplicates(m_container->tracks(), m_current_track);
}

void PlayListModel::refresh()
{
    m_task->refresh(m_container->tracks(), m_current_track);
}

void PlayListModel::clearQueue()
{
     m_container->clearQueue();
     m_stop_track = nullptr;
     emit listChanged(QUEUE);
}

void PlayListModel::stopAfterSelected()
{
    QList<PlayListTrack*> selected_tracks = selectedTracks();

    if(!isEmptyQueue())
    {
        m_stop_track = m_stop_track != m_container->queuedTracks().constLast() ? m_container->queuedTracks().constLast() : nullptr;
        emit listChanged(STOP_AFTER);
    }
    else if(selected_tracks.count() == 1)
    {
        m_stop_track = m_stop_track != selected_tracks.constFirst() ? selected_tracks.constFirst() : nullptr;
        emit listChanged(STOP_AFTER);
    }
    else if(selected_tracks.count() > 1)
    {
        blockSignals(true);
        addToQueue();
        blockSignals(false);
        m_stop_track = m_container->queuedTracks().last();
        emit listChanged(STOP_AFTER | QUEUE);
    }
}

void PlayListModel::rebuildGroups()
{
    if(m_ui_settings->isGroupsEnabled())
        prepareGroups(true);
}
