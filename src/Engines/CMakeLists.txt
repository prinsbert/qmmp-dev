option(USE_FFVIDEO "FFmpeg video plugin" ON)

if(USE_FFVIDEO)
    add_subdirectory(ffvideo)
endif(USE_FFVIDEO)
