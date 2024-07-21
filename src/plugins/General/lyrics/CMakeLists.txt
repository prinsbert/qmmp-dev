project(liblyrics)

# libqmmpui & libqmmp
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../../../)
link_directories(${CMAKE_CURRENT_BINARY_DIR}/../../../qmmpui)
link_directories(${CMAKE_CURRENT_BINARY_DIR}/../../../qmmp)

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
add_dependencies(lyrics libqmmpui)
target_link_libraries(lyrics PRIVATE Qt6::Widgets Qt6::Network libqmmpui libqmmp)
install(TARGETS lyrics DESTINATION ${PLUGIN_DIR}/General)
