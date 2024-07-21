project(librgscan)

# libqmmpui & libqmmp
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../../../)
link_directories(${CMAKE_CURRENT_BINARY_DIR}/../../../qmmpui)
link_directories(${CMAKE_CURRENT_BINARY_DIR}/../../../qmmp)

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
add_dependencies(rgscan libqmmpui)
target_link_libraries(rgscan PRIVATE Qt6::Widgets libqmmpui libqmmp PkgConfig::TAGLIB)
install(TARGETS rgscan DESTINATION ${PLUGIN_DIR}/General)
