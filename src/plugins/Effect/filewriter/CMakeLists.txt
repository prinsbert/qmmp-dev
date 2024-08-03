project(libfilewriter)

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
    target_link_libraries(filewriter PRIVATE Qt5::Widgets libqmmp libqmmpui PkgConfig::VORBISENC)
    install(TARGETS filewriter DESTINATION ${PLUGIN_DIR}/Effect)
endif(VORBISENC_FOUND)
