set(USE_CURL TRUE CACHE BOOL "enable/disable curl-based http plugin")

if(USE_CURL)
    add_subdirectory(http)
endif(USE_CURL)
