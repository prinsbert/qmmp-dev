set(USE_MPLAYER TRUE CACHE BOOL "enable/disable mplayer support")

if(USE_MPLAYER)
    add_subdirectory(mplayer)
endif(USE_MPLAYER)
