SET(USE_YTB TRUE CACHE BOOL "enable/disable youtube plugin")

IF(USE_YTB)
add_subdirectory(ytb)
ENDIF(USE_YTB)
