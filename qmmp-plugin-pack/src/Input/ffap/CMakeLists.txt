project(libffap)

# libqmmp
include_directories(${QMMP_INCLUDE_DIRS})
link_directories( ${QMMP_LIBRARY_DIRS})

include_directories(${TAGLIB_INCLUDE_DIRS})
link_directories(${TAGLIB_LIBRARY_DIRS})
ADD_DEFINITIONS(${TAGLIB_CFLAGS})

SET(libffap_SRCS
  decoder_ffap.cpp
  decoder_ffapcue.cpp
  decoderffapfactory.cpp
  ffapmetadatamodel.cpp
  ffap.c
  translations/translations.qrc
)

ADD_LIBRARY(ffap MODULE ${libffap_SRCS})
target_link_libraries(ffap PRIVATE Qt6::Widgets ${QMMP_LDFLAGS} ${ffap_LDFLAGS} ${TAGLIB_LDFLAGS})
install(TARGETS ffap DESTINATION ${PLUGIN_DIR}/Input)
