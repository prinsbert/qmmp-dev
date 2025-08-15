/***************************************************************************
 *   Copyright (C) 2012-2025 by Ilya Kotov                                 *
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

#include <QSettings>
#include <QApplication>
#include <qmmp/qmmp.h>
#include "metadatahelper_p.h"
#include "playlistmanager.h"
#include "qmmpuisettings.h"

QmmpUiSettings *QmmpUiSettings::m_instance = nullptr;

QmmpUiSettings::QmmpUiSettings(QObject *parent) : QObject(parent)
{
    if(m_instance)
        qCFatal(core) << "only one instance is allowed";
    m_instance = this;
    m_helper = new MetaDataHelper;
    QSettings s;
    s.beginGroup(u"PlayList"_s);
    m_group_format = s.value(u"group_format"_s, u"%p%if(%p&%a, - %if(%y,[%y] ,),)%a"_s).toString();
    m_group_extra_row_format = s.value(u"group_extra_row_format"_s, tr("%if(%l,%l | ,)%{format} | %{bitrate} kbps | %{samplerate} Hz")).toString();
    m_lines_per_group = s.value(u"lines_per_group"_s, 1).toInt();
    m_group_extra_row_visible = s.value(u"group_extra_row_visible"_s, true).toBool();
    m_group_cover_visible = s.value(u"group_cover_visible"_s, true).toBool();
    m_group_dividing_line_visible = s.value(u"group_dividing_line_visible"_s, true).toBool();
    m_convert_underscore = s.value (u"convert_underscore"_s, true).toBool();
    m_convert_twenty = s.value(u"convert_twenty"_s, true).toBool();
    m_use_metadata = s.value(u"load_metadata"_s, true).toBool();
    m_autosave_playlist = s.value(u"autosave"_s, true).toBool();
    m_repeate_list = s.value(u"repeate_list"_s, false).toBool();
    m_shuffle = s.value(u"shuffle"_s, false).toBool();
    m_groups_enabled = s.value(u"groups"_s, false).toBool();
    m_repeat_track = s.value(u"repeate_track"_s, false).toBool();
    m_no_pl_advance = s.value(u"no_advance"_s, false).toBool();
    m_clear_prev_playlist = s.value(u"clear_previous"_s, false).toBool();
    m_read_metadata_for_playlist = s.value(u"read_metadata_for_playlist"_s, true).toBool();
    m_transit_between_playlists = s.value(u"transit_between_playlists"_s, false).toBool();
    m_skip_existing_tracks = s.value(u"skip_existing_tracks"_s, false).toBool();
    m_stop_after_removing_of_current = s.value(u"stop_after_removing_of_current"_s, false).toBool();
    s.endGroup();
    s.beginGroup(u"General"_s);
    m_resume_on_startup = s.value(u"resume_on_startup"_s, false).toBool();
    m_restrict_filters = s.value(u"restrict_filters"_s).toStringList();
    m_exclude_filters = s.value(u"exclude_filters"_s).toStringList();
    m_use_default_pl = s.value(u"use_default_pl"_s, false).toBool();
    m_default_pl_name = s.value(u"default_pl_name"_s, tr("Playlist")).toString();
    s.endGroup();
    m_use_clipboard = s.value(u"URLDialog/use_clipboard"_s, false).toBool();

    m_helper->setGroupFormat0(m_group_format);
    m_helper->setGroupFormat1(m_group_extra_row_format);
}

QmmpUiSettings::~QmmpUiSettings()
{
    m_instance = nullptr;
    sync();
    delete m_helper;
}

QString QmmpUiSettings::groupFormat() const
{
    return m_group_format;
}

QString QmmpUiSettings::groupExtraRowFormat() const
{
    return m_group_extra_row_format;
}

int QmmpUiSettings::linesPerGroup() const
{
    return m_lines_per_group;
}

bool QmmpUiSettings::groupExtraRowVisible() const
{
    return m_group_extra_row_visible;
}

bool QmmpUiSettings::groupCoverVisible() const
{
    return m_group_cover_visible;
}

bool QmmpUiSettings::groupDividingLineVisible() const
{
    return m_group_dividing_line_visible;
}

bool QmmpUiSettings::isRepeatableList() const
{
    return m_repeate_list;
}

bool QmmpUiSettings::isShuffle() const
{
    return m_shuffle;
}

bool QmmpUiSettings::isGroupsEnabled() const
{
    return m_groups_enabled;
}

bool QmmpUiSettings::isRepeatableTrack() const
{
    return m_repeat_track;
}

bool QmmpUiSettings::isNoPlayListAdvance() const
{
    return m_no_pl_advance;
}

bool QmmpUiSettings::isPlayListTransitionEnabled() const
{
    return m_transit_between_playlists;
}

bool QmmpUiSettings::convertUnderscore() const
{
    return m_convert_underscore;
}

bool QmmpUiSettings::convertTwenty() const
{
    return m_convert_twenty;
}

bool QmmpUiSettings::useMetaData() const
{
    return m_use_metadata;
}

void QmmpUiSettings::setConvertUnderscore(bool yes)
{
    m_convert_underscore = yes;
    saveSettings();
}

void  QmmpUiSettings::setConvertTwenty(bool yes)
{
    m_convert_twenty = yes;
    saveSettings();
}

void QmmpUiSettings::setGroupFormat(const QString &groupFormat)
{
    if(groupFormat != m_group_format)
    {
        m_group_format = groupFormat;
        m_helper->setGroupFormat0(m_group_format);
        saveSettings(true);
    }
}

void QmmpUiSettings::setGroupExtraRowFormat(const QString &extraRowFormat)
{
    if(m_group_extra_row_format != extraRowFormat)
    {
        m_group_extra_row_format = extraRowFormat;
        m_helper->setGroupFormat1(m_group_extra_row_format);
        saveSettings(true);
    }
}

void QmmpUiSettings::setLinesPerGroup(int lines)
{
    if(m_lines_per_group != lines)
    {
        m_lines_per_group = lines;
        saveSettings(true);
    }
}

void QmmpUiSettings::setGroupExtraRowVisible(bool enabled)
{
    if(m_group_extra_row_visible != enabled)
    {
        m_group_extra_row_visible = enabled;
        saveSettings(true);
    }
}

void QmmpUiSettings::setGroupCoverVisible(bool enabled)
{
    if(m_group_cover_visible != enabled)
    {
        m_group_cover_visible = enabled;
        saveSettings(true);
    }
}

void QmmpUiSettings::setGroupDividingLineVisible(bool enabled)
{
    if(m_group_dividing_line_visible != enabled)
    {
        m_group_dividing_line_visible = enabled;
        saveSettings(true);
    }
}

void QmmpUiSettings::setUseMetaData(bool yes)
{
    m_use_metadata = yes;
    saveSettings();
}

bool QmmpUiSettings::resumeOnStartup() const
{
    return m_resume_on_startup;
}

void QmmpUiSettings::setResumeOnStartup(bool enabled)
{
    m_resume_on_startup = enabled;
    saveSettings();
}

void QmmpUiSettings::setUseClipboard(bool enabled)
{
    m_use_clipboard = enabled;
    saveSettings();
}

bool QmmpUiSettings::useClipboard() const
{
    return m_use_clipboard;
}

void QmmpUiSettings::sync()
{
    if(m_saveSettings)
    {
        qCDebug(core) << "saving settings...";
        QSettings s;
        s.setValue(u"PlayList/group_format"_s, m_group_format);
        s.setValue(u"PlayList/group_extra_row_format"_s, m_group_extra_row_format);
        s.setValue(u"PlayList/lines_per_group"_s, m_lines_per_group);
        s.setValue(u"PlayList/group_extra_row_visible"_s, m_group_extra_row_visible);
        s.setValue(u"PlayList/group_cover_visible"_s, m_group_cover_visible);
        s.setValue(u"PlayList/group_dividing_line_visible"_s, m_group_dividing_line_visible);
        s.setValue(u"PlayList/convert_underscore"_s, m_convert_underscore);
        s.setValue(u"PlayList/convert_twenty"_s, m_convert_twenty);
        s.setValue(u"PlayList/load_metadata"_s, m_use_metadata);
        s.setValue(u"PlayList/autosave"_s, m_autosave_playlist);
        s.setValue(u"PlayList/repeate_list"_s, m_repeate_list);
        s.setValue(u"PlayList/shuffle"_s, m_shuffle);
        s.setValue(u"PlayList/groups"_s, m_groups_enabled);
        s.setValue(u"PlayList/repeate_track"_s, m_repeat_track);
        s.setValue(u"PlayList/no_advance"_s, m_no_pl_advance);
        s.setValue(u"PlayList/clear_previous"_s, m_clear_prev_playlist);
        s.setValue(u"PlayList/read_metadata_for_playlist"_s, m_read_metadata_for_playlist);
        s.setValue(u"PlayList/transit_between_playlists"_s, m_transit_between_playlists);
        s.setValue(u"PlayList/skip_existing_tracks"_s, m_skip_existing_tracks);
        s.setValue(u"PlayList/stop_after_removing_of_current"_s, m_stop_after_removing_of_current);
        s.setValue(u"General/resume_on_startup"_s, m_resume_on_startup);
        s.setValue(u"General/restrict_filters"_s, m_restrict_filters);
        s.setValue(u"General/exclude_filters"_s, m_exclude_filters);
        s.setValue(u"General/use_default_pl"_s, m_use_default_pl);
        s.setValue(u"General/default_pl_name"_s, m_default_pl_name);
        s.setValue(u"URLDialog/use_clipboard"_s, m_use_clipboard);
        m_saveSettings = false; //protect from multiple calls
    }

    if(m_rebuildGroups)
    {
        qCDebug(core) << "rebuilding groups...";
        PlayListManager::instance()->rebuildGroups();
        m_rebuildGroups = false; //protect from multiple calls
    }
}

void QmmpUiSettings::saveSettings(bool rebuildGroups)
{
    m_saveSettings = true;

    if(rebuildGroups)
        m_rebuildGroups = true;

    QMetaObject::invokeMethod(this, &QmmpUiSettings::sync, Qt::QueuedConnection);
}

void QmmpUiSettings::setRepeatableList(bool r)
{
    if(m_repeate_list != r)
    {
        m_repeate_list = r;
        saveSettings();
        emit repeatableListChanged(r);
    }
}

void QmmpUiSettings::setShuffle(bool s)
{
    if(m_shuffle != s)
    {
        m_shuffle = s;
        saveSettings();
        emit shuffleChanged(s);
    }
}

void QmmpUiSettings::setGroupsEnabled(bool enabled)
{
    if(m_groups_enabled != enabled)
    {
        m_groups_enabled = enabled;
        saveSettings();
        emit groupsChanged(enabled);
    }
}

void QmmpUiSettings::setRepeatableTrack(bool enabled)
{
    if(m_repeat_track != enabled)
    {
        m_repeat_track = enabled;
        saveSettings();
        emit repeatableTrackChanged(enabled);
    }
}

void QmmpUiSettings::setNoPlayListAdvance(bool enabled)
{
    if(m_no_pl_advance != enabled)
    {
        m_no_pl_advance = enabled;
        saveSettings();
        emit noPlayListAdvanceChanged(enabled);
    }
}

void QmmpUiSettings::setPlayListTransitionEnabled(bool enabled)
{
    if(m_transit_between_playlists != enabled)
    {
        m_transit_between_playlists = enabled;
        saveSettings();
        emit playListTransitionChanged(enabled);
    }
}

const QStringList &QmmpUiSettings::restrictFilters() const
{
    return m_restrict_filters;
}

void QmmpUiSettings::setRestrictFilters(const QString &filters)
{
    m_restrict_filters = filters.trimmed().split(QLatin1Char(','), Qt::SkipEmptyParts);
    saveSettings();
}

const QStringList &QmmpUiSettings::excludeFilters() const
{
    return m_exclude_filters;
}

void QmmpUiSettings::setExcludeFilters(const QString &filters)
{
    m_exclude_filters = filters.trimmed().split(QLatin1Char(','), Qt::SkipEmptyParts);
    saveSettings();
}

bool QmmpUiSettings::useDefaultPlayList() const
{
    return m_use_default_pl;
}

const QString &QmmpUiSettings::defaultPlayListName() const
{
    return m_default_pl_name;
}

QmmpUiSettings *QmmpUiSettings::instance()
{
    if(!m_instance)
        return new QmmpUiSettings(qApp);
    return m_instance;
}

void QmmpUiSettings::setDefaultPlayList(const QString &name, bool enabled)
{
    m_use_default_pl = enabled;
    m_default_pl_name = name;
    saveSettings();
}

void QmmpUiSettings::setAutoSavePlayList(bool enabled)
{
    m_autosave_playlist = enabled;
    saveSettings();
}

bool QmmpUiSettings::autoSavePlayList() const
{
    return m_autosave_playlist;
}

void QmmpUiSettings::setClearPreviousPlayList(bool enabled)
{
    m_clear_prev_playlist = enabled;
    saveSettings();
}

bool QmmpUiSettings::clearPreviousPlayList() const
{
    return m_clear_prev_playlist;
}

void QmmpUiSettings::setSkipExistingTracks(bool enabled)
{
    m_skip_existing_tracks = enabled;
    saveSettings();
}

bool QmmpUiSettings::skipExistingTracks() const
{
    return m_skip_existing_tracks;
}

bool QmmpUiSettings::stopAfterRemovingOfCurrentTrack() const
{
    return m_stop_after_removing_of_current;
}

void QmmpUiSettings::setStopAfterRemovingOfCurrentTrack(bool enabled)
{
    m_stop_after_removing_of_current = enabled;
    saveSettings();
}

bool QmmpUiSettings::readMetaDataForPlayLists() const
{
    return m_read_metadata_for_playlist;
}

void QmmpUiSettings::setReadMetaDataForPlayLists(bool enabled)
{
    m_read_metadata_for_playlist = enabled;
    saveSettings();
}
