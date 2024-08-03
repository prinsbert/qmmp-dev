project(libmpeg)

include(CheckIncludeFileCXX)

# libmad
if(USE_MAD)
    pkg_search_module(MAD libmad mad IMPORTED_TARGET)
endif(USE_MAD)

# libmpg123
if(USE_MPG123)
    pkg_search_module(MPG123 libmpg123>=1.13.0 IMPORTED_TARGET)
endif(USE_MPG123)

set(USE_LIBRCD FALSE CACHE BOOL "enable/disable charset detection for ID3v1/2 tags using librcd")

if(USE_LIBRCD)
    check_include_file_cxx(librcd.h LIBRCD_HEADER_FOUND)
endif(USE_LIBRCD)

if(LIBRCD_HEADER_FOUND)
    set(LIBRCD_FOUND TRUE CACHE INTERNAL "rcd")
    set(LIBRCD_LDFLAGS -lrcd)
    add_definitions(-DWITH_LIBRCD)
endif(LIBRCD_HEADER_FOUND)


set(libmpeg_SRCS
  decodermpegfactory.cpp
  mpegsettingsdialog.cpp
  tagextractor.cpp
  mpegmetadatamodel.cpp
  mpegsettingsdialog.ui
  translations/translations.qrc
)

if(MAD_FOUND)
    add_definitions(-DWITH_MAD)
    set(libmpeg_SRCS ${libmpeg_SRCS} decoder_mad.cpp)
endif(MAD_FOUND)

if(MPG123_FOUND)
    add_definitions(-DWITH_MPG123)
    set(libmpeg_SRCS ${libmpeg_SRCS} decoder_mpg123.cpp)
endif(MPG123_FOUND)

# Don't forget to include output directory, otherwise
# the UI file won't be wrapped!
include_directories(${CMAKE_CURRENT_BINARY_DIR})

if(MAD_FOUND OR MPG123_FOUND)
    add_library(mpeg MODULE ${libmpeg_SRCS})
    target_link_libraries(mpeg PRIVATE Qt5::Widgets libqmmp PkgConfig::TAGLIB PkgConfig::MAD PkgConfig::MPG123 ${LIBRCD_LDFLAGS})
    install(TARGETS mpeg DESTINATION ${PLUGIN_DIR}/Input)
endif(MAD_FOUND OR MPG123_FOUND)
