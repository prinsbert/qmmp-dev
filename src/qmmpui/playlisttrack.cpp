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

#include <qmmp/metadatamanager.h>
#include "qmmpuisettings.h"
#include "metadatahelper_p.h"
#include "playlisttrack.h"

PlayListTrack::PlayListTrack() : TrackInfo(), PlayListItem()
{
    m_settings = QmmpUiSettings::instance();
    m_helper = MetaDataHelper::instance();
}

PlayListTrack::PlayListTrack(const PlayListTrack &other) : TrackInfo(other),
    PlayListItem()
{
    m_settings = QmmpUiSettings::instance();
    m_helper = MetaDataHelper::instance();

    m_formattedTitles = other.m_formattedTitles;
    m_group = other.m_group;
    m_formattedLength = other.m_formattedLength;
    m_titleFormats = other.m_titleFormats;
    m_groupFormat = other.m_groupFormat;
    setSelected(other.isSelected());
    m_formattedLength = other.m_formattedLength;
}

PlayListTrack::PlayListTrack(const TrackInfo *info) : TrackInfo(*info),
    PlayListItem()
{
    m_settings = QmmpUiSettings::instance();
    m_helper = MetaDataHelper::instance();
}

PlayListTrack::~PlayListTrack()
{
    if(m_refCount != 0)
        qCWarning(core, "deleting busy track");
}

PlayListTrack &PlayListTrack::operator=(const PlayListTrack &other)
{
    TrackInfo::operator =(other);
    m_formattedTitles = other.m_formattedTitles;
    m_group = other.m_group;
    m_formattedLength = other.m_formattedLength;
    m_titleFormats = other.m_titleFormats;
    m_groupFormat = other.m_groupFormat;
    setSelected(other.isSelected());
    m_formattedLength = other.m_formattedLength;
    return *this;
}

void PlayListTrack::updateMetaData(const TrackInfo *info)
{
    setValues(info->metaData());
    if(info->parts() & TrackInfo::Properties)
        setValues(info->properties());
    if(info->parts() & TrackInfo::ReplayGainInfo)
        setValues(info->replayGainInfo());
    setDuration(info->duration());
    setPath(info->path());
    m_formattedTitles.clear();
    m_formattedLength.clear();
    m_group.clear();
    formatGroup();
}

void PlayListTrack::updateMetaData(const TrackInfo &info)
{
    updateMetaData(&info);
}

void PlayListTrack::updateMetaData()
{
    QList<TrackInfo *> list = MetaDataManager::instance()->createPlayList(path());
    if(list.count() == 1 && list.constFirst()->path() == path() && list.constFirst()->parts() != TrackInfo::Parts())
    {
        TrackInfo *info = list.constFirst();
        updateMetaData(info);
    }
    qDeleteAll(list);
}

QString PlayListTrack::groupName() const
{
    QString groupFormat = m_settings->groupFormat();
    if(m_settings->groupExtraRowVisible())
        groupFormat.append(m_settings->groupExtraRowFormat());

    if(m_group.isEmpty() || m_groupFormat != groupFormat)
    {
        m_groupFormat = groupFormat;
        formatGroup();
    }
    return m_group;
}

bool PlayListTrack::isGroup() const
{
    return false;
}

int PlayListTrack::trackIndex() const
{
    return m_track_index;
}

int PlayListTrack::queuedIndex() const
{
    return m_queued_index;
}

bool PlayListTrack::isQueued() const
{
    return m_queued_index >= 0;
}

void PlayListTrack::beginUsage()
{
    m_refCount++;
}

void PlayListTrack::endUsage()
{
    m_refCount--;
}

void PlayListTrack::deleteLater()
{
    m_sheduledForDeletion = true;
}

bool PlayListTrack::isSheduledForDeletion() const
{
    return m_sheduledForDeletion;
}

bool PlayListTrack::isUsed() const
{
    return (m_refCount != 0);
}

