option(USE_SRC "enable/disable SRC plugin" ON)

if(USE_SRC)
    add_subdirectory(srconverter)
endif(USE_SRC)
