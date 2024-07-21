set(USE_ANALYZER TRUE CACHE BOOL "enable/disable analyzer plugin")
set(USE_PROJECTM TRUE CACHE BOOL "enable/disable projectm plugin")

if(USE_ANALYZER)
    add_subdirectory(analyzer)
endif(USE_ANALYZER)

if(USE_PROJECTM)
    add_subdirectory(projectm)
endif(USE_PROJECTM)
