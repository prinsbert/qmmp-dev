project(libcdaudio)

# libqmmp
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../../../)
link_directories(${CMAKE_CURRENT_BINARY_DIR}/../../../qmmp)

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
    add_dependencies(cdaudio libqmmp)
    target_link_libraries(cdaudio PRIVATE Qt6::Widgets libqmmp PkgConfig::CDIO)
    install(TARGETS cdaudio DESTINATION ${PLUGIN_DIR}/Input)
endif(CDIO_FOUND)
