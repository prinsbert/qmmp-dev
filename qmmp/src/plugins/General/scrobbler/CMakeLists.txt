project(libscrobbler)

set(libscrobbler_SRCS
    scrobblersettingsdialog.cpp
    scrobblerhandler.cpp
    scrobblerfactory.cpp
    scrobblercache.cpp
    scrobbler.cpp
    scrobblersettingsdialog.ui
    translations/translations.qrc
)

# Don't forget to include output directory, otherwise
# the UI file won't be wrapped!
include_directories(${CMAKE_CURRENT_BINARY_DIR})

add_library(scrobbler MODULE ${libscrobbler_SRCS})
target_link_libraries(scrobbler PRIVATE Qt6::Widgets Qt6::Network libqmmpui libqmmp)
install(TARGETS scrobbler DESTINATION ${PLUGIN_DIR}/General)
