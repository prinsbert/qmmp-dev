SET(USE_GOOM TRUE CACHE BOOL "enable/disable goom plugin")

IF(USE_GOOM)
add_subdirectory(goom)
ENDIF(USE_GOOM)
