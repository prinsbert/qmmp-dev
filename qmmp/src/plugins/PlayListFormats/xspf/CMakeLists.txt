project(libxspfplaylistformat)

include_directories(${CMAKE_CURRENT_BINARY_DIR})

set(libxspfplaylistformat_SRCS
    xspfplaylistformat.cpp
)

add_library(xspfplaylistformat MODULE ${libxspfplaylistformat_SRCS})
add_dependencies(xspfplaylistformat libqmmpui)
target_link_libraries(xspfplaylistformat PRIVATE Qt6::Widgets libqmmpui libqmmp)
install(TARGETS xspfplaylistformat DESTINATION ${PLUGIN_DIR}/PlayListFormats)
