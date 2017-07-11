cmake_minimum_required(VERSION 2.8.11 FATAL_ERROR)

#freebsd support
include_directories(SYSTEM /usr/local/include)
SET(CMAKE_REQUIRED_INCLUDES ${CMAKE_REQUIRED_INCLUDES} /usr/local/include)

#Qt5 modules
SET(CMAKE_PREFIX_PATH ${CMAKE_PREFIX_PATH} /opt/qt56/lib/cmake)


CONFIGURE_FILE(
  "${CMAKE_CURRENT_SOURCE_DIR}/cmake_uninstall.cmake.in"
  "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake"
  IMMEDIATE @ONLY)

ADD_CUSTOM_TARGET(uninstall
  "${CMAKE_COMMAND}" -P "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake")

SET(QMMP_PLUGIN_PACK_VERSION 1.2.0)
SET(SVN_VERSION TRUE)

INCLUDE(FindPkgConfig)
set(CMAKE_AUTOMOC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
find_package(Qt5Widgets 5.4.0 REQUIRED)
find_package(Qt5LinguistTools 5.4.0 REQUIRED)
find_package(Qt5Sql 5.4.0)

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

IF(NOT LIB_SUFFIX)
IF("${QT_LIBRARY_DIR}" MATCHES lib64)
set(LIB_SUFFIX 64)
ENDIF("${QT_LIBRARY_DIR}" MATCHES lib64)
ENDIF(NOT LIB_SUFFIX)

SET(LIB_DIR "lib${LIB_SUFFIX}" CACHE STRING "library path")

pkg_check_modules(QMMP qmmp>=1.2.0 REQUIRED)
pkg_check_modules(QMMPUI qmmpui>=1.2.0)

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
    COMMAND rm
    ARGS -rf ${CMAKE_CURRENT_BINARY_DIR}/install_manifest.txt
    TARGET  distclean
)
ENDIF(UNIX)

MESSAGE("")
MESSAGE("Input Plugins:")
PRINT_SUMMARY ("mpg123 plugin ..................." USE_MPG123  MPG123_FOUND)
PRINT_SUMMARY ("FFap plugin (Monkey's audio) ...." USE_FFAP TAGLIB_FOUND)
PRINT_SUMMARY ("XMP plugin ......................" USE_XMP XMP_FOUND)

MESSAGE("")
MESSAGE("Effect Plugins:")
PRINT_SUMMARY ("Sample Rate Conversion .........." USE_SRC SAMPLERATE_FOUND)

MESSAGE("")
MESSAGE("Visual Plugins:")
PRINT_SUMMARY ("Goom visualization .............." USE_GOOM 1)

MESSAGE("")
MESSAGE("General Plugins:")
PRINT_SUMMARY ("History plugin .................." USE_HISTORY QMMPUI_FOUND AND Qt5Sql_FOUND)

MESSAGE("")
MESSAGE("Advanced:")
PRINT_SUMMARY ("FFap sssempler optimizations ...." YASM_FOUND USE_FFAP)
PRINT_SUMMARY ("Goom assempler optimizations ...." GOOM_ASM USE_GOOM)


MESSAGE("")
