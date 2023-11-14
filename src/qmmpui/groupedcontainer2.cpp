#include <QRandomGenerator>
#include "groupedcontainer2_p.h"

GroupedContainer2::GroupedContainer2()
{

}

void GroupedContainer2::addTrack(PlayListTrack *track)
{
    int firstIndex = 0, lastIndex = 0;
    //insert if possible
    for(int i = 0; i < m_groups.count(); ++i)
    {
        if(i == 0)
        {
           firstIndex = 0;
           lastIndex = m_groups[i]->count() - 1;
        }
        else
        {
            firstIndex = lastIndex + 1;
            lastIndex = firstIndex + m_groups[i]->count() - 1;
        }

        if(track->groupName() == m_groups[i]->formattedTitle())
        {
            m_groups[i]->trackList.append(track);
            m_tracks.insert(lastIndex + 1, track);
            m_update = true;
            return;
        }
    }
    PlayListGroup *group = new PlayListGroup(track->groupName());
    group->trackList.append(track);
    m_tracks.append(track);
    m_groups.append(group);

    if(!m_update)
    {
        int group_height = 3;

        for(int j = 0; j < group_height; ++j)
        {
            PlayListLine line = { .isGroup = true, .index = int(m_groups.count() - 1), .subindex = j };
            m_lines << line;
        }

        m_lines << PlayListLine { .isGroup = false, .index = int(m_tracks.count() - 1), .subindex = 0 };
    }
}

void GroupedContainer2::addTracks(const QList<PlayListTrack *> &tracks)
{
    m_tracks.reserve(m_tracks.count() + tracks.count());
    for(PlayListTrack *track : qAsConst(tracks))
        addTrack(track);
}

int GroupedContainer2::insertTrack(int index, PlayListTrack *track)
{
    int firstIndex = 0, lastIndex = 0;
    //insert if possible
    for(int i = 0; i < m_groups.count(); ++i)
    {
        if(i == 0)
        {
           firstIndex = 0;
           lastIndex = m_groups[i]->count() - 1;
        }
        else
        {
            firstIndex = lastIndex + 1;
            lastIndex = firstIndex + m_groups[i]->count() - 1;
        }

        if(track->groupName() == m_groups[i]->formattedTitle() &&
                index >= firstIndex && index <= lastIndex + 1)
        {
            m_groups[i]->trackList.insert(index - firstIndex, track);
            m_tracks.insert(index, track);
            m_update = true;
            return index;
        }
    }
    //just add otherwise
    addTrack(track);
    return m_tracks.count() - 1;
}

void GroupedContainer2::replaceTracks(const QList<PlayListTrack *> &tracks)
{
    for(PlayListGroup *g : qAsConst(m_groups))
    {
        g->trackList.clear();
    }
    clear();
    addTracks(tracks);
}

QList<PlayListGroup *> GroupedContainer2::groups() const
{
    return m_groups;
}

QList<PlayListTrack *> GroupedContainer2::tracks() const
{
    return m_tracks;
}

int GroupedContainer2::groupCount() const
{
    return m_groups.count();
}

int GroupedContainer2::trackCount() const
{
    return m_tracks.count();
}

QList<PlayListTrack *> GroupedContainer2::mid(int pos, int count) const
{
    updateCache();
    return m_tracks.mid(pos, count);
}

bool GroupedContainer2::isEmpty() const
{
    return m_tracks.isEmpty();
}

bool GroupedContainer2::isSelected(int index) const
{
    return m_tracks.at(index)->isSelected();
}

void GroupedContainer2::setSelected(int index, bool selected)
{
    return m_tracks.at(index)->setSelected(selected);
}

void GroupedContainer2::clearSelection()
{
    for(PlayListTrack *track : qAsConst(m_tracks))
        track->setSelected(false);
}

int GroupedContainer2::indexOf(PlayListTrack *track) const
{
    return m_tracks.indexOf(track);
}

PlayListTrack *GroupedContainer2::track(int index) const
{
    return m_tracks.at(index);
}

PlayListGroup *GroupedContainer2::group(int index) const
{
    return m_groups.at(index);
}

bool GroupedContainer2::contains(PlayListItem *item) const
{
    return item->isGroup() ? m_groups.contains(item) : m_tracks.contains(item);
}

void GroupedContainer2::removeTrack(PlayListTrack *track)
{
    QList<PlayListGroup *>::iterator it = m_groups.begin();
    while(it != m_groups.end())
    {
        if((*it)->contains(track))
        {
            (*it)->trackList.removeAll(track);
            m_tracks.removeAll(track);
            if((*it)->isEmpty())
            {
                PlayListGroup *group = *it;
                m_groups.removeAll(group);
                delete group;
            }
            return;
        }
        ++it;
    }

    m_update = true;
}

