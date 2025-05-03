/***************************************************************************
 *   Copyright (C) 2011-2025 by Ilya Kotov                                 *
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

#include <QtPlugin>
#include <QLocale>
#include <qmmp/soundcore.h>
#include <qmmpui/playlistmanager.h>
#include <qmmpui/metadataformatter.h>
#include <qmmpui/mediaplayer.h>
#include <qmmpui/qmmpuisettings.h>
#include <qmmpui/commandlinemanager.h>
#include "playlistoption.h"

void PlayListOption::registerOprions()
{
    registerOption(PL_HELP,  u"--pl-help"_s, tr("Show playlist manipulation commands"));
    registerOption(PL_LIST,  u"--pl-list"_s, tr("List all available playlists"));
    registerOption(PL_DUMP,  u"--pl-dump"_s, tr("Show playlist content"), QStringList{ u"id"_s });
    registerOption(PL_PLAY,  u"--pl-play"_s, tr("Play track <track> in playlist <id>"), QStringList{ u"id"_s, u"track"_s});
    registerOption(PL_CLEAR, u"--pl-clear"_s, tr("Clear playlist"), { u"id"_s });
    registerOption(PL_NEXT,  u"--pl-next"_s, tr("Activate next playlist"));
    registerOption(PL_PREV,  u"--pl-prev"_s, tr("Activate previous playlist"));
    registerOption(PL_REPEATE_TOGGLE, u"--pl-repeat-toggle"_s, tr("Toggle playlist repeat"));
    registerOption(PL_SHUFFLE_TOGGLE, u"--pl-shuffle-toggle"_s, tr("Toggle playlist shuffle"));
    registerOption(PL_STATE, u"--pl-state"_s, tr("Show playlist options"));

    setOptionFlags(PL_HELP, NoStart);
    setOptionFlags(PL_LIST, HiddenFromHelp);
    setOptionFlags(PL_DUMP, HiddenFromHelp);
    setOptionFlags(PL_PLAY, HiddenFromHelp);
    setOptionFlags(PL_CLEAR, HiddenFromHelp);
    setOptionFlags(PL_NEXT, HiddenFromHelp);
    setOptionFlags(PL_PREV, HiddenFromHelp);
    setOptionFlags(PL_REPEATE_TOGGLE, HiddenFromHelp);
    setOptionFlags(PL_SHUFFLE_TOGGLE, HiddenFromHelp);
    setOptionFlags(PL_STATE, HiddenFromHelp);
}

QString PlayListOption::shortName() const
{
    return QLatin1String("PlayListOption");
}

QString PlayListOption::translation() const
{
    return QLatin1String(":/playlist_plugin_");
}

QString PlayListOption::executeCommand(int id, const QStringList &args, const QString &cwd)
{
    Q_UNUSED(cwd);
    QString out;
    PlayListManager *pl_manager = PlayListManager::instance();
    MediaPlayer *player = MediaPlayer::instance();
    QmmpUiSettings *ui_settings = (id == PL_HELP) ? nullptr : QmmpUiSettings::instance();

    switch (id)
    {
    case PL_HELP:
    {
        const QStringList list = {
            helpString(PL_LIST),
            helpString(PL_DUMP),
            helpString(PL_PLAY),
            helpString(PL_CLEAR),
            helpString(PL_NEXT),
            helpString(PL_PREV),
            helpString(PL_REPEATE_TOGGLE),
            helpString(PL_SHUFFLE_TOGGLE),
            helpString(PL_STATE)
        };

        for(const QString &line : std::as_const(list))
            out += CommandLineManager::formatHelpString(line) + QChar::LineFeed;
    }
        break;
    case PL_LIST:
    {
        QStringList names = pl_manager->playListNames();
        for(int i = 0; i <  names.count(); ++i)
        {
            if(i == pl_manager->currentPlayListIndex())
                out += QStringLiteral("%1. %2 [*]\n").arg(i + 1).arg(names.at(i));
            else
                out += QStringLiteral("%1. %2\n").arg(i + 1).arg(names.at(i));
        }
    }
        break;
    case PL_DUMP:
    {
        MetaDataFormatter formatter(u"%p%if(%p&%t, - ,)%t%if(%p,,%if(%t,,%f))%if(%l, - %l,)"_s);
        int pl_id = args.isEmpty() ? pl_manager->currentPlayListIndex() : args.constFirst().toInt() - 1;
        PlayListModel *model = pl_manager->playListAt(pl_id);
        if(!model)
            return tr("Invalid playlist ID");
        for(int i = 0; i < model->trackCount(); ++i)
        {
            PlayListTrack *track = model->track(i);
            out += QStringLiteral("%1. %2").arg(track->trackIndex() + 1).arg(formatter.format(track));
            if(i == model->currentIndex())
                out += u" [*]"_s;

            if(i != model->trackCount() - 1)
                out += QChar::LineFeed;
        }
    }
        break;
    case PL_PLAY:
    {
        if(args.count() > 2 || args.isEmpty())
            return tr("Invalid number of arguments");

        int pl_id = (args.count() == 1) ? pl_manager->currentPlayListIndex() : args.constFirst().toInt() - 1;
        int track_number = (args.count() == 1) ? args.constFirst().toInt() - 1 : args.at(1).toInt() - 1;
        PlayListModel *model = pl_manager->playListAt(pl_id);
        if(!model)
            return tr("Invalid playlist ID");

        PlayListTrack *track = model->findTrack(track_number);
        if(!track)
            return tr("Invalid track ID");
        player->stop();
        pl_manager->activatePlayList(model);
        pl_manager->selectPlayList(model);
        model->setCurrent(track);
        player->play();
    }
        break;
    case PL_NEXT:
    {
        bool playing = SoundCore::instance()->state() == Qmmp::Playing;
        player->stop();
        pl_manager->selectPlayList(pl_manager->currentPlayList());
        pl_manager->selectNextPlayList();
        pl_manager->activateSelectedPlayList();
        if(playing)
            player->play();
    }
        break;
    case PL_PREV:
    {
        bool playing = SoundCore::instance()->state() == Qmmp::Playing;
        player->stop();
        pl_manager->selectPlayList(pl_manager->currentPlayList());
        pl_manager->selectPreviousPlayList();
        pl_manager->activateSelectedPlayList();
        if(playing)
            player->play();
    }
        break;
    case PL_CLEAR:
    {
        int pl_id= args.isEmpty() ? pl_manager->currentPlayListIndex() : args.constFirst().toInt() - 1;
        PlayListModel *model = pl_manager->playListAt(pl_id);
        if(!model)
            return tr("Invalid playlist ID");
        model->clear();
    }
        break;
    case PL_REPEATE_TOGGLE:
        ui_settings->setRepeatableList(!ui_settings->isRepeatableList());
        break;
    case PL_SHUFFLE_TOGGLE:
        ui_settings->setShuffle(!ui_settings->isShuffle());
        break;
    case PL_STATE:
        out += u"SHUFFLE:             "_s + boolToText(ui_settings->isShuffle()) + QChar::LineFeed;
        out += u"REPEAT PLAYLIST:     "_s + boolToText(ui_settings->isRepeatableList()) + QChar::LineFeed;
        out += u"REPEAT TRACK:        "_s + boolToText(ui_settings->isRepeatableTrack()) + QChar::LineFeed;
        out += u"NO PLAYLIST ADVANCE: "_s + boolToText(ui_settings->isNoPlayListAdvance());
        break;
    default:
        ;
    }
    return out;
}

QString PlayListOption::boolToText(bool enabled)
{
    return enabled ? u"[+]"_s : u"[-]"_s;
}
