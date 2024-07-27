project(libffap)

SET(libffap_SRCS
  decoder_ffap.cpp
  decoder_ffapcue.cpp
  decoderffapfactory.cpp
  ffapmetadatamodel.cpp
  ffap.c
  translations/translations.qrc
)

add_library(ffap MODULE ${libffap_SRCS})
target_link_libraries(ffap PRIVATE Qt6::Widgets PkgConfig::QMMP PkgConfig::TAGLIB)
install(TARGETS ffap DESTINATION ${PLUGIN_DIR}/Input)
