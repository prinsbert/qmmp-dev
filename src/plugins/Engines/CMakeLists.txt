option(USE_MPLAYER "enable/disable mplayer support" ON)

if(USE_MPLAYER)
    add_subdirectory(mplayer)
endif(USE_MPLAYER)
