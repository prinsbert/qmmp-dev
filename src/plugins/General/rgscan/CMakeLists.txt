project(librgscan)

set(librgscan_SRCS
    rgscanfactory.cpp
    rgscanhelper.cpp
    rgscandialog.cpp
    rgscanner.cpp
    gain_analysis.c
    rgscandialog.ui
    translations/translations.qrc
)

# Don't forget to include output directory, otherwise
# the UI file won't be wrapped!
include_directories(${CMAKE_CURRENT_BINARY_DIR})

add_library(rgscan MODULE ${librgscan_SRCS} ${librgscan_UIS_H})
target_link_libraries(rgscan PRIVATE Qt5::Widgets libqmmpui libqmmp PkgConfig::TAGLIB)
install(TARGETS rgscan DESTINATION ${PLUGIN_DIR}/General)
