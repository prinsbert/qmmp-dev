project(libfileops)

set(libfileops_SRCS
    fileops.cpp
    fileopsfactory.cpp
    fileopssettingsdialog.cpp
    fileopssettingsdialog.ui
    translations/translations.qrc
)

# Don't forget to include output directory, otherwise
# the UI file won't be wrapped!
include_directories(${CMAKE_CURRENT_BINARY_DIR})

add_library(fileops MODULE ${libfileops_SRCS})
target_link_libraries(fileops PRIVATE Qt5::Widgets libqmmpui libqmmp)
install(TARGETS fileops DESTINATION ${PLUGIN_DIR}/General)
