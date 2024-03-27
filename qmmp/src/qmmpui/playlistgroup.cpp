/***************************************************************************
 *   Copyright (C) 2013-2024 by Ilya Kotov                                 *
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

#include "playlistgroup.h"

PlayListGroup::PlayListGroup(const QString &name) : m_name(name)
{}

PlayListGroup::~PlayListGroup()
{
    while(!trackList.isEmpty())
    {
        PlayListTrack* mf = trackList.takeFirst();

        if (mf->isUsed())
            mf->deleteLater();
        else
            delete mf;
    }
}

QString PlayListGroup::formattedTitle(int column) const
{
    Q_UNUSED(column);
    return m_name;
}

QStringList PlayListGroup::formattedTitles() const
{
    return QStringList() << m_name;
}

bool PlayListGroup::contains(PlayListTrack *track) const
{
    return trackList.contains(track);
}

bool PlayListGroup::isEmpty() const
{
    return trackList.isEmpty();
}

QList<PlayListTrack *> PlayListGroup::tracks() const
{
    return trackList;
}

int PlayListGroup::count() const
{
    return trackList.count();
}

QString PlayListGroup::formattedDuration() const
{
    return QString();
}

bool PlayListGroup::isGroup() const
{
    return true;
}

QString PlayListGroup::firstTrackPath() const
{
    return trackList.isEmpty() ? QString() : trackList.constFirst()->path();
}

bool PlayListGroup::isCoverLoaded() const
{
    return m_isCoverLoaded;
}

QPixmap PlayListGroup::cover() const
{
    return m_cover;
}

void PlayListGroup::setCover(const QPixmap &cover)
{
    m_isCoverLoaded = true;
    m_cover = cover;
}
