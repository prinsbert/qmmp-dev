cmake_minimum_required(VERSION 2.8.11 FATAL_ERROR)
project(libffmpeg)

#installation paths
include(GNUInstallDirs)

if(POLICY CMP0071)
  cmake_policy(SET CMP0071 NEW)
endif()

CONFIGURE_FILE(
  "${CMAKE_CURRENT_SOURCE_DIR}/cmake_uninstall.cmake.in"
  "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake"
  IMMEDIATE @ONLY)

ADD_CUSTOM_TARGET(uninstall
  "${CMAKE_COMMAND}" -P "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake")
  
SET(QMMP_PLUGIN_PACK_VERSION 1.4.0)
SET(SVN_VERSION TRUE)

string(REPLACE "." ";" VERSIONS ${QMMP_PLUGIN_PACK_VERSION})
list(GET VERSIONS 0 QMMP_PLUGIN_PACK_VERSION_MAJOR)
list(GET VERSIONS 1 QMMP_PLUGIN_PACK_VERSION_MINOR)
list(GET VERSIONS 2 QMMP_PLUGIN_PACK_VERSION_PATCH)

INCLUDE(FindPkgConfig)
set(CMAKE_AUTOMOC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
find_package(Qt5Widgets 5.4.0 REQUIRED)
find_package(Qt5LinguistTools 5.4.0 REQUIRED)
set(CMAKE_CXX_VISIBILITY_PRESET hidden)
set(CMAKE_VISIBILITY_INLINES_HIDDEN 1)
set(CMAKE_CXX_STANDARD 11)

get_target_property(QT_LRELEASE_EXECUTABLE Qt5::lrelease IMPORTED_LOCATION)

IF(QT_LRELEASE_EXECUTABLE)
MESSAGE(STATUS "Found lrelease executable: " ${QT_LRELEASE_EXECUTABLE})
ELSE(QT_LRELEASE_EXECUTABLE)
MESSAGE(FATAL_ERROR "Could NOT find lrelease executable")
ENDIF(QT_LRELEASE_EXECUTABLE)

MESSAGE(STATUS "Generating translations ...")
execute_process(COMMAND find ${CMAKE_CURRENT_SOURCE_DIR} -name *.ts COMMAND xargs ${QT_LRELEASE_EXECUTABLE} -silent)


SET(PLUGIN_DIR "${CMAKE_INSTALL_LIBDIR}/qmmp-${QMMP_PLUGIN_PACK_VERSION_MAJOR}.${QMMP_PLUGIN_PACK_VERSION_MINOR}" CACHE STRING "plugin path")

# fixes ffmpeg defines
ADD_DEFINITIONS(-D__STDC_CONSTANT_MACROS)

include_directories(${CMAKE_CURRENT_BINARY_DIR})



# minimal supported versions
# ffmpeg-3.2: libavcodec-57.64.100, libavformat-56.40.0, libavutil-55.34.100
pkg_check_modules(FFMPEG libavcodec>=56.60.0 libavformat>=56.40.0 libavutil>=54.31.0 REQUIRED)
pkg_check_modules(QMMP qmmp${APP_NAME_SUFFIX}>=1.4.0 REQUIRED)

include_directories(${FFMPEG_INCLUDE_DIRS})
link_directories(${FFMPEG_LIBRARY_DIRS})
ADD_DEFINITIONS(${FFMPEG_CFLAGS})

# libqmmp
include_directories(${QMMP_INCLUDE_DIRS})
link_directories( ${QMMP_LIBRARY_DIRS})


SET(libffmpeg_SRCS
  decoder_ffmpeg.cpp
  decoder_ffmpegcue.cpp
  decoderffmpegfactory.cpp
  ffmpegmetadatamodel.cpp
  settingsdialog.cpp
  replaygainreader.cpp
)

SET(libffmpeg_HDRS
  decoder_ffmpeg.h
  decoder_ffmpegcue.h
  replaygainreader.h
)

SET(libffmpeg_RCCS translations/translations.qrc)

QT5_ADD_RESOURCES(libffmpeg_RCC_SRCS ${libffmpeg_RCCS})

# user interface

SET(libffmpeg_UIS
  settingsdialog.ui
)

QT5_WRAP_UI(libffmpeg_UIS_H ${libffmpeg_UIS})
# Don't forget to include output directory, otherwise
# the UI file won't be wrapped!
include_directories(${CMAKE_CURRENT_BINARY_DIR})


ADD_LIBRARY(ffmpeg MODULE ${libffmpeg_SRCS} ${libffmpeg_UIS_H} ${libffmpeg_RCC_SRCS} ${libffmpeg_HDRS})
target_link_libraries(ffmpeg Qt5::Widgets ${QMMP_LDFLAGS} ${FFMPEG_LDFLAGS})
install(TARGETS ffmpeg DESTINATION ${PLUGIN_DIR}/Input)

IF (UNIX)
ADD_CUSTOM_TARGET (distclean @echo cleaning for source distribution)

ADD_CUSTOM_COMMAND(
    COMMENT "distribution clean"
    COMMAND make
    ARGS -C ${CMAKE_CURRENT_BINARY_DIR} clean
    COMMAND find
    ARGS ${CMAKE_CURRENT_BINARY_DIR} -name "CMakeCache.txt" | xargs rm -rf
    COMMAND find
    ARGS ${CMAKE_CURRENT_BINARY_DIR} -name "CMakeFiles" | xargs rm -rf
    COMMAND find
    ARGS ${CMAKE_CURRENT_BINARY_DIR} -name "Makefile" | xargs rm -rf
    COMMAND find
    ARGS ${CMAKE_CURRENT_BINARY_DIR} -name \"*.cmake\" | xargs rm -rf
    COMMAND find
    ARGS ${CMAKE_CURRENT_SOURCE_DIR} -name "*.qm" | xargs rm -rf
    COMMAND find
    ARGS ${CMAKE_CURRENT_BINARY_DIR} -name "*.pc" | xargs rm -rf
    COMMAND find
    ARGS ${CMAKE_CURRENT_BINARY_DIR} -name "*.depends" | xargs rm -rf
    COMMAND find
    ARGS ${CMAKE_CURRENT_BINARY_DIR} -name "*.cxx_parameters" | xargs rm -rf
    COMMAND find
    ARGS ${CMAKE_CURRENT_BINARY_DIR} -name "moc_*.cpp" | xargs rm -rf
    COMMAND find
    ARGS ${CMAKE_CURRENT_BINARY_DIR} -name "*_automoc.cpp" | xargs rm -rf
    COMMAND rm
    ARGS -rf ${CMAKE_CURRENT_BINARY_DIR}/install_manifest.txt
    COMMAND find
    ARGS ${CMAKE_CURRENT_BINARY_DIR} -name *_automoc.dir | xargs rm -rf
    TARGET  distclean
)
ENDIF(UNIX)
