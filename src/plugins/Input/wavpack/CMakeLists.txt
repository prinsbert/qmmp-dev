project(libwavpack)

# wavpack
pkg_search_module(WAVPACK wavpack IMPORTED_TARGET)

SET(libwavpack_SRCS
  decoder_wavpack.cpp
  decoderwavpackfactory.cpp
  wavpackmetadatamodel.cpp
  translations/translations.qrc
)

include_directories(${CMAKE_CURRENT_BINARY_DIR})

if(WAVPACK_FOUND)
    add_library(wavpack MODULE ${libwavpack_SRCS})
    target_link_libraries(wavpack PRIVATE Qt5::Widgets libqmmp PkgConfig::WAVPACK)
    install(TARGETS wavpack DESTINATION ${PLUGIN_DIR}/Input)
endif(WAVPACK_FOUND)

