project(libfilewriter)

# libqmmp
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../../../)
link_directories(${CMAKE_CURRENT_BINARY_DIR}/../../../qmmp)
# libqmmpui
link_directories(${CMAKE_CURRENT_BINARY_DIR}/../../../qmmpui)

pkg_check_modules(VORBISENC vorbis ogg vorbisenc IMPORTED_TARGET)

set(libfilewriter_SRCS
    filewriterplugin.cpp
    filewritersettingsdialog.cpp
    effectfilewriterfactory.cpp
    filewritersettingsdialog.ui
    translations/translations.qrc
)

# Don't forget to include output directory, otherwise
# the UI file won't be wrapped!
include_directories(${CMAKE_CURRENT_BINARY_DIR})

if(VORBISENC_FOUND)
    add_library(filewriter MODULE ${libfilewriter_SRCS})
    add_dependencies(filewriter libqmmp libqmmpui)
    target_link_libraries(filewriter PRIVATE Qt6::Widgets libqmmp libqmmpui PkgConfig::VORBISENC)
    install(TARGETS filewriter DESTINATION ${PLUGIN_DIR}/Effect)
endif(VORBISENC_FOUND)
