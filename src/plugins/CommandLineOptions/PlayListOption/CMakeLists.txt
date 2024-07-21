project(libplaylistoption)

include_directories(${CMAKE_CURRENT_BINARY_DIR})

set(libplaylistoption_SRCS
    playlistoption.cpp
    translations/translations.qrc
)

add_library(playlistoption MODULE ${libplaylistoption_SRCS})
target_link_libraries(playlistoption PRIVATE Qt6::Widgets libqmmpui libqmmp)
install(TARGETS playlistoption DESTINATION ${PLUGIN_DIR}/CommandLineOptions)
