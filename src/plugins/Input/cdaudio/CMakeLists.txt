project(libcdaudio)

# libcdaudio
pkg_check_modules(CDIO libcdio>=1.0.0 libcdio_cdda libcdio_paranoia libcddb IMPORTED_TARGET)

SET(libcdaudio_SRCS
  decoder_cdaudio.cpp
  decodercdaudiofactory.cpp
  cdaudiosettingsdialog.cpp
  cdaudiosettingsdialog.ui
  translations/translations.qrc
)

# Don't forget to include output directory, otherwise
# the UI file won't be wrapped!
include_directories(${CMAKE_CURRENT_BINARY_DIR})

if(CDIO_FOUND)
    add_library(cdaudio MODULE ${libcdaudio_SRCS})
    target_link_libraries(cdaudio PRIVATE Qt5::Widgets libqmmp PkgConfig::CDIO)
    install(TARGETS cdaudio DESTINATION ${PLUGIN_DIR}/Input)
endif(CDIO_FOUND)
