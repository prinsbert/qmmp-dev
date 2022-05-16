SET(USE_MODPLUG TRUE CACHE BOOL "enable/disable modplug plugin")
SET(USE_FFAP    TRUE CACHE BOOL "enable/disable ffap plugin")

INCLUDE(FindPkgConfig)
pkg_check_modules(TAGLIB taglib>=1.12)

IF(USE_MODPLUG)
add_subdirectory(modplug)
ENDIF(USE_MODPLUG)

IF(USE_FFAP AND TAGLIB_FOUND)
add_subdirectory(ffap)
ENDIF(USE_FFAP AND TAGLIB_FOUND)
