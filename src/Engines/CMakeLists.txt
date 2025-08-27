option(USE_FFVIDEO "FFmpeg video plugin" ON)
option(USE_MPLAYER "Mplayer video plugin" ON)
option(USE_MPV     "MPV video plugin" ON)

if(USE_FFVIDEO)
    add_subdirectory(ffvideo)
endif(USE_FFVIDEO)

if(USE_MPLAYER)
    add_subdirectory(mplayer)
endif(USE_MPLAYER)

if(USE_MPV)
    add_subdirectory(mpv)
endif(USE_MPV)
