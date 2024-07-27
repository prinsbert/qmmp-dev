set(USE_YTB TRUE CACHE BOOL "enable/disable youtube plugin")

if(USE_YTB)
    add_subdirectory(ytb)
endif(USE_YTB)
