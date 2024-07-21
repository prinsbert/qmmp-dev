project(libconverter)

include_directories(${CMAKE_CURRENT_BINARY_DIR})

# libqmmpui & libqmmp
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../../../)
link_directories(${CMAKE_CURRENT_BINARY_DIR}/../../../qmmpui)
link_directories(${CMAKE_CURRENT_BINARY_DIR}/../../../qmmp)

set(libconverter_SRCS
    converterfactory.cpp
    converter.cpp
    converterhelper.cpp
    converterdialog.cpp
    converterpreseteditor.cpp
    converterdialog.ui
    converterpreseteditor.ui
    translations/translations.qrc
    presets.qrc
)

# Don't forget to include output directory, otherwise
# the UI file won't be wrapped!
include_directories(${CMAKE_CURRENT_BINARY_DIR})

add_library(converter MODULE ${libconverter_SRCS})
add_dependencies(converter libqmmpui)
target_link_libraries(converter PRIVATE Qt6::Widgets libqmmpui libqmmp PkgConfig::TAGLIB)
install(TARGETS converter DESTINATION ${PLUGIN_DIR}/General)
