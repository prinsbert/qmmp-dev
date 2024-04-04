/***************************************************************************
 *   Copyright (C) 2012-2024 by Ilya Kotov                                 *
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
#include <QTimer>
#include <qmmp/qmmp.h>
#include "metadatahelper_p.h"
#include "playlistmanager.h"
#include "qmmpuisettings.h"

QmmpUiSettings *QmmpUiSettings::m_instance = nullptr;

QmmpUiSettings::QmmpUiSettings(QObject *parent) : QObject(parent)
{
    if(m_instance)
        qFatal("QmmpUiSettings: only one instance is allowed");
    m_instance = this;
    m_helper = new MetaDataHelper;
    QSettings s;
    s.beginGroup("PlayList");
    m_group_format = s.value("group_format", "%p%if(%p&%a, - %if(%y,[%y] ,),)%a").toString();
    m_group_extra_row_format = s.value("group_extra_row_format", "%y | %l | %{bitrate} kb/s").toString();
    m_lines_per_group = s.value("lines_per_group", 1).toInt();
    m_group_extra_row_visible = s.value("group_extra_row_visible", true).toBool();
    m_group_cover_visible = s.value("group_cover_visible", true).toBool();
    m_group_dividing_line_visible = s.value("group_dividing_line_visible", true).toBool();
    m_convert_underscore = s.value ("convert_underscore", true).toBool();
    m_convert_twenty = s.value ("convert_twenty", true).toBool();
    m_use_metadata = s.value ("load_metadata", true).toBool();
    m_autosave_playlist = s.value("autosave", true).toBool();
    m_repeate_list = s.value("repeate_list", false).toBool();
    m_shuffle = s.value("shuffle", false).toBool();
    m_groups_enabled = s.value("groups", false).toBool();
    m_repeat_track = s.value("repeate_track", false).toBool();
    m_no_pl_advance = s.value("no_advance", false).toBool();
    m_clear_prev_playlist = s.value("clear_previous", false).toBool();
    m_read_metadata_for_playlist = s.value("read_metadata_for_playlist", true).toBool();
    m_transit_between_playlists = s.value("transit_between_playlists", false).toBool();
    m_skip_existing_tracks = s.value("skip_existing_tracks", false).toBool();
    m_stop_after_removing_of_current = s.value("stop_after_removing_of_current", false).toBool();
    s.endGroup();
    s.beginGroup("General");
    m_resume_on_startup = s.value("resume_on_startup", false).toBool();
    m_restrict_filters = s.value("restrict_filters").toStringList();
    m_exclude_filters = s.value("exclude_filters").toStringList();
    m_use_default_pl = s.value("use_default_pl", false).toBool();
    m_default_pl_name = s.value("default_pl_name", tr("Playlist")).toString();
    s.endGroup();
    m_use_clipboard = s.value("URLDialog/use_clipboard", false).toBool();
    m_timer = new QTimer(this);
    m_timer->setInterval(5000);
    m_timer->setSingleShot(true);
    connect(m_timer, SIGNAL(timeout()), SLOT(sync()));

    m_helper->setGroupFormat(m_group_format);
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
    m_timer->start();
}

void  QmmpUiSettings::setConvertTwenty(bool yes)
{
    m_convert_twenty = yes;
    m_timer->start();
}

void QmmpUiSettings::setGroupFormat(const QString &groupFormat)
{
    if(groupFormat != m_group_format)
    {
        m_group_format = groupFormat;
        m_helper->setGroupFormat(m_group_format);
        for(PlayListModel *model : PlayListManager::instance()->playLists())
        {
            model->rebuildGroups();
        }
        m_timer->start();
    }
}

void QmmpUiSettings::setGroupExtraRowFormat(const QString &extraRowFormat)
{
    m_group_extra_row_format = extraRowFormat;
    m_timer->start();
}

void QmmpUiSettings::setLinesPerGroup(int lines)
{
    m_lines_per_group = lines;
    m_timer->start();
}

void QmmpUiSettings::setGroupExtraRowVisible(bool enabled)
{
    m_group_extra_row_visible = enabled;
    m_timer->start();
}

void QmmpUiSettings::setGroupCoverVisible(bool enabled)
{
    m_group_cover_visible = enabled;
    m_timer->start();
}

void QmmpUiSettings::setGroupDividingLineVisible(bool enabled)
{
    m_group_dividing_line_visible = enabled;
    m_timer->start();
}

void QmmpUiSettings::setUseMetaData(bool yes)
{
    m_use_metadata = yes;
    m_timer->start();
}

bool QmmpUiSettings::resumeOnStartup() const
{
    return m_resume_on_startup;
}

void QmmpUiSettings::setResumeOnStartup(bool enabled)
{
    m_resume_on_startup = enabled;
    m_timer->start();
}

void QmmpUiSettings::setUseClipboard(bool enabled)
{
    m_use_clipboard = enabled;
    m_timer->start();
}

bool QmmpUiSettings::useClipboard() const
{
    return m_use_clipboard;
}

void QmmpUiSettings::sync()
{
    qDebug("%s", Q_FUNC_INFO);
    QSettings s;
    s.setValue("PlayList/group_format", m_group_format);
    s.setValue("PlayList/group_extra_row_format", m_group_extra_row_format);
    s.setValue("PlayList/lines_per_group", m_lines_per_group);
    s.setValue("PlayList/group_extra_row_visible", m_group_extra_row_visible);
    s.setValue("PlayList/group_cover_visible", m_group_cover_visible);
    s.setValue("PlayList/group_dividing_line_visible", m_group_dividing_line_visible);
    s.setValue("PlayList/convert_underscore", m_convert_underscore);
    s.setValue("PlayList/convert_twenty", m_convert_twenty);
    s.setValue("PlayList/load_metadata", m_use_metadata);
    s.setValue("PlayList/autosave", m_autosave_playlist);
    s.setValue("PlayList/repeate_list", m_repeate_list);
    s.setValue("PlayList/shuffle", m_shuffle);
    s.setValue("PlayList/groups", m_groups_enabled);
    s.setValue("PlayList/repeate_track", m_repeat_track);
    s.setValue("PlayList/no_advance", m_no_pl_advance);
    s.setValue("PlayList/clear_previous", m_clear_prev_playlist);
    s.setValue("PlayList/read_metadata_for_playlist", m_read_metadata_for_playlist);
    s.setValue("PlayList/transit_between_playlists", m_transit_between_playlists);
    s.setValue("PlayList/skip_existing_tracks", m_skip_existing_tracks);
    s.setValue("PlayList/stop_after_removing_of_current", m_stop_after_removing_of_current);
    s.setValue("General/resume_on_startup", m_resume_on_startup);
    s.setValue("General/restrict_filters", m_restrict_filters);
    s.setValue("General/exclude_filters", m_exclude_filters);
    s.setValue("General/use_default_pl", m_use_default_pl);
    s.setValue("General/default_pl_name", m_default_pl_name);
    s.setValue("URLDialog/use_clipboard", m_use_clipboard);
}

void QmmpUiSettings::setRepeatableList(bool r)
{
    if(m_repeate_list != r)
    {
        m_repeate_list = r;
        m_timer->start();
        emit repeatableListChanged(r);
    }
}

void QmmpUiSettings::setShuffle(bool s)
{
    if(m_shuffle != s)
    {
        m_shuffle = s;
        m_timer->start();
        emit shuffleChanged(s);
    }
}

void QmmpUiSettings::setGroupsEnabled(bool enabled)
{
    if(m_groups_enabled != enabled)
    {
        m_groups_enabled = enabled;
        m_timer->start();
        emit groupsChanged(enabled);
    }
}

void QmmpUiSettings::setRepeatableTrack(bool enabled)
{
    if(m_repeat_track != enabled)
    {
        m_repeat_track = enabled;
        m_timer->start();
        emit repeatableTrackChanged(enabled);
    }
}

void QmmpUiSettings::setNoPlayListAdvance(bool enabled)
{
    if(m_no_pl_advance != enabled)
    {
        m_no_pl_advance = enabled;
        m_timer->start();
        emit noPlayListAdvanceChanged(enabled);
    }
}

void QmmpUiSettings::setPlayListTransitionEnabled(bool enabled)
{
    if(m_transit_between_playlists != enabled)
    {
        m_transit_between_playlists = enabled;
        m_timer->start();
        emit playListTransitionChanged(enabled);
    }
}

const QStringList &QmmpUiSettings::restrictFilters() const
{
    return m_restrict_filters;
}

void QmmpUiSettings::setRestrictFilters(const QString &filters)
{
    m_restrict_filters = filters.trimmed().split(",", Qt::SkipEmptyParts);
    m_timer->start();
}

const QStringList &QmmpUiSettings::excludeFilters() const
{
    return m_exclude_filters;
}

void QmmpUiSettings::setExcludeFilters(const QString &filters)
{
    m_exclude_filters = filters.trimmed().split(",", Qt::SkipEmptyParts);
    m_timer->start();
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
    m_timer->start();
}

void QmmpUiSettings::setAutoSavePlayList(bool enabled)
{
    m_autosave_playlist = enabled;
    m_timer->start();
}

bool QmmpUiSettings::autoSavePlayList() const
{
    return m_autosave_playlist;
}

void QmmpUiSettings::setClearPreviousPlayList(bool enabled)
{
    m_clear_prev_playlist = enabled;
    m_timer->start();
}

bool QmmpUiSettings::clearPreviousPlayList() const
{
    return m_clear_prev_playlist;
}

void QmmpUiSettings::setSkipExistingTracks(bool enabled)
{
    m_skip_existing_tracks = enabled;
    m_timer->start();
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
    m_timer->start();
}

bool QmmpUiSettings::readMetaDataForPlayLists() const
{
    return m_read_metadata_for_playlist;
}

void QmmpUiSettings::setReadMetaDataForPlayLists(bool enabled)
{
    m_read_metadata_for_playlist = enabled;
    m_timer->start();
}
