project(libsid)

include(CheckIncludeFileCXX)

# libsidplayfp
pkg_search_module(SIDPLAYFP libsidplayfp>=1.0.3 IMPORTED_TARGET)

# libresidfp support
if(SIDPLAYFP_FOUND)
    check_include_file_cxx(sidplayfp/builders/residfp.h RESIDFP_HEADER_FOUND ${SIDPLAYFP_CFLAGS})
    if(RESIDFP_HEADER_FOUND)
        add_definitions(-DHAVE_RESIDFP_HEADER)
    endif(RESIDFP_HEADER_FOUND)
endif(SIDPLAYFP_FOUND)

SET(libsid_SRCS
  decoder_sid.cpp
  decodersidfactory.cpp
  sidsettingsdialog.cpp
  sidhelper.cpp
  sidsettingsdialog.ui
  translations/translations.qrc
)

# Don't forget to include output directory, otherwise
# the UI file won't be wrapped!
include_directories(${CMAKE_CURRENT_BINARY_DIR})

if(SIDPLAYFP_FOUND)
    add_library(sid MODULE ${libsid_SRCS})
    target_link_libraries(sid PRIVATE Qt5::Widgets libqmmp PkgConfig::SIDPLAYFP)
    install(TARGETS sid DESTINATION ${PLUGIN_DIR}/Input)
endif(SIDPLAYFP_FOUND)
