set(USE_GOOM TRUE CACHE BOOL "enable/disable goom plugin")

if(USE_GOOM)
    add_subdirectory(goom)
endif(USE_GOOM)
