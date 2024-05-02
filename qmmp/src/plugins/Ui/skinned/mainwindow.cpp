/***************************************************************************
 *   Copyright (C) 2006-2023 by Ilya Kotov                                 *
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

#include <QFileDialog>
#include <QDir>
#include <QAction>
#include <QMenu>
#include <QScreen>
#include <algorithm>
#include <math.h>
#include <qmmp/soundcore.h>
#include <qmmp/visual.h>
#include <qmmp/metadatamanager.h>
#include <qmmpui/uihelper.h>
#include <qmmpui/general.h>
#include <qmmpui/playlistparser.h>
#include <qmmpui/playlistformat.h>
#include <qmmpui/commandlinemanager.h>
#include <qmmpui/filedialog.h>
#include <qmmpui/playlistmodel.h>
#include <qmmpui/playlistmanager.h>
#include <qmmpui/mediaplayer.h>
#include <qmmpui/configdialog.h>
#include <qmmpui/qmmpuisettings.h>
#include <qmmpui/visualmenu.h>
#include "hotkeyeditor.h"
#include "skinnedsettings.h"
#include "mainwindow.h"
#include "skin.h"
#include "playlist.h"
#include "dock.h"
#include "eqwidget.h"
#include "mainvisual.h"
#include "listwidget.h"
#include "windowsystem.h"
#include "skinnedactionmanager.h"

#define KEY_OFFSET 10000

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent)
{
#ifdef QMMP_WS_X11
    qDebug("MainWindow: detected wm: %s", qPrintable(WindowSystem::netWindowManagerName()));
    QString wm_name = WindowSystem::netWindowManagerName();
    if(wm_name.contains(u"Marco"_s, Qt::CaseInsensitive) ||
            wm_name.contains(u"Metacity"_s, Qt::CaseInsensitive) ||
            wm_name.contains(u"Mutter"_s, Qt::CaseInsensitive) ||
            wm_name.contains(u"GNOME"_s, Qt::CaseInsensitive))
    {
        setWindowFlags(Qt::Window | Qt::FramelessWindowHint |
                       Qt::WindowCloseButtonHint | Qt::WindowSystemMenuHint);
    }
    else
#endif
        setWindowFlags(Qt::Window | Qt::FramelessWindowHint |
                       Qt::WindowCloseButtonHint | Qt::WindowMinMaxButtonsHint | Qt::WindowSystemMenuHint);

    restoreWindowTitle();

    m_titleFormatter.setPattern(u"%if(%p,%p - %t,%t)"_s);

    new SkinnedActionManager(this);

    m_player = MediaPlayer::instance();
    m_core = SoundCore::instance();
    m_pl_manager = PlayListManager::instance();
    m_uiHelper = UiHelper::instance();
    m_ui_settings = QmmpUiSettings::instance();

    //user interface
    m_skin = new Skin(this);
    setFixedSize(275 * m_skin->ratio(),116 * m_skin->ratio());

    Dock *dock = new Dock(this);
    dock->setMainWidget(this);
    m_display = new SkinnedDisplay(this);
    setCentralWidget(m_display);
    m_display->setFocus ();

    m_playlist = new PlayList(m_pl_manager, this);
    dock->addWidget(m_playlist);

    m_equalizer = new EqWidget(this);
    dock->addWidget(m_equalizer);

    createActions();
    //prepare visualization
    Visual::initialize(this, m_visMenu, SLOT(updateActions()));
    m_vis = MainVisual::instance();
    Visual::add(m_vis);
    //connections
    connect (m_player,SIGNAL(playbackFinished()), SLOT(restoreWindowTitle()));
    connect (m_playlist,SIGNAL(next()),SLOT(next()));
    connect (m_playlist,SIGNAL(prev()),SLOT(previous()));
    connect (m_playlist,SIGNAL(play()),SLOT(play()));
    connect (m_playlist,SIGNAL(pause()), m_core ,SLOT(pause()));
    connect (m_playlist,SIGNAL(stop()),SLOT(stop()));
    connect (m_playlist,SIGNAL(eject()),SLOT(playFiles()));
    connect (m_playlist,SIGNAL(loadPlaylist()),SLOT(loadPlaylist()));
    connect (m_playlist,SIGNAL(savePlaylist()),SLOT(savePlaylist()));

    connect(m_display,SIGNAL(shuffleToggled(bool)),m_ui_settings,SLOT(setShuffle(bool)));
    connect(m_display,SIGNAL(repeatableToggled(bool)),m_ui_settings,SLOT(setRepeatableList(bool)));

    connect(m_core, SIGNAL(stateChanged(Qmmp::State)), SLOT(showState(Qmmp::State)));
    connect(m_core, SIGNAL(elapsedChanged(qint64)),m_playlist, SLOT(setTime(qint64)));
    connect(m_core, SIGNAL(trackInfoChanged()),SLOT(showMetaData()));
    connect(m_uiHelper, SIGNAL(toggleVisibilityCalled()), SLOT(toggleVisibility()));
    connect(m_uiHelper, SIGNAL(showMainWindowCalled()), SLOT(showAndRaise()));

    readSettings();
    m_display->setEQ(m_equalizer);
    m_display->setPL(m_playlist);
    dock->updateDock();
    m_pl_manager->currentPlayList()->doCurrentVisibleRequest();
    if (m_startHidden && m_uiHelper->visibilityControl())
        toggleVisibility();
}

MainWindow::~MainWindow()
{}

void MainWindow::play()
{
    m_player->play();
}

void MainWindow::replay()
{
    stop();
    m_pl_manager->activatePlayList(m_pl_manager->selectedPlayList());
    play();
}

void MainWindow::forward()
{
    m_core->seek(m_core->elapsed() + KEY_OFFSET);
}

void MainWindow::backward()
{
    m_core->seek(qMax(qint64(0), m_core->elapsed() - KEY_OFFSET));
}

void MainWindow::pause(void)
{
    m_core->pause();
}

void MainWindow::stop()
{
    m_player->stop();
}

void MainWindow::next()
{
    m_player->next();
}

void MainWindow::previous()
{
    m_player->previous();
}

void MainWindow::showState(Qmmp::State state)
{
    switch ((int) state)
    {
    case Qmmp::Playing:
//        if (m_pl_manager->currentPlayList()->currentTrack())
//            m_equalizer->loadPreset(m_pl_manager->currentPlayList()->currentTrack()->path().section(QLatin1Char('/'), -1));
        break;
    case Qmmp::Paused:
        break;
    case Qmmp::Stopped:
        m_playlist->setTime(-1);
        break;
    }
}

void MainWindow::showMetaData()
{
    PlayListTrack *track = m_pl_manager->currentPlayList()->currentTrack();
    if (track && track->path() == m_core->trackInfo().path())
    {
        setWindowTitle(m_titleFormatter.format(track));
    }
}

void MainWindow::closeEvent (QCloseEvent *)
{
    if (!m_hideOnClose || !m_uiHelper->visibilityControl())
        m_uiHelper->exit();
    else
    {
        m_playlist->close();
        m_equalizer->close();
    }
}

void MainWindow::hideEvent (QHideEvent *)
{
    writeSettings();
    m_playlist->writeSettings();
    m_equalizer->writeSettings();
}

void MainWindow::addDir()
{
    m_uiHelper->addDirectory(this);
}

void MainWindow::addFile()
{
    m_uiHelper->addFiles(this);
}

void MainWindow::playFiles()
{
    m_uiHelper->playFiles(this);
}

void MainWindow::changeEvent (QEvent * event)
{
    if (event->type() == QEvent::ActivationChange)
    {
        m_display->setActive(isActiveWindow());
    }
}

void MainWindow::readSettings()
{
    QSettings settings;
    settings.beginGroup("Skinned");
    m_titleFormatter.setPattern(settings.value("window_title_format", u"%if(%p,%p - %t,%t)"_s).toString());

    if (m_update)
    {
        if(ACTION(SkinnedActionManager::WM_ALLWAYS_ON_TOP)->isChecked())
        {
            setWindowFlags(windowFlags() | Qt::WindowStaysOnTopHint);
            m_playlist->setWindowFlags(m_playlist->windowFlags() | Qt::WindowStaysOnTopHint);
            m_equalizer->setWindowFlags(m_equalizer->windowFlags() | Qt::WindowStaysOnTopHint);
        }
        else
        {
            setWindowFlags(windowFlags() & ~Qt::WindowStaysOnTopHint);
            m_playlist->setWindowFlags(m_playlist->windowFlags() & ~Qt::WindowStaysOnTopHint);
            m_equalizer->setWindowFlags(m_equalizer->windowFlags() & ~Qt::WindowStaysOnTopHint);
        }
        show();
        qApp->processEvents();
        m_playlist->setVisible(m_display->isPlaylistVisible());
        qApp->processEvents();
        m_equalizer->setVisible(m_display->isEqualizerVisible());

        if(m_pl_manager->currentPlayList()->currentTrack())
            setWindowTitle(m_titleFormatter.format(m_pl_manager->currentPlayList()->currentTrack()));
    }
    else
    {
        QScreen *primaryScreen = QGuiApplication::primaryScreen();
        QRect availableGeometry = primaryScreen->availableGeometry();
        QPoint pos = settings.value("mw_pos", QPoint(100, 100)).toPoint();
        int r = m_skin->ratio();
        const QList<QScreen *> screens = QGuiApplication::screens();
        auto it = std::find_if(screens.cbegin(), screens.cend(), [pos](QScreen *screen){ return screen->availableGeometry().contains(pos); });
        if(it != screens.cend())
            availableGeometry = (*it)->availableGeometry();
        pos.setX(qBound(availableGeometry.left(), pos.x(), availableGeometry.right() - r*275));
        pos.setY(qBound(availableGeometry.top(), pos.y(), availableGeometry.bottom() - r*116));
        move(pos); //geometry
        m_startHidden = settings.value("start_hidden", false).toBool();
        if(settings.value("always_on_top", false).toBool())
        {
            ACTION(SkinnedActionManager::WM_ALLWAYS_ON_TOP)->setChecked(true);
            setWindowFlags(windowFlags() | Qt::WindowStaysOnTopHint);
            m_playlist->setWindowFlags(m_playlist->windowFlags() | Qt::WindowStaysOnTopHint);
            m_equalizer->setWindowFlags(m_equalizer->windowFlags() | Qt::WindowStaysOnTopHint);
        }
        ACTION(SkinnedActionManager::WM_STICKY)->setChecked(settings.value("show_on_all_desktops",
                                                                    false).toBool());
        show();
        qApp->processEvents();
        //visibility
        m_playlist->setVisible(settings.value("pl_visible",true).toBool());
        qApp->processEvents();
        m_equalizer->setVisible(settings.value("eq_visible",true).toBool());
        qApp->processEvents();
        // Repeat/Shuffle
        m_display->setIsRepeatable(m_ui_settings->isRepeatableList());
        m_display->setIsShuffle(m_ui_settings->isShuffle());
        ACTION(SkinnedActionManager::REPEAT_ALL)->setChecked(m_ui_settings->isRepeatableList());
        ACTION(SkinnedActionManager::SHUFFLE)->setChecked(m_ui_settings->isShuffle());
        ACTION(SkinnedActionManager::REPEAT_TRACK)->setChecked(m_ui_settings->isRepeatableTrack());
        ACTION(SkinnedActionManager::NO_PL_ADVANCE)->setChecked(m_ui_settings->isNoPlayListAdvance());
        ACTION(SkinnedActionManager::TRANSIT_BETWEEN_PLAYLISTS)->setChecked(m_ui_settings->isPlayListTransitionEnabled());
        m_update = true;
    }
#ifdef QMMP_WS_X11
    WindowSystem::changeWinSticky(winId(), ACTION(SkinnedActionManager::WM_STICKY)->isChecked());
    WindowSystem::setWinHint(winId(), "player", "Qmmp");
#endif
    //Call setWindowOpacity only if needed
    double opacity = settings.value("mw_opacity", 1.0).toDouble();
    if(opacity != windowOpacity ())
        setWindowOpacity(opacity);

    opacity = settings.value("eq_opacity", 1.0).toDouble();
    if(opacity !=  m_equalizer->windowOpacity ())
        m_equalizer->setWindowOpacity(opacity);

    opacity = settings.value("pl_opacity", 1.0).toDouble();
    if(opacity !=  m_playlist->windowOpacity ())
        m_playlist->setWindowOpacity(opacity);

    m_hideOnClose = settings.value("hide_on_close", false).toBool();
    settings.endGroup();

    Dock::instance()->addActions(m_uiHelper->actions(UiHelper::PLAYLIST_MENU));
    Dock::instance()->addActions(m_uiHelper->actions(UiHelper::TOOLS_MENU));
}

void MainWindow::writeSettings()
{
    QSettings settings;
    settings.beginGroup("Skinned");
    //geometry
    settings.setValue("mw_pos", this->pos());
    //look & feel
    settings.setValue("double_size", ACTION(SkinnedActionManager::WM_DOUBLE_SIZE)->isChecked());
    settings.setValue("always_on_top", ACTION(SkinnedActionManager::WM_ALLWAYS_ON_TOP)->isChecked());
    settings.setValue("show_on_all_desktops", ACTION(SkinnedActionManager::WM_STICKY)->isChecked());
    settings.setValue("antialiasing", ACTION(SkinnedActionManager::WM_ANTIALIASING)->isChecked());
    settings.endGroup();
}

void MainWindow::showSettings()
{
    ConfigDialog *confDialog = new ConfigDialog(this);
    SkinnedSettings *skinnedSettings = new SkinnedSettings(this);
    confDialog->addPage(tr("Appearance"), skinnedSettings, QIcon(u":/skinned/interface.png"_s));
    confDialog->addPage(tr("Shortcuts"), new HotkeyEditor(this), QIcon(u":/skinned/shortcuts.png"_s));
    confDialog->exec();
    skinnedSettings->writeSettings();
    confDialog->deleteLater();
    updateSettings();
    SkinnedActionManager::instance()->saveActions();
}

void MainWindow::toggleVisibility()
{
    if (isHidden() || isMinimized())
    {
        show();
        raise();
        activateWindow();
        m_playlist->setVisible(m_display->isPlaylistVisible());
        m_equalizer->setVisible(m_display->isEqualizerVisible());
#ifdef QMMP_WS_X11
        if(WindowSystem::netWindowManagerName() == u"Metacity"_s)
        {
            m_playlist->activateWindow();
            m_equalizer->activateWindow();
        }
#endif
        qApp->processEvents();
        setFocus ();
        if (isMinimized())
        {
            showNormal();
        }
#ifdef QMMP_WS_X11
        WindowSystem::changeWinSticky(winId(), ACTION(SkinnedActionManager::WM_STICKY)->isChecked());
        WindowSystem::setWinHint(winId(), "player", "Qmmp");
        raise();
#endif
    }
    else
    {
        if (m_playlist->isVisible())
            m_playlist->hide();
        if (m_equalizer->isVisible())
            m_equalizer->hide();
        hide();
    }
    qApp->processEvents();
}

void MainWindow::showAndRaise()
{
    if(isHidden() || isMinimized())
        toggleVisibility();
    else
    {
        activateWindow();
        raise();
    }
}

void MainWindow::createActions()
{
    SET_ACTION(SkinnedActionManager::PL_ADD_FILE, this, &MainWindow::addFile);
    SET_ACTION(SkinnedActionManager::PL_ADD_DIRECTORY, this, &MainWindow::addDir);
    SET_ACTION(SkinnedActionManager::PL_ADD_URL, this, &MainWindow::addUrl);

    m_mainMenu = new QMenu(this);
    m_mainMenu->addAction(SET_ACTION(SkinnedActionManager::PLAY, this, &MainWindow::play));
    m_mainMenu->addAction(SET_ACTION(SkinnedActionManager::PAUSE, this, &MainWindow::pause));
    m_mainMenu->addAction(SET_ACTION(SkinnedActionManager::STOP, this, &MainWindow::stop));
    m_mainMenu->addAction(SET_ACTION(SkinnedActionManager::PREVIOUS, this, &MainWindow::previous));
    m_mainMenu->addAction(SET_ACTION(SkinnedActionManager::NEXT, this, &MainWindow::next));
    m_mainMenu->addAction(SET_ACTION(SkinnedActionManager::PLAY_PAUSE, this, &MainWindow::playPause));
    m_mainMenu->addSeparator();
    m_mainMenu->addAction(SET_ACTION(SkinnedActionManager::JUMP, this, &MainWindow::jumpToTrack));
    m_mainMenu->addSeparator();
    QMenu *viewMenu = m_mainMenu->addMenu(tr("View"));
    viewMenu->addAction(ACTION(SkinnedActionManager::SHOW_PLAYLIST));
    viewMenu->addAction(ACTION(SkinnedActionManager::SHOW_EQUALIZER));
    viewMenu->addSeparator();
    viewMenu->addAction(SET_ACTION(SkinnedActionManager::WM_ALLWAYS_ON_TOP, this, &MainWindow::updateSettings));
    viewMenu->addAction(SET_ACTION(SkinnedActionManager::WM_STICKY, this, &MainWindow::updateSettings));
    viewMenu->addAction(SET_ACTION(SkinnedActionManager::WM_DOUBLE_SIZE, this, &MainWindow::updateSettings));
    viewMenu->addAction(SET_ACTION(SkinnedActionManager::WM_ANTIALIASING, this, &MainWindow::updateSettings));

    QMenu *plMenu = m_mainMenu->addMenu(tr("Playlist"));
    plMenu->addAction(SET_ACTION(SkinnedActionManager::REPEAT_ALL, m_ui_settings, &QmmpUiSettings::setRepeatableList));
    plMenu->addAction(SET_ACTION(SkinnedActionManager::REPEAT_TRACK, m_ui_settings, &QmmpUiSettings::setRepeatableTrack));
    plMenu->addAction(SET_ACTION(SkinnedActionManager::SHUFFLE, m_ui_settings, &QmmpUiSettings::setShuffle));
    plMenu->addAction(SET_ACTION(SkinnedActionManager::NO_PL_ADVANCE, m_ui_settings, &QmmpUiSettings::setNoPlayListAdvance));
    plMenu->addAction(SET_ACTION(SkinnedActionManager::TRANSIT_BETWEEN_PLAYLISTS, m_ui_settings, &QmmpUiSettings::setPlayListTransitionEnabled));
    plMenu->addAction(SET_ACTION(SkinnedActionManager::STOP_AFTER_SELECTED, m_pl_manager, &PlayListManager::stopAfterSelected));
    plMenu->addAction(SET_ACTION(SkinnedActionManager::CLEAR_QUEUE, m_pl_manager, &PlayListManager::clearQueue));
    plMenu->addSeparator();
    plMenu->addAction(ACTION(SkinnedActionManager::PL_SHOW_HEADER));
    plMenu->addAction(ACTION(SkinnedActionManager::PL_SHOW_TABBAR));
    plMenu->addAction(ACTION(SkinnedActionManager::PL_GROUP_TRACKS));

    connect(m_ui_settings, SIGNAL(repeatableListChanged(bool)),
            ACTION(SkinnedActionManager::REPEAT_ALL), SLOT(setChecked(bool)));
    connect(m_ui_settings, SIGNAL (repeatableTrackChanged(bool)),
            ACTION(SkinnedActionManager::REPEAT_TRACK), SLOT(setChecked(bool)));
    connect(m_ui_settings, SIGNAL (noPlayListAdvanceChanged(bool)),
            ACTION(SkinnedActionManager::NO_PL_ADVANCE), SLOT(setChecked(bool)));
    connect(m_ui_settings, SIGNAL(shuffleChanged(bool)),
            ACTION(SkinnedActionManager::SHUFFLE), SLOT(setChecked(bool)));
    connect(m_ui_settings, SIGNAL(playListTransitionChanged(bool)),
            ACTION(SkinnedActionManager::TRANSIT_BETWEEN_PLAYLISTS), SLOT(setChecked(bool)));

    QMenu *audioMenu = m_mainMenu->addMenu(tr("Audio"));
    audioMenu->addAction(SET_ACTION(SkinnedActionManager::VOL_ENC, m_core, &SoundCore::volumeUp));
    audioMenu->addAction(SET_ACTION(SkinnedActionManager::VOL_DEC, m_core, &SoundCore::volumeDown));
    audioMenu->addAction(SET_ACTION(SkinnedActionManager::VOL_MUTE, m_core, &SoundCore::setMuted));
    connect(m_core, SIGNAL(mutedChanged(bool)), ACTION(SkinnedActionManager::VOL_MUTE), SLOT(setChecked(bool)));

    m_visMenu = new VisualMenu(this);
    m_mainMenu->addMenu(m_visMenu);
    m_mainMenu->addMenu(m_uiHelper->createMenu(UiHelper::TOOLS_MENU, tr("Tools"), true, this));

    m_mainMenu->addSeparator();
    m_mainMenu->addAction(SET_ACTION(SkinnedActionManager::SETTINGS, this, &MainWindow::showSettings));
    m_mainMenu->addSeparator();
    m_mainMenu->addAction(SET_ACTION(SkinnedActionManager::ABOUT, this, &MainWindow::about));
    m_mainMenu->addAction(SET_ACTION(SkinnedActionManager::ABOUT_QT, qApp, &QApplication::aboutQt));
    m_mainMenu->addSeparator();
    m_mainMenu->addAction(SET_ACTION(SkinnedActionManager::QUIT, m_uiHelper, &UiHelper::exit));

    QAction* forward = new QAction(this);
    forward->setShortcut(QKeySequence(Qt::Key_Right));
    connect(forward,SIGNAL(triggered(bool)),this,SLOT(forward()));
    QAction* backward = new QAction(this);
    backward->setShortcut(QKeySequence(Qt::Key_Left));
    connect(backward,SIGNAL(triggered(bool)),this,SLOT(backward()));

    Dock::instance()->addActions(QList<QAction*>() << forward << backward);
    Dock::instance()->addActions(SkinnedActionManager::instance()->actions());
}

void MainWindow::about()
{
    m_uiHelper->about(this);
}

void MainWindow::updateSettings()
{
    readSettings();
    m_playlist->readSettings();
    m_visMenu->updateActions();
    m_skin->reloadSkin();
    Dock::instance()->updateDock();
}

QMenu* MainWindow::menu()
{
    return m_mainMenu;
}

void MainWindow::loadPlaylist()
{
    m_uiHelper->loadPlayList(this);
}

void MainWindow::savePlaylist()
{
    m_uiHelper->savePlayList(this);
}

void MainWindow::playPause()
{
    if (m_core->state() == Qmmp::Playing)
        m_core->pause();
    else
        play();
}

void MainWindow::jumpToTrack()
{
    m_uiHelper->jumpToTrack(this);
}

void MainWindow::addUrl()
{
    m_uiHelper->addUrl(this);
}

SkinnedDisplay * MainWindow::mainDisplay() const
{
    return m_display;
}

void MainWindow::keyPressEvent(QKeyEvent *ke)
{
    QKeyEvent event = QKeyEvent(ke->type(), ke->key(),
                                ke->modifiers(), ke->text(),ke->isAutoRepeat(), ke->count());
    QApplication::sendEvent(m_playlist,&event);
}

void MainWindow::restoreWindowTitle()
{
    setWindowTitle(tr("Qmmp"));
}
