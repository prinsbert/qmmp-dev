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
)

SET(libffap_HDRS
  decoder_ffap.h
  decoder_ffapcue.h
  ffap.h
)


SET(libffap_RCCS translations/translations.qrc)

QT5_ADD_RESOURCES(libffap_RCC_SRCS ${libffap_RCCS})

# Don't forget to include output directory, otherwise
# the UI file won't be wrapped!
include_directories(${CMAKE_CURRENT_BINARY_DIR})


ADD_LIBRARY(ffap MODULE ${libffap_SRCS} ${libffap_RCC_SRCS} ${libffap_HDRS})
target_link_libraries(ffap Qt5::Widgets ${QMMP_LDFLAGS} ${ffap_LDFLAGS} ${TAGLIB_LDFLAGS})
install(TARGETS ffap DESTINATION ${PLUGIN_DIR}/Input)