QString PlayListTrack::formattedTitle(int column) const
{
    if(m_formattedTitles.count() != m_helper->columnCount())
    {
        while(m_formattedTitles.count() > m_helper->columnCount())
            m_formattedTitles.takeLast();

        while(m_formattedTitles.count() < m_helper->columnCount())
            m_formattedTitles.append(QString());

        while(m_titleFormats.count() > m_helper->columnCount())
            m_titleFormats.takeLast();

        while(m_titleFormats.count() < m_helper->columnCount())
            m_titleFormats.append(QString());
    }

    if(column < 0 || column >= m_formattedTitles.size())
    {
        qCWarning(core, "column number is out of range");
        return QString();
    }

    if(m_formattedTitles[column].isEmpty() || m_titleFormats[column] != m_helper->titleFormatter(column)->pattern())
    {
        m_titleFormats[column] = m_helper->titleFormatter(column)->pattern();
        formatTitle(column);
    }
    return m_formattedTitles[column];
}

QStringList PlayListTrack::formattedTitles() const
{
    if(m_formattedTitles.count() != m_helper->columnCount())
    {
        while(m_formattedTitles.count() > m_helper->columnCount())
            m_formattedTitles.takeLast();

        while(m_formattedTitles.count() < m_helper->columnCount())
            m_formattedTitles.append(QString());

        while(m_titleFormats.count() > m_helper->columnCount())
            m_titleFormats.takeLast();

        while(m_titleFormats.count() < m_helper->columnCount())
            m_titleFormats.append(QString());
    }

    for(int column = 0; column < m_helper->columnCount(); column++)
    {
        if(m_formattedTitles[column].isEmpty() ||
                m_titleFormats[column] != m_helper->titleFormatter(column)->pattern() ||
                m_titleFormats[column].contains(u"%I"_s))
        {
            m_titleFormats[column] = m_helper->titleFormatter(column)->pattern();
            formatTitle(column);
        }
    }

    return m_formattedTitles;
}

QString PlayListTrack::formattedDuration() const
{
    if(duration() > 0 && m_formattedLength.isEmpty())
        m_formattedLength = MetaDataFormatter::formatDuration(duration());
    else if(duration() <= 0 && !m_formattedLength.isEmpty())
        m_formattedLength.clear();
    return m_formattedLength;
}

void PlayListTrack::formatTitle(int column) const
{
    m_formattedTitles[column] = m_helper->titleFormatter(column)->format(this);
    if(m_formattedTitles.count() == 1)
    {
        if (m_formattedTitles[column].isEmpty())
            m_formattedTitles[column] = path().section(QLatin1Char('/'), -1);
        if (m_formattedTitles[column].isEmpty())
            m_formattedTitles[column] = path();
    }
    if(m_settings->convertUnderscore())
        m_formattedTitles[column].replace(QLatin1Char('_'), QChar::Space);
    if(m_settings->convertTwenty())
        m_formattedTitles[column].replace(u"%20"_s, u" "_s);
}

void PlayListTrack::formatGroup() const
{
    if(path().contains(u"://"_s) && !path().contains(QLatin1Char('#')))
    {
        m_group = QStringLiteral("Streams");
        return;
    }

    //suitable for grouping
    static const QList<Qmmp::MetaData> groupingKeys = {
        Qmmp::ARTIST,
        Qmmp::ALBUMARTIST,
        Qmmp::ALBUM,
        Qmmp::COMMENT,
        Qmmp::GENRE,
        Qmmp::COMPOSER,
        Qmmp::YEAR,
        Qmmp::DISCNUMBER
    };

    PlayListTrack track;
    track.setPath(path());

    for(Qmmp::MetaData key : std::as_const(groupingKeys))
        track.setValue(key, value(key));

    m_group = m_helper->groupFormatter0()->format(track);

    if(m_settings->groupExtraRowVisible())
        m_group += m_helper->groupFormatter1()->format(track);

    if (m_group.isEmpty())
        m_group = QStringLiteral("Empty group");
}
