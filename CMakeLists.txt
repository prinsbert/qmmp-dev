cmake_minimum_required(VERSION 3.18.0 FATAL_ERROR)
project(qmmp-plugin-pack)

#freebsd support
include_directories(SYSTEM /usr/local/include)
set(CMAKE_REQUIRED_INCLUDES ${CMAKE_REQUIRED_INCLUDES} /usr/local/include)

#Qt6 modules
set(CMAKE_PREFIX_PATH ${CMAKE_PREFIX_PATH} /home/user/qt6)

#installation paths
include(GNUInstallDirs)
include(FindPkgConfig)

if(POLICY CMP0071)
    cmake_policy(SET CMP0071 NEW)
endif()

if(POLICY CMP0175)
    cmake_policy(SET CMP0175 NEW)
endif()


configure_file(
  "${CMAKE_CURRENT_SOURCE_DIR}/cmake_uninstall.cmake.in"
  "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake"
  IMMEDIATE @ONLY)

add_custom_target(uninstall
  "${CMAKE_COMMAND}" -P "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake")

set(QMMP_PLUGIN_PACK_VERSION 2.3.2)
set(SVN_VERSION TRUE)

string(REPLACE "." ";" VERSIONS ${QMMP_PLUGIN_PACK_VERSION})
list(GET VERSIONS 0 QMMP_PLUGIN_PACK_VERSION_MAJOR)
list(GET VERSIONS 1 QMMP_PLUGIN_PACK_VERSION_MINOR)
list(GET VERSIONS 2 QMMP_PLUGIN_PACK_VERSION_PATCH)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_C_VISIBILITY_PRESET hidden)
set(CMAKE_CXX_VISIBILITY_PRESET hidden)
set(CMAKE_VISIBILITY_INLINES_HIDDEN 1)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

ADD_DEFINITIONS(-DQMMP_WS_X11 -DQT_DISABLE_DEPRECATED_BEFORE=0x060200 -DQT_DEPRECATED_WARNINGS)
ADD_COMPILE_OPTIONS(-Wall -Wextra)

find_package(Qt6 6.2.0 CONFIG REQUIRED COMPONENTS Widgets LinguistTools Network)

get_target_property(QT_LRELEASE_EXECUTABLE Qt6::lrelease IMPORTED_LOCATION)

if(QT_LRELEASE_EXECUTABLE)
    MESSAGE(STATUS "Found lrelease executable: " ${QT_LRELEASE_EXECUTABLE})
else(QT_LRELEASE_EXECUTABLE)
    MESSAGE(FATAL_ERROR "Could NOT find lrelease executable")
endif(QT_LRELEASE_EXECUTABLE)

MESSAGE(STATUS "Generating translations ...")
if(SVN_VERSION)
    execute_process(COMMAND find ${CMAKE_CURRENT_SOURCE_DIR} -name *.ts COMMAND xargs ${QT_LRELEASE_EXECUTABLE} -silent -nounfinished)
else(SVN_VERSION)
    execute_process(COMMAND find ${CMAKE_CURRENT_SOURCE_DIR} -name *.ts COMMAND xargs ${QT_LRELEASE_EXECUTABLE} -silent)
endif(SVN_VERSION)

SET(PLUGIN_DIR "${CMAKE_INSTALL_LIBDIR}/qmmp-${QMMP_PLUGIN_PACK_VERSION_MAJOR}.${QMMP_PLUGIN_PACK_VERSION_MINOR}" CACHE STRING "plugin path")
#SET(APP_NAME_SUFFIX "-1")

pkg_check_modules(QMMP qmmp${APP_NAME_SUFFIX}>=2.3.0 REQUIRED IMPORTED_TARGET)
pkg_check_modules(QMMPUI qmmpui${APP_NAME_SUFFIX}>=2.3.0 IMPORTED_TARGET)

if(QMMP_FOUND)
    add_subdirectory(src)
    include_directories(${QMMP_INCLUDEDIR})
endif(QMMP_FOUND)

if(QMMPUI_FOUND)
    include_directories(${QMMPUI_INCLUDEDIR})
endif(QMMPUI_FOUND)

MACRO(PRINT_SUMMARY str USE FOUND)
if(${USE} AND ${FOUND})
    MESSAGE("${str}enabled")
else(${USE} AND ${FOUND})
    MESSAGE("${str}disabled")
endif(${USE} AND ${FOUND})
ENDMACRO(PRINT_SUMMARY)

add_custom_target(distclean @echo cleaning for source distribution)

add_custom_command(
    POST_BUILD
    COMMENT "distribution clean"
    COMMAND ${CMAKE_MAKE_PROGRAM}
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
    ARGS ${CMAKE_CURRENT_BINARY_DIR} -name "qrc_*.cxx" | xargs rm -rf
    COMMAND find
    ARGS ${CMAKE_CURRENT_BINARY_DIR} -name "*_automoc.cpp" | xargs rm -rf
    COMMAND rm
    ARGS -rf ${CMAKE_CURRENT_BINARY_DIR}/install_manifest.txt
    COMMAND find
    ARGS ${CMAKE_CURRENT_BINARY_DIR} -name *_automoc.dir | xargs rm -rf
    COMMAND rm
    ARGS -rf ${CMAKE_CURRENT_BINARY_DIR}/doc/html .qt config.tests build.ninja .ninja_deps
    TARGET  distclean
)

MESSAGE("")
MESSAGE("Transports:")
PRINT_SUMMARY ("YouTube support ................." USE_YTB 1)
PRINT_SUMMARY ("MMS support ....................." USE_MMS MMS_FOUND)

MESSAGE("")
MESSAGE("Input Plugins:")
PRINT_SUMMARY ("FFap plugin (Monkey's audio) ...." USE_FFAP TAGLIB_FOUND)
PRINT_SUMMARY ("ModPlug plugin .................." USE_MODPLUG MODPLUG_FOUND)

MESSAGE("")
MESSAGE("Engines:")
PRINT_SUMMARY ("FFmpeg video plugin ............." USE_FFVIDEO FFMPEG_FOUND)
PRINT_SUMMARY ("Mplayer video plugin ............" USE_MPLAYER 1)
PRINT_SUMMARY ("Mpv video plugin ................" USE_MPV MPV_FOUND)

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

if(UNIX)
    install(FILES appdata/qmmp-plugin-pack${APP_NAME_SUFFIX}.appdata.xml DESTINATION ${CMAKE_INSTALL_DATADIR}/metainfo)
endif(UNIX)
