project(libcopypaste)

set(libcopypaste_SRCS
    copypaste.cpp
    copypastefactory.cpp
    translations/translations.qrc
)

# Don't forget to include output directory, otherwise
# the UI file won't be wrapped!
include_directories(${CMAKE_CURRENT_BINARY_DIR})

add_library(copypaste MODULE ${libcopypaste_SRCS})
target_link_libraries(copypaste PRIVATE Qt5::Widgets libqmmpui libqmmp)
install(TARGETS copypaste DESTINATION ${PLUGIN_DIR}/General)
