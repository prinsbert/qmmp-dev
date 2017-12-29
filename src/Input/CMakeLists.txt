SET(USE_XMP    TRUE CACHE BOOL "enable/disable xmp plugin")
SET(USE_FFAP   TRUE CACHE BOOL "enable/disable ffap plugin")

INCLUDE(FindPkgConfig)
pkg_check_modules(TAGLIB taglib>=1.10)

IF(USE_XMP)
add_subdirectory(xmp)
ENDIF(USE_XMP)

IF(USE_FFAP AND TAGLIB_FOUND)
add_subdirectory(ffap)
ENDIF(USE_FFAP AND TAGLIB_FOUND)
