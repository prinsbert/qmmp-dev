project(libmplayer)

set(libmplayer_SRCS
    mplayerenginefactory.cpp
    mplayerengine.cpp
    mplayermetadatamodel.cpp
    mplayersettingsdialog.cpp
    mplayersettingsdialog.ui
    translations/translations.qrc
)

add_library(mplayer MODULE ${libmplayer_SRCS})
target_link_libraries(mplayer PRIVATE Qt6::Widgets PkgConfig::QMMP)
install(TARGETS mplayer DESTINATION ${PLUGIN_DIR}/Engines)

