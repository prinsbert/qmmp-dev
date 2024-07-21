project(libm3uplaylistformat)

include_directories(${CMAKE_CURRENT_BINARY_DIR})

# libqmmpui
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../../../)
link_directories(${CMAKE_CURRENT_BINARY_DIR}/../../../qmmpui)

set(libm3uplaylistformat_SRCS
    m3uplaylistformat.cpp
)

add_library(m3uplaylistformat MODULE ${libm3uplaylistformat_SRCS})
target_link_libraries(m3uplaylistformat PRIVATE Qt6::Widgets libqmmpui)
install(TARGETS m3uplaylistformat DESTINATION ${PLUGIN_DIR}/PlayListFormats)
