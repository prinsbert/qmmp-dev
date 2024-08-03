project(liblyrics)

set(liblyrics_SRCS
    lyrics.cpp
    lyricsfactory.cpp
    lyricsprovider.cpp
    lyricswidget.cpp
    lyricssettingsdialog.cpp
    ultimatelyricsparser.cpp
    lyricswidget.ui
    lyricssettingsdialog.ui
    translations/translations.qrc
    providers/providers.qrc
)

# Don't forget to include output directory, otherwise
# the UI file won't be wrapped!
include_directories(${CMAKE_CURRENT_BINARY_DIR})

add_library(lyrics MODULE ${liblyrics_SRCS})
target_link_libraries(lyrics PRIVATE Qt5::Widgets Qt5::Network libqmmpui libqmmp)
install(TARGETS lyrics DESTINATION ${PLUGIN_DIR}/General)
