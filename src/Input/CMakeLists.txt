set(USE_MODPLUG TRUE CACHE BOOL "enable/disable modplug plugin")
set(USE_FFAP    TRUE CACHE BOOL "enable/disable ffap plugin")

include(FindPkgConfig)
pkg_check_modules(TAGLIB taglib>=1.12 IMPORTED_TARGET)

if(USE_MODPLUG)
    add_subdirectory(modplug)
ENDIF(USE_MODPLUG)

if(USE_FFAP AND TAGLIB_FOUND)
    add_subdirectory(ffap)
endif(USE_FFAP AND TAGLIB_FOUND)
