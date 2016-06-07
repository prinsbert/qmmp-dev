SET(USE_MPG123 TRUE CACHE BOOL "enable/disable mpg123 plugin")
SET(USE_XMP    TRUE CACHE BOOL "enable/disable xmp plugin")
SET(USE_FFAP   TRUE CACHE BOOL "enable/disable ffap plugin")
SET(USE_ASM    TRUE CACHE BOOL "enable/disable assembler optimizations (inside ffap decoder)")

INCLUDE(FindPkgConfig)
pkg_check_modules(TAGLIB taglib>=1.10)

IF(USE_MPG123 AND TAGLIB_FOUND)
add_subdirectory(mpg123)
ENDIF(USE_MPG123 AND TAGLIB_FOUND)

IF(USE_XMP)
add_subdirectory(xmp)
ENDIF(USE_XMP)

IF(USE_FFAP AND TAGLIB_FOUND)
add_subdirectory(ffap)
ENDIF(USE_FFAP AND TAGLIB_FOUND)
