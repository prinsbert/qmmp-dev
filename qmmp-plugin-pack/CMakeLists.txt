cmake_minimum_required(VERSION 3.5.0 FATAL_ERROR)
project(qmmp-plugin-pack)

#freebsd support
include_directories(SYSTEM /usr/local/include)
SET(CMAKE_REQUIRED_INCLUDES ${CMAKE_REQUIRED_INCLUDES} /usr/local/include)

#Qt5 modules
SET(CMAKE_PREFIX_PATH ${CMAKE_PREFIX_PATH} /opt/qt515/lib/cmake)

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

SET(QMMP_PLUGIN_PACK_VERSION 1.5.0)
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
find_package(Qt5Network 5.4.0 REQUIRED)
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
IF(SVN_VERSION)
execute_process(COMMAND find ${CMAKE_CURRENT_SOURCE_DIR} -name *.ts COMMAND xargs ${QT_LRELEASE_EXECUTABLE} -silent -nounfinished)
ELSE(SVN_VERSION)
execute_process(COMMAND find ${CMAKE_CURRENT_SOURCE_DIR} -name *.ts COMMAND xargs ${QT_LRELEASE_EXECUTABLE} -silent)
ENDIF(SVN_VERSION)

SET(PLUGIN_DIR "${CMAKE_INSTALL_LIBDIR}/qmmp-${QMMP_PLUGIN_PACK_VERSION_MAJOR}.${QMMP_PLUGIN_PACK_VERSION_MINOR}" CACHE STRING "plugin path")
#SET(APP_NAME_SUFFIX "-1")

pkg_check_modules(QMMP qmmp${APP_NAME_SUFFIX}>=1.5.0 REQUIRED)
pkg_check_modules(QMMPUI qmmpui${APP_NAME_SUFFIX}>=1.5.0)

IF(QMMP_FOUND)
add_subdirectory(src)
include_directories(${QMMP_INCLUDEDIR})
ENDIF(QMMP_FOUND)

IF(QMMPUI_FOUND)
include_directories(${QMMPUI_INCLUDEDIR})
ENDIF(QMMPUI_FOUND)

MACRO(PRINT_SUMMARY str USE FOUND)
IF(${USE} AND ${FOUND})
MESSAGE("${str}enabled")
ELSE(${USE} AND ${FOUND})
MESSAGE("${str}disabled")
ENDIF(${USE} AND ${FOUND})
ENDMACRO(PRINT_SUMMARY)

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

MESSAGE("")
MESSAGE("Transports:")
PRINT_SUMMARY ("YouTube support ................." USE_YTB 1)

MESSAGE("")
MESSAGE("Input Plugins:")
PRINT_SUMMARY ("FFap plugin (Monkey's audio) ...." USE_FFAP TAGLIB_FOUND)
PRINT_SUMMARY ("XMP plugin ......................" USE_XMP XMP_FOUND)

MESSAGE("")
MESSAGE("Engines:")
PRINT_SUMMARY ("FFmpeg video plugin ............." USE_FFVIDEO FFMPEG_FOUND)

MESSAGE("")
MESSAGE("Effect Plugins:")
PRINT_SUMMARY ("Sample Rate Conversion .........." USE_SRC SAMPLERATE_FOUND)

MESSAGE("")
MESSAGE("Visual Plugins:")
PRINT_SUMMARY ("Goom visualization .............." USE_GOOM 1)

MESSAGE("")
MESSAGE("Advanced:")
PRINT_SUMMARY ("Goom assembler optimizations ...." GOOM_ASM USE_GOOM)


MESSAGE("")

install(FILES appdata/qmmp-plugin-pack${APP_NAME_SUFFIX}.appdata.xml DESTINATION ${CMAKE_INSTALL_DATADIR}/metainfo)
