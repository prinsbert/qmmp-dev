SET(USE_FFVIDEO TRUE CACHE BOOL "enable/disable FFmpeg video plugin")

IF(USE_FFVIDEO)
add_subdirectory(ffvideo)
ENDIF(USE_FFVIDEO)
