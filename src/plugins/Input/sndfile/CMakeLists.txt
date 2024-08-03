project(libsndfile)

# libsndfile
pkg_search_module(SNDFILE sndfile>=1.0.22 IMPORTED_TARGET)

SET(libsndfile_SRCS
  decoder_sndfile.cpp
  decodersndfilefactory.cpp
  translations/translations.qrc
)

# Don't forget to include output directory, otherwise
# the UI file won't be wrapped!
include_directories(${CMAKE_CURRENT_BINARY_DIR})

if(SNDFILE_FOUND)
    add_library(sndfile MODULE ${libsndfile_SRCS})
    target_link_libraries(sndfile PRIVATE Qt5::Widgets libqmmp PkgConfig::SNDFILE)
    install(TARGETS sndfile DESTINATION ${PLUGIN_DIR}/Input)
endif(SNDFILE_FOUND)



