option(USE_MPLAYER "mplayer support" ON)

if(USE_MPLAYER)
    add_subdirectory(mplayer)
endif(USE_MPLAYER)
