option(USE_GOOM "enable/disable goom plugin" ON)

if(USE_GOOM)
    add_subdirectory(goom)
endif(USE_GOOM)
