project(libmplayer)

set(libmplayer_SRCS
    mplayerenginefactory.cpp
    mplayerengine.cpp
    mplayermetadatamodel.cpp
    mplayersettingsdialog.cpp
    mplayersettingsdialog.ui
    translations/translations.qrc
)

# Don't forget to include output directory, otherwise
# the UI file won't be wrapped!
include_directories(${CMAKE_CURRENT_BINARY_DIR})

add_library(mplayer MODULE ${libmplayer_SRCS})
target_link_libraries(mplayer PRIVATE Qt6::Widgets libqmmp)
install(TARGETS mplayer DESTINATION ${PLUGIN_DIR}/Engines)

