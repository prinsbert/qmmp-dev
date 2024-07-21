set(USE_SOXR TRUE CACHE BOOL "enable/disable SoX resampler plugin")
set(USE_BS2B TRUE CACHE BOOL "enable/disable bs2b plugin")
set(USE_LADSPA TRUE CACHE BOOL "enable/disable ladspa plugin")
set(USE_CROSSFADE TRUE CACHE BOOL "enable/disable crossfade plugin")
set(USE_STEREO TRUE CACHE BOOL "enable/disable extra stereo plugin")
set(USE_FILEWRITER TRUE CACHE BOOL "enable/disable file writer plugin")
set(USE_MONOTOSTEREO TRUE CACHE BOOL "enable/disable mono to stereo converter plugin")

if(USE_SOXR)
    add_subdirectory(soxr)
endif(USE_SOXR)

if(USE_BS2B)
    add_subdirectory(bs2b)
endif(USE_BS2B)

if(USE_LADSPA)
   add_subdirectory(ladspa)
endif(USE_LADSPA)

if(USE_CROSSFADE)
    add_subdirectory(crossfade)
endif(USE_CROSSFADE)

if(USE_STEREO)
    add_subdirectory(stereo)
endif(USE_STEREO)

if(USE_FILEWRITER)
    add_subdirectory(filewriter)
endif(USE_FILEWRITER)

if(USE_MONOTOSTEREO)
    add_subdirectory(monotostereo)
endif(USE_MONOTOSTEREO)
