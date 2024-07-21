project(libtrackchange)

set(libtrackchange_SRCS
    trackchange.cpp
    trackchangefactory.cpp
    trackchangesettingsdialog.cpp
    trackchangesettingsdialog.ui
    translations/translations.qrc
)

# Don't forget to include output directory, otherwise
# the UI file won't be wrapped!
include_directories(${CMAKE_CURRENT_BINARY_DIR})

add_library(trackchange MODULE ${libtrackchange_SRCS})
add_dependencies(trackchange libqmmpui)
target_link_libraries(trackchange PRIVATE Qt6::Widgets libqmmpui libqmmp)
install(TARGETS trackchange DESTINATION ${PLUGIN_DIR}/General)
