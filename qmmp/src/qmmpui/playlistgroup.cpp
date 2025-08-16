/***************************************************************************
 *   Copyright (C) 2013-2025 by Ilya Kotov                                 *
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
#include "qmmpuisettings.h"
#include "metadatahelper_p.h"
#include "playlistgroup.h"

PlayListGroup::PlayListGroup(const QString &groupName) : m_groupName(groupName)
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

QString PlayListGroup::formattedTitle(int line) const
{
    if(line == 0)
    {
        if(m_title0.isEmpty())
            m_title0 = formatTitle0();

        return m_title0;
    }

    if(line == 1)
    {
        if(m_title1.isEmpty())
            m_title1 = formatTitle1();

        return m_title1;
    }

    return QString();
}

QStringList PlayListGroup::formattedTitles() const
{
    if(m_title0.isEmpty())
        m_title0 = formatTitle0();

    if(m_title1.isEmpty())
        m_title1 = formatTitle1();

    return { m_title0, m_title1 };
}

bool PlayListGroup::contains(const PlayListTrack *track) const
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

QString PlayListGroup::groupName() const
{
    return m_groupName;
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

QImage PlayListGroup::cover() const
{
    return m_cover;
}

void PlayListGroup::setCover(const QImage &cover)
{
    m_isCoverLoaded = true;
    m_cover = cover;
}

void PlayListGroup::updateMetaData()
{
    m_title0.clear();
    m_title1.clear();
}

QString PlayListGroup::formatTitle0() const
{
    if(m_trackList.isEmpty())
        return QString();

    if(m_trackList.constFirst()->path().contains(u"://"_s) && !m_trackList.constFirst()->path().contains(QLatin1Char('#')))
        return qApp->translate("PlayListGroup", "Streams");

    qint64 duration = 0;
    for(const PlayListTrack *t : std::as_const(m_trackList))
        duration += t->duration();

    TrackInfo info = *m_trackList.constFirst();
    info.setDuration(duration);

    QString title = m_helper->groupFormatter0()->format(info);
    if(m_settings->convertUnderscore())
        title.replace(QLatin1Char('_'), QChar::Space);
    if(m_settings->convertTwenty())
        title.replace(u"%20"_s, u" "_s);

    if(title.isEmpty())
        return qApp->translate("PlayListGroup", "Empty group");

    return title;
}

QString PlayListGroup::formatTitle1() const
{
    if(m_trackList.isEmpty() || m_trackList.constFirst()->properties().isEmpty())
        return QString();

    qint64 duration = 0;
    for(const PlayListTrack *t : std::as_const(m_trackList))
        duration += t->duration();

    TrackInfo info = *m_trackList.constFirst();
    info.setDuration(duration);

    QString title = m_helper->groupFormatter1()->format(info);
    if(m_settings->convertUnderscore())
        title.replace(QLatin1Char('_'), QChar::Space);
    if(m_settings->convertTwenty())
        title.replace(u"%20"_s, u" "_s);

    return title;
}
