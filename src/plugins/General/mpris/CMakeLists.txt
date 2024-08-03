project(libmpris)

set(libmpris_SRCS
    mpris.cpp
    mprisfactory.cpp
    root2object.cpp
    player2object.cpp
    translations/translations.qrc
)

# Don't forget to include output directory, otherwise
# the UI file won't be wrapped!
include_directories(${CMAKE_CURRENT_BINARY_DIR})

add_library(mpris MODULE ${libmpris_SRCS})
target_link_libraries(mpris PRIVATE Qt5::Widgets Qt5::DBus libqmmpui libqmmp)
install(TARGETS mpris DESTINATION ${PLUGIN_DIR}/General)
