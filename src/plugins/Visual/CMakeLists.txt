option(USE_ANALYZER "enable/disable analyzer plugin" ON)
option(USE_PROJECTM "enable/disable projectm plugin" ON)

if(USE_ANALYZER)
    add_subdirectory(analyzer)
endif(USE_ANALYZER)

if(USE_PROJECTM)
    add_subdirectory(projectm)
endif(USE_PROJECTM)
