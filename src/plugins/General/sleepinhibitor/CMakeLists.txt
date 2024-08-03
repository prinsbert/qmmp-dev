project(libsleepinhibitor)

set(libsleepinhibitor_SRCS
    sleepinhibitor.cpp
    sleepinhibitorfactory.cpp
    translations/translations.qrc
)

# Don't forget to include output directory, otherwise
# the UI file won't be wrapped!
include_directories(${CMAKE_CURRENT_BINARY_DIR})

add_library(sleepinhibitor MODULE ${libsleepinhibitor_SRCS})
target_link_libraries(sleepinhibitor PRIVATE Qt5::Widgets Qt5::DBus libqmmpui libqmmp)
install(TARGETS sleepinhibitor DESTINATION ${PLUGIN_DIR}/General)
