set(USE_CURL TRUE CACHE BOOL "enable/disable curl-based http plugin")
set(USE_MMS TRUE CACHE BOOL "enable/disable mms plugin")

if(USE_CURL)
    add_subdirectory(http)
endif(USE_CURL)

if(USE_MMS)
    add_subdirectory(mms)
endif(USE_MMS)
