project(libgme)

include(CheckIncludeFileCXX)

# libgme
check_include_file_cxx(gme/gme.h GME_HEADER_FOUND)
find_library(GME_LIB NAME gme PATHS /usr/lib /usr/lib32  /usr/local/lib /usr/local/lib32)

if(GME_HEADER_FOUND AND (NOT GME_LIB MATCHES "^.*-NOTFOUND"))
    set(GME_FOUND TRUE CACHE INTERNAL "GME")
    message(STATUS "Found Game Music Emu: ${GME_LIB}")
else(GME_HEADER_FOUND AND (NOT GME_LIB MATCHES "^.*-NOTFOUND"))
    message(STATUS "Could not find Game Music Emu library")
endif(GME_HEADER_FOUND AND (NOT GME_LIB MATCHES "^.*-NOTFOUND"))


SET(libgme_SRCS
  decoder_gme.cpp
  decodergmefactory.cpp
  gmehelper.cpp
  gmesettingsdialog.cpp
  gmesettingsdialog.ui
  translations/translations.qrc
)

# Don't forget to include output directory, otherwise
# the UI file won't be wrapped!
include_directories(${CMAKE_CURRENT_BINARY_DIR})

if(GME_FOUND)
    add_library(gme MODULE ${libgme_SRCS} ${libgme_RCC_SRCS})
    target_link_libraries(gme PRIVATE Qt6::Widgets libqmmp ${GME_LIB})
    install(TARGETS gme DESTINATION ${PLUGIN_DIR}/Input)
endif(GME_FOUND)
