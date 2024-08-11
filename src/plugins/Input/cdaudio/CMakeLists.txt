project(libcdaudio)

# libcdaudio
pkg_check_modules(CDIO libcdio>=1.0.0 libcdio_cdda libcdio_paranoia IMPORTED_TARGET)

set(USE_LIBCDDB TRUE CACHE BOOL "enable/disable CDDB support using libcddb")

if(USE_LIBCDDB AND CDIO_FOUND)
    pkg_check_modules(CDDB libcddb IMPORTED_TARGET)
endif()

set(libcdaudio_SRCS
  decoder_cdaudio.cpp
  decodercdaudiofactory.cpp
  cdaudiosettingsdialog.cpp
  cdaudiosettingsdialog.ui
  translations/translations.qrc
)

if(CDDB_FOUND)
    add_definitions(-DWITH_LIBCDDB)
endif()

# Don't forget to include output directory, otherwise
# the UI file won't be wrapped!
include_directories(${CMAKE_CURRENT_BINARY_DIR})

if(CDIO_FOUND)
    add_library(cdaudio MODULE ${libcdaudio_SRCS})
    target_link_libraries(cdaudio PRIVATE Qt6::Widgets libqmmp PkgConfig::CDIO)
    if(CDDB_FOUND)
        target_link_libraries(cdaudio PRIVATE PkgConfig::CDDB)
    endif()
    install(TARGETS cdaudio DESTINATION ${PLUGIN_DIR}/Input)
endif(CDIO_FOUND)
