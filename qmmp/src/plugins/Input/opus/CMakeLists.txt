project(libopus)

# libqmmp
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../../../)
link_directories(${CMAKE_CURRENT_BINARY_DIR}/../../../qmmp)

# libopus and libopusfile
pkg_check_modules(OPUS opus>=1.0.2 opusfile>=0.2 IMPORTED_TARGET)

set(libopus_SRCS
  decoder_opus.cpp
  decoderopusfactory.cpp
  opusmetadatamodel.cpp
  translations/translations.qrc
)

# Don't forget to include output directory, otherwise
# the UI file won't be wrapped!
include_directories(${CMAKE_CURRENT_BINARY_DIR})

if(OPUS_FOUND)
    add_library(opus MODULE ${libopus_SRCS})
    target_link_libraries(opus PRIVATE Qt6::Widgets libqmmp PkgConfig::OPUS PkgConfig::TAGLIB)
    install(TARGETS opus DESTINATION ${PLUGIN_DIR}/Input)
endif(OPUS_FOUND)
