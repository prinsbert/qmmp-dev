project(libffvideo)

# qt plugin
ADD_DEFINITIONS( -Wall )
ADD_DEFINITIONS(${QT_DEFINITIONS})
ADD_DEFINITIONS(-DQT_PLUGIN)
ADD_DEFINITIONS(-DQT_NO_DEBUG)
ADD_DEFINITIONS(-DQT_SHARED)
ADD_DEFINITIONS(-DQT_THREAD)
# fixes ffmpeg defines
ADD_DEFINITIONS(-D__STDC_CONSTANT_MACROS)

# libqmmp
include_directories(${QMMP_INCLUDE_DIRS})
link_directories( ${QMMP_LIBRARY_DIRS})

# minimal supported version
# ffmpeg-3.1: libavcodec-57.48.101, libavformat-57.40.101, libavutil-55.27.100, libswscale-4.1.100
pkg_check_modules(FFMPEG libavcodec>=57.48.101 libavformat>=57.40.101 libavutil>=55.27.100 libswscale>=4.1.100)

include_directories(${FFMPEG_INCLUDE_DIRS})
link_directories(${FFMPEG_LIBRARY_DIRS})
ADD_DEFINITIONS(${FFMPEG_CFLAGS})

SET(libffvideo_SRCS
  audiothread.cpp
  ffmpegengine.cpp
  ffvideodecoder.cpp
  ffvideofactory.cpp
  ffvideometadatamodel.cpp
  packetbuffer.cpp
  videothread.cpp
  videowindow.cpp
)

SET(libffvideo_HDRS
  ffvideodecoder.h
)

SET(libffvideo_RCCS translations/translations.qrc)

# Don't forget to include output directory, otherwise
# the UI file won't be wrapped!
include_directories(${CMAKE_CURRENT_BINARY_DIR})

IF(FFMPEG_FOUND)
ADD_LIBRARY(ffvideo MODULE ${libffvideo_SRCS} ${libffvideo_RCC_SRCS} ${libffvideo_HDRS})
target_link_libraries(ffvideo ${QT_LIBRARIES} ${QMMP_LDFLAGS} ${FFMPEG_LDFLAGS})
install(TARGETS ffvideo DESTINATION ${LIB_DIR}/qmmp/Engines)
ENDIF(FFMPEG_FOUND)
