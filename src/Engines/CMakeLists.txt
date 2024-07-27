set(USE_FFVIDEO TRUE CACHE BOOL "enable/disable FFmpeg video plugin")

if(USE_FFVIDEO)
    add_subdirectory(ffvideo)
endif(USE_FFVIDEO)
