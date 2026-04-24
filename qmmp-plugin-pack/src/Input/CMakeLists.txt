option(USE_MODPLUG "modplug plugin" ON)
option(USE_FFAP    "ffap plugin" ON)
option(USE_TIMIDITY "timidity plugin" ON)

include(FindPkgConfig)
pkg_check_modules(TAGLIB taglib>=1.12 IMPORTED_TARGET)

if(USE_MODPLUG)
    add_subdirectory(modplug)
endif(USE_MODPLUG)

if(USE_FFAP AND TAGLIB_FOUND)
    add_subdirectory(ffap)
endif(USE_FFAP AND TAGLIB_FOUND)

if(USE_TIMIDITY)
    add_subdirectory(timidity)
endif(USE_TIMIDITY)
