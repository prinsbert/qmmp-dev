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

#include "qmmpuisettings.h"
#include "metadatahelper_p.h"
#include "playlistgroup.h"

PlayListGroup::PlayListGroup(const QString &name) : m_title(name)
{
    m_settings = QmmpUiSettings::instance();
    m_helper = MetaDataHelper::instance();
}

PlayListGroup::~PlayListGroup()
{
    while(!m_trackList.isEmpty())
    {
        PlayListTrack* mf = m_trackList.takeFirst();

        if (mf->isUsed())
            mf->deleteLater();
        else
            delete mf;
    }
}

QString PlayListGroup::formattedTitle(int column) const
{
    if(column == 0)
        return m_title;

    if(column == 1)
    {
        if(m_title2.isEmpty())
            m_title2 = formatTitle2();

        return m_title2;
    }

    return QString();
}

QStringList PlayListGroup::formattedTitles() const
{
    if(m_title2.isEmpty())
        m_title2 = formatTitle2();

    if(!m_title2.isEmpty())
        return { m_title, m_title2 };

    return { m_title };
}

bool PlayListGroup::contains(PlayListTrack *track) const
{
    return m_trackList.contains(track);
}

bool PlayListGroup::isEmpty() const
{
    return m_trackList.isEmpty();
}

QList<PlayListTrack *> PlayListGroup::tracks() const
{
    return m_trackList;
}

int PlayListGroup::count() const
{
    return m_trackList.count();
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
    return m_trackList.isEmpty() ? QString() : m_trackList.constFirst()->path();
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

QString PlayListGroup::formatTitle2() const
{
    if(m_trackList.isEmpty())
        return QString();

    qint64 duration = 0;
    for(const PlayListTrack *t : qAsConst(m_trackList))
        duration += t->duration();

    TrackInfo info = *m_trackList.constFirst();
    info.setDuration(duration);

    return m_helper->groupFormatter2()->format(info);
}
