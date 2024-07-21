project(libxspfplaylistformat)

include_directories(${CMAKE_CURRENT_BINARY_DIR})

# libqmmpui
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../../../)
link_directories(${CMAKE_CURRENT_BINARY_DIR}/../../../qmmpui)
link_directories(${CMAKE_CURRENT_BINARY_DIR}/../../../qmmp)

set(libxspfplaylistformat_SRCS
    xspfplaylistformat.cpp
)

add_library(xspfplaylistformat MODULE ${libxspfplaylistformat_SRCS})
add_dependencies(xspfplaylistformat libqmmpui)
target_link_libraries(xspfplaylistformat PRIVATE Qt6::Widgets libqmmpui libqmmp)
install(TARGETS xspfplaylistformat DESTINATION ${PLUGIN_DIR}/PlayListFormats)
