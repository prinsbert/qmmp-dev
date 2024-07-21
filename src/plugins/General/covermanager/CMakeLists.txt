project(libcovermanager)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})

set(libcovermanager_SRCS
    coverwidget.cpp
    covermanager.cpp
    covermanagerfactory.cpp
    translations/translations.qrc
    images/cm_images.qrc
)

# Don't forget to include output directory, otherwise
# the UI file won't be wrapped!
include_directories(${CMAKE_CURRENT_BINARY_DIR})

add_library(covermanager MODULE ${libcovermanager_SRCS})
add_dependencies(covermanager libqmmpui)
target_link_libraries(covermanager PRIVATE Qt6::Widgets libqmmpui libqmmp)
install(TARGETS covermanager DESTINATION ${PLUGIN_DIR}/General)
