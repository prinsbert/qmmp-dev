set(USE_SKINNED TRUE CACHE BOOL "enable/disable skinned user interface")
set(USE_QSUI TRUE CACHE BOOL "enable/disable simple user interface (qsui)")

if(USE_SKINNED AND SKINNED_AVAILABLE)
    add_subdirectory(skinned)
endif(USE_SKINNED AND SKINNED_AVAILABLE)

if(USE_QSUI)
    add_subdirectory(qsui)
endif(USE_QSUI)
