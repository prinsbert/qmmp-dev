project(librdetect)

SET(librdetect_SRCS
    rdetectfactory.cpp
    removablehelper.cpp
    rdetectsettingsdialog.cpp
    rdetectsettingsdialog.ui
    translations/translations.qrc
)

# Don't forget to include output directory, otherwise
# the UI file won't be wrapped!
include_directories(${CMAKE_CURRENT_BINARY_DIR})

add_library(rdetect MODULE ${librdetect_SRCS})
add_dependencies(rdetect libqmmpui)
target_link_libraries(rdetect PRIVATE Qt6::Widgets libqmmpui libqmmp)
install(TARGETS rdetect DESTINATION ${PLUGIN_DIR}/General)
