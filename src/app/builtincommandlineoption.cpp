/***************************************************************************
 *   Copyright (C) 2008-2024 by Ilya Kotov                                 *
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
#include <QFileInfo>
#include <qmmp/soundcore.h>
#include <qmmpui/mediaplayer.h>
#include <qmmpui/uihelper.h>
#include <qmmpui/filedialog.h>
#include <qmmpui/qmmpuisettings.h>
#include <qmmpui/playlistdownloader.h>
#include <qmmpui/playlistparser.h>
#include <qmmp/metadatamanager.h>
#include "builtincommandlineoption.h"

// BuiltinCommandLineOption methods implementation
BuiltinCommandLineOption::BuiltinCommandLineOption(QObject *parent) : QObject(parent)
{
#ifdef Q_OS_WIN
    m_timer = new QTimer(this);
    m_timer->setSingleShot(true);
    m_timer->setInterval(500);
    connect(m_timer, SIGNAL(timeout()), SLOT(addPendingPaths()));
#endif
}

void BuiltinCommandLineOption::registerOprions()
{
    registerOption(ENQUEUE, { "-e", "--enqueue" }, tr("Don't clear the playlist"));
    registerOption(PLAY, { "-p", "--play" }, tr("Start playing current song"));
    registerOption(PAUSE, { "-u", "--pause"}, tr("Pause current song"));
    registerOption(PLAY_PAUSE, { "-t", "--play-pause" }, tr("Pause if playing, play otherwise"));
    registerOption(STOP, { "-s", "--stop" }, tr("Stop current song"));
    registerOption(JUMP_TO_TRACK, { "-j", "--jump-to-track" }, tr("Display Jump to Track dialog"));
    registerOption(QUIT, { "-q", "--quit" }, tr("Quit application"));
    registerOption(VOLUME, "--volume", tr("Set playback volume (example: qmmp --volume 20)"), { "0..100" });
    registerOption(VOLUME_STATUS, "--volume-status", tr("Print volume level"));
    registerOption(TOGGLE_MUTE, "--toggle-mute", tr("Mute/Restore volume"));
    registerOption(MUTE_STATUS, "--mute-status", tr("Print mute status"));
    registerOption(NEXT, "--next", tr("Skip forward in playlist"));
    registerOption(PREVIOUS, "--previous", tr("Skip backwards in playlist"));
    registerOption(TOGGLE_VISIBILITY, "--toggle-visibility", tr("Show/hide application"));
    registerOption(SHOW_MW, "--show-mw", tr("Show main window"));
    registerOption(ADD_FILE, "--add-file", tr("Display Add File dialog"));
    registerOption(ADD_DIRECTORY, "--add-dir", tr("Display Add Directory dialog"));
}

QString BuiltinCommandLineOption::shortName() const
{
    return u"BuiltinOptions"_s;
}

QString BuiltinCommandLineOption::translation() const
{
    return QString();
}

QString BuiltinCommandLineOption::executeCommand(int id, const QStringList &args, const QString &cwd)
{
    SoundCore *core = SoundCore::instance();
    MediaPlayer *player = MediaPlayer::instance();
    PlayListManager *pl_manager = PlayListManager::instance();
    QmmpUiSettings *settings = QmmpUiSettings::instance();
    QString out;
    if(!core || !player)
        return out;

    switch(id) {
    case OPEN:
    case ENQUEUE:
    {
        if(args.isEmpty())
            return out;
        QStringList full_path_list, remote_pls_list;
        for(QString s : qAsConst(args))
        {
#ifdef Q_OS_WIN
            s.replace("\\","/");
#endif
            if(QFileInfo(s).isAbsolute()) //absolute path
                full_path_list << s;
            else if(s.contains("://")) //url
            {
                if(PlayListParser::findByUrl(s)) //remote playlist
                    remote_pls_list << s;
                else
                    full_path_list << s; //url
            }
            else //relative path
                full_path_list << cwd + "/" + s;
        }
        //default playlist
        if(settings->useDefaultPlayList())
        {
            if(!pl_manager->playListNames().contains(settings->defaultPlayListName()))
                pl_manager->createPlayList(settings->defaultPlayListName());
            pl_manager->selectPlayList(settings->defaultPlayListName());
        }
        pl_manager->activatePlayList(pl_manager->selectedPlayList());
        m_model = pl_manager->selectedPlayList();

        if(id == OPEN)
        {
            m_model->clear(); //clear playlist if option is empty
            m_pending_path_list << full_path_list;
#ifdef Q_OS_WIN
            //windows starts instance for each selected file,
            //so we should wait paths from all started qmmp instances
            m_timer->start();
#else
            addPendingPaths();
#endif
        }
        else
            m_model->add(full_path_list);
        if(!remote_pls_list.isEmpty())
        {
            PlayListDownloader *downloader = new PlayListDownloader(this);
            connect(downloader, SIGNAL(finished(bool,QString)), downloader, SLOT(deleteLater()));
            downloader->start(remote_pls_list.at(0), m_model);
        }
        break;
    }
    case PLAY:
        player->play();
        break;
    case STOP:
        core->stop();
        break;
    case PAUSE:
        if(core->state() == Qmmp::Playing)
            core->pause();
        break;
    case NEXT:
        player->next();
        if (core->state() == Qmmp::Stopped)
            player->play();
        break;
    case PREVIOUS:
        player->previous();
        if (core->state() == Qmmp::Stopped)
            player->play();
        break;
    case PLAY_PAUSE:
        if (core->state() == Qmmp::Playing)
            core->pause();
        else
            player->play();
        break;
    case JUMP_TO_TRACK:
        UiHelper::instance()->jumpToTrack();
        break;
    case QUIT:
        qApp->closeAllWindows();
        qApp->quit();
        break;
    case TOGGLE_VISIBILITY:
        UiHelper::instance()->toggleVisibility();
        break;
    case SHOW_MW:
        UiHelper::instance()->showMainWindow();
        break;
    case ADD_FILE:
        UiHelper::instance()->addFiles();
        break;
    case ADD_DIRECTORY:
        UiHelper::instance()->addDirectory();
        break;
    case VOLUME:
        if(!args.isEmpty())
        {
            bool ok = false;
            int volume = args.at(0).toInt(&ok);
            if (ok)
                core->setVolume(volume);
        }
        break;
    case VOLUME_STATUS:
        out += QStringLiteral("%1\n").arg(core->volume());
        break;
    case TOGGLE_MUTE:
        core->setMuted(!core->isMuted());
        break;
    case MUTE_STATUS:
        out += QStringLiteral("%1\n").arg(core->isMuted());
        break;
    }
    return out;
}

QHash<QString, QStringList> BuiltinCommandLineOption::splitArgs(const QStringList &args) const
{
    QHash <QString, QStringList> commands;
    QString lastCmd;
    for(const QString &arg : qAsConst(args))
    {
        QString cmd = arg.trimmed();
        if(cmd.startsWith("-") || cmd.startsWith("--"))
        {
            commands.insert(cmd, QStringList());
            lastCmd = cmd;
        }
        else if(!commands.isEmpty() && !lastCmd.isEmpty())
            commands[lastCmd] << arg;
    }
    return commands;
}

void BuiltinCommandLineOption::disconnectPl()
{
    if(m_model)
    {
        disconnect(m_model, SIGNAL(trackAdded(PlayListTrack*)), MediaPlayer::instance(), SLOT(play()));
        disconnect(m_model, SIGNAL(trackAdded(PlayListTrack*)), this, SLOT(disconnectPl()));
        disconnect(m_model, SIGNAL(loaderFinished()), this, SLOT(disconnectPl()));
        m_model = nullptr;
    }
}

void BuiltinCommandLineOption::addPendingPaths()
{
    if(m_pending_path_list.isEmpty())
        return;

    SoundCore *core = SoundCore::instance();
    MediaPlayer *player = MediaPlayer::instance();
    PlayListManager *pl_manager = PlayListManager::instance();

    if(core->state() != Qmmp::Stopped)
    {
        core->stop();
        qApp->processEvents(); //receive stop signal
    }
    m_model = pl_manager->selectedPlayList();
    m_model->clear();

    connect(m_model, SIGNAL(trackAdded(PlayListTrack*)), player, SLOT(play()));
    connect(m_model, SIGNAL(trackAdded(PlayListTrack*)), SLOT(disconnectPl()));
    connect(m_model, SIGNAL(loaderFinished()), SLOT(disconnectPl()));

    m_model->add(m_pending_path_list);
    m_pending_path_list.clear();
}
