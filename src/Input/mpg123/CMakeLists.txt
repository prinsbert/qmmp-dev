project(libmpg123)

if(COMMAND cmake_policy)
    cmake_policy(SET CMP0003 NEW)
    cmake_policy(SET CMP0005 NEW)
endif(COMMAND cmake_policy)

# libqmmp
include_directories(${QMMP_INCLUDE_DIRS})
link_directories( ${QMMP_LIBRARY_DIRS})

# libmpg123
pkg_check_modules(MPG123 libmpg123>=1.13.0)

include_directories(${MPG123_INCLUDE_DIRS} ${TAGLIB_INCLUDE_DIRS})
link_directories(${MPG123_LIBRARY_DIRS} ${TAGLIB_LIBRARY_DIRS})
ADD_DEFINITIONS(${MPG123_CFLAGS} ${TAGLIB_CFLAGS})

SET(libmpg123_SRCS
  decoder_mpg123.cpp
  decodermpg123factory.cpp
  settingsdialog.cpp
  tagextractor.cpp
  mpegmetadatamodel.cpp
  replaygainreader.cpp
)

SET(libmpg123_HDRS
  decoder_mpg123.h
  tagextractor.h
  replaygainreader.h
)

SET(libmpg123_RCCS translations/translations.qrc)

QT5_ADD_RESOURCES(libmpg123_RCC_SRCS ${libmpg123_RCCS})

# user interface

SET(libmpg123_UIS
    settingsdialog.ui
)

QT5_WRAP_UI(libmpg123_UIS_H ${libmpg123_UIS})
# Don't forget to include output directory, otherwise
# the UI file won't be wrapped!
include_directories(${CMAKE_CURRENT_BINARY_DIR})

IF(MPG123_FOUND)
ADD_LIBRARY(mpg123 MODULE ${libmpg123_SRCS} ${libmpg123_UIS_H} ${libmpg123_RCC_SRCS} ${libmpg123_HDRS})
target_link_libraries(mpg123 Qt5::Widgets ${QMMP_LDFLAGS} ${MPG123_LDFLAGS} ${TAGLIB_LDFLAGS})
install(TARGETS mpg123 DESTINATION ${LIB_DIR}/qmmp/Input)
ENDIF(MPG123_FOUND)
