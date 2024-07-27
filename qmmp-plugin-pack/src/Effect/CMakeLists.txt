set(USE_SRC TRUE CACHE BOOL "enable/disable SRC plugin")

if(USE_SRC)
    add_subdirectory(srconverter)
endif(USE_SRC)
