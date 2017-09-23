project(libgoom)

# qt plugin
ADD_DEFINITIONS(${QT_DEFINITIONS})
ADD_DEFINITIONS(-DQT_PLUGIN)
ADD_DEFINITIONS(-DQT_NO_DEBUG)
ADD_DEFINITIONS(-DQT_SHARED)
ADD_DEFINITIONS(-DQT_THREAD)
ADD_DEFINITIONS(-DYY_NO_INPUT)

include_directories(${CMAKE_CURRENT_BINARY_DIR})

# libqmmp
include_directories(${QMMP_INCLUDE_DIRS})
link_directories( ${QMMP_LIBRARY_DIRS})

SET(libgoom_SRCS
  config_param.c
  convolve_fx.c
  cpu_info.c
  drawmethods.c
  filters.c
  flying_stars_fx.c
  gfontlib.c
  gfontrle.c
  goom_core.c
  goomsl.c
  goomsl_hash.c
  goomsl_heap.c
  goomsl_lex.c
  goomsl_yacc.c
  goom_tools.c
  goomwidget.cpp
  graphic.c
  ifs.c
  lines.c
  mathtools.c
  plugin_info.c
  sound_tester.c
  surf3d.c
  tentacle3d.c
  v3d.c
  visualgoomfactory.cpp
)

SET(libgoom_HDRS
  attributes.h
  cpu_info.h
  default_scripts.h
  drawmethods.h
  gfontlib.h
  goom_config.h
  goom_config_param.h
  goom_filters.h
  goom_fx.h
  goom_graphic.h
  goom.h
  goom_plugin_info.h
  goomsl.h
  goomsl_hash.h
  goomsl_heap.h
  goomsl_private.h
  goomsl_yacc.h
  goom_tools.h
  goom_typedefs.h
  goom_visual_fx.h
  goomwidget.h
  ifs.h
  lines.h
  mathtools.h
  motif_goom1.h
  motif_goom2.h
  sound_tester.h
  surf3d.h
  tentacle3d.h
  v3d.h
  visualgoomfactory.h
)

IF(CMAKE_SYSTEM_PROCESSOR MATCHES "^(i.86)$")
  ADD_DEFINITIONS(-DCPU_X86)
  ADD_DEFINITIONS(-DARCH_X86_32)
  message("-- Architecture: x86")
  SET(GOOM_ASM TRUE CACHE INTERNAL "goom_asm")
  SET(libgoom_HDRS ${libgoom_HDRS} mmx.h)
  SET(libgoom_SRCS ${libgoom_SRCS} mmx.c xmmx.c)
ELSEIF(CMAKE_SYSTEM_PROCESSOR STREQUAL "x86_64")
  ADD_DEFINITIONS(-DCPU_X86)
  ADD_DEFINITIONS(-DARCH_X86_64)
  message("-- Architecture: x86_64")
  SET(GOOM_ASM TRUE CACHE INTERNAL "goom_asm")
  SET(libgoom_HDRS ${libgoom_HDRS} mmx.h)
  SET(libgoom_SRCS ${libgoom_SRCS} mmx.c xmmx.c)
ELSE(CMAKE_SYSTEM_PROCESSOR MATCHES "^(i.86)$")
  message("-- Assembler optimizations are disabled")
ENDIF(CMAKE_SYSTEM_PROCESSOR MATCHES "^(i.86)$")


SET(libgoom_RCCS translations/translations.qrc)

QT4_ADD_RESOURCES(libgoom_RCC_SRCS ${libgoom_RCCS})

# Don't forget to include output directory, otherwise
# the UI file won't be wrapped!
include_directories(${CMAKE_CURRENT_BINARY_DIR})


ADD_LIBRARY(goom MODULE ${libgoom_SRCS} ${libgoom_RCC_SRCS} ${libgoom_HDRS})
target_link_libraries(goom ${QT_LIBRARIES} ${QMMP_LDFLAGS} ${goom_LDFLAGS})
install(TARGETS goom DESTINATION ${LIB_DIR}/qmmp/Visual)
