project(libnull)

set(libnull_SRCS
  outputnull.cpp
  outputnullfactory.cpp
  translations/translations.qrc
)

# Don't forget to include output directory, otherwise
# the UI file won't be wrapped!
include_directories(${CMAKE_CURRENT_BINARY_DIR})


add_library(null MODULE ${libnull_SRCS})
add_dependencies(null libqmmp)
target_link_libraries(null PRIVATE Qt6::Widgets libqmmp)
install(TARGETS null DESTINATION ${PLUGIN_DIR}/Output)
