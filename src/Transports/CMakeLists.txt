set(USE_YTB TRUE CACHE BOOL "enable/disable youtube plugin")
set(USE_MMS TRUE CACHE BOOL "enable/disable mms plugin")

if(USE_YTB)
    add_subdirectory(ytb)
endif(USE_YTB)

if(USE_MMS)
    add_subdirectory(mms)
endif(USE_MMS)
