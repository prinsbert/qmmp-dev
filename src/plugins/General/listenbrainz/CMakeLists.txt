project(liblistenbrainz)

SET(liblistenbrainz_SRCS
    listenbrainzsettingsdialog.cpp
    listenbrainzfactory.cpp
    payloadcache.cpp
    listenbrainz.cpp
    listenbrainzsettingsdialog.ui
    translations/translations.qrc
)

# Don't forget to include output directory, otherwise
# the UI file won't be wrapped!
include_directories(${CMAKE_CURRENT_BINARY_DIR})

add_library(listenbrainz MODULE ${liblistenbrainz_SRCS})
add_dependencies(listenbrainz libqmmpui)
target_link_libraries(listenbrainz PRIVATE Qt6::Widgets Qt6::Network libqmmpui libqmmp)
install(TARGETS listenbrainz DESTINATION ${PLUGIN_DIR}/General)