void GroupedContainer2::removeTracks(QList<PlayListTrack *> tracks)
{
    for(PlayListTrack *t : qAsConst(tracks))
        removeTrack(t);
}

bool GroupedContainer2::move(const QList<int> &indexes, int from, int to)
{
    PlayListGroup *group = nullptr;
    int firstIndex = 0, lastIndex = 0;

    for(int i = 0; i < m_groups.count(); ++i)
    {
        if(i == 0)
        {
           firstIndex = 0;
           lastIndex = m_groups[i]->count() - 1;
        }
        else
        {
            firstIndex = lastIndex + 1;
            lastIndex = firstIndex + m_groups[i]->count() + 1;
        }

        if(from > firstIndex && from <= lastIndex && to > firstIndex && to <= lastIndex)
        {
            group = m_groups.at(i);
            break;
        }
    }

    if(!group)
        return false;

    for(int i : qAsConst(indexes))
    {
        if(i <= firstIndex || i > lastIndex)
            return false;
        if(i + to - from - firstIndex - 1 >= group->count())
            return false;
        if(i + to - from - firstIndex - 1 < 0)
            return false;
        if(i + to - from < 0)
            return false;
    }

    if (from > to)
    {
        for(const int &i : qAsConst(indexes))
        {
            if (i + to - from < 0)
                break;

            m_tracks.move(i,i + to - from);
            group->trackList.move(i - firstIndex - 1,
                                  i + to - from  - firstIndex - 1);
        }
    }
    else
    {
        for (int i = indexes.count() - 1; i >= 0; i--)
        {
            if (indexes[i] + to - from >= m_tracks.count())
                break;

            m_tracks.move(indexes[i], indexes[i] + to - from);
            group->trackList.move(indexes[i] - firstIndex - 1,
                                  indexes[i] + to - from - firstIndex - 1);
        }
    }
    return true;
}

QList<PlayListTrack *> GroupedContainer2::takeAllTracks()
{
    QList<PlayListTrack *> tracks = m_tracks;
    for(PlayListGroup *g : qAsConst(m_groups))
    {
        g->trackList.clear();
    }
    clear();
    return tracks;
}

void GroupedContainer2::clear()
{
    //clearQueue();
    while(!m_groups.isEmpty())
    {
        delete m_groups.takeFirst();
    }
    m_tracks.clear();
    m_lines.clear();
}

void GroupedContainer2::reverseList()
{
    QList<PlayListTrack *> tracks = takeAllTracks();

    for (int i = 0; i < tracks.size() / 2 ;i++)
        tracks.swapItemsAt(i, tracks.size() - i - 1);

    addTracks(tracks);
}

void GroupedContainer2::randomizeList()
{
    QRandomGenerator *rg = QRandomGenerator::global();
    QList<PlayListTrack *> tracks = takeAllTracks();

    for (int i = 0; i < tracks.size(); i++)
        tracks.swapItemsAt(rg->generate() % tracks.size(), rg->generate() % tracks.size());
    addTracks(tracks);
}

int GroupedContainer2::lineCount() const
{
    updateCache();
    return m_lines.count();
}

int GroupedContainer2::groupAtLine(int line) const
{
    updateCache();
    return m_lines[line].isGroup ? m_lines[line].index : -1;
}

int GroupedContainer2::sublineAtLine(int line) const
{
    updateCache();
    return m_lines[line].isGroup ? m_lines[line].subindex : -1;
}

int GroupedContainer2::trackAtLine(int line) const
{
    updateCache();
    return m_lines[line].isGroup ? -1 : m_lines[line].index;
}

void GroupedContainer2::updateCache() const
{
    if(!m_update)
        return;

    int group_height = 3;
    int t = 0;

    m_lines.clear();
    m_lines.reserve(group_height * m_groups.count() + m_tracks.count());

    for(int g = 0; g < m_groups.count(); ++g)
    {

        for(int j = 0; j < group_height; ++j)
        {
            PlayListLine line = {
                .isGroup = true,
                .index = g,
                .subindex = j
            };
            m_lines << line;
        }

        for(int i = 0; i < m_groups.at(g)->trackList.count(); ++i)
        {
            PlayListLine line = {
                .isGroup = false,
                .index = t++,
                .subindex = 0
            };
            m_lines << line;
        }
    }
    m_update = false;
}
