project(libconverter)

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
target_link_libraries(converter PRIVATE Qt5::Widgets libqmmpui libqmmp PkgConfig::TAGLIB)
install(TARGETS converter DESTINATION ${PLUGIN_DIR}/General)
