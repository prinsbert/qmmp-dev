project(libplsplaylistformat)

include_directories(${CMAKE_CURRENT_BINARY_DIR})

set(libplsplaylistformat_SRCS
    plsplaylistformat.cpp
)

add_library(plsplaylistformat MODULE ${libplsplaylistformat_SRCS})
target_link_libraries(plsplaylistformat PRIVATE Qt5::Widgets libqmmpui)
install(TARGETS plsplaylistformat DESTINATION ${PLUGIN_DIR}/PlayListFormats)
