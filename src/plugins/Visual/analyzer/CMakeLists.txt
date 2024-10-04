project(libanalyzer)

SET(libanalyzer_SRCS
    analyzer.cpp
    analyzercolorwidget.cpp
    analyzersettingsdialog.cpp
    visualanalyzerfactory.cpp
    fft.c
    analyzersettingsdialog.ui
    translations/translations.qrc
)

# Don't forget to include output directory, otherwise
# the UI file won't be wrapped!
include_directories(${CMAKE_CURRENT_BINARY_DIR} ${CMAKE_CURRENT_SOURCE_DIR})

add_library(analyzer MODULE ${libanalyzer_SRCS})
target_link_libraries(analyzer PRIVATE Qt5::Widgets libqmmp)
install(TARGETS analyzer DESTINATION ${PLUGIN_DIR}/Visual)
