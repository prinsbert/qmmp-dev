project(skinned)

set(skinned_SRCS
	fft.c
	cursorimage.cpp
	dock.cpp
	monostereo.cpp
	pixmapwidget.cpp
	shadedbar.cpp
	shadedvisual.cpp
	skin.cpp
	skinnedactionmanager.cpp
	skinnedbalancebar.cpp
	skinnedbutton.cpp
	skinneddisplay.cpp
	skinnedeqgraph.cpp
	skinnedeqslider.cpp
	skinnedeqtitlebar.cpp
	skinnedeqwidget.cpp
	skinnedfactory.cpp
	skinnedhorizontalslider.cpp
	skinnedhotkeyeditor.cpp
	skinnedkeyboardmanager.cpp
	skinnedlistwidget.cpp
	skinnedlistwidgetdrawer.cpp
	skinnedmainwindow.cpp
	skinnednumber.cpp
	skinnedplaylistbrowser.cpp
	skinnedplaylistcontrol.cpp
	skinnedplaylist.cpp
	skinnedplaylistheader.cpp
	skinnedplaylistselector.cpp
	skinnedplaylistslider.cpp
	skinnedplaylisttitlebar.cpp
	skinnedplaystatus.cpp
	skinnedpopupsettings.cpp
	skinnedpopupwidget.cpp
	skinnedpositionbar.cpp
	skinnedpreseteditor.cpp
	skinnedsettings.cpp
	skinnedshortcutitem.cpp
	skinnedtextscroller.cpp
	skinnedtimeindicator.cpp
	skinnedtitlebarcontrol.cpp
	skinnedtitlebar.cpp
	skinnedtogglebutton.cpp
	skinnedvisualization.cpp
	skinnedvolumebar.cpp
	skinreader.cpp
	symboldisplay.cpp
	windowsystem.cpp
	forms/skinnedsettings.ui
	forms/skinnedpreseteditor.ui
	forms/skinnedplaylistbrowser.ui
	forms/skinnedpopupsettings.ui
	forms/skinnedhotkeyeditor.ui
	glare/glare.qrc
	resources/resources.qrc
	translations/translations.qrc
)

# Don't forget to include output directory, otherwise
# the UI file won't be wrapped!
include_directories(${CMAKE_CURRENT_BINARY_DIR} ${CMAKE_CURRENT_SOURCE_DIR})
set(CMAKE_AUTOUIC_SEARCH_PATHS ${CMAKE_AUTOUIC_SEARCH_PATHS} ${CMAKE_CURRENT_SOURCE_DIR}/forms)

add_library(skinned MODULE ${skinned_SRCS} ${skinned_UIS_H})
target_link_libraries(skinned PRIVATE Qt5::Widgets Qt5::X11Extras libqmmpui libqmmp PkgConfig::X11)
install(TARGETS skinned DESTINATION ${PLUGIN_DIR}/Ui)
install(FILES scripts/kwin.sh DESTINATION ${CMAKE_INSTALL_DATADIR}/qmmp${APP_NAME_SUFFIX}/scripts)
