project(libgoom)

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
  plugin_info.c
  sound_tester.c
  surf3d.c
  tentacle3d.c
  v3d.c
  visualgoomfactory.cpp
  translations/translations.qrc
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


ADD_LIBRARY(goom MODULE ${libgoom_SRCS})
target_link_libraries(goom Qt6::Widgets ${QMMP_LDFLAGS} ${goom_LDFLAGS})
install(TARGETS goom DESTINATION ${PLUGIN_DIR}/Visual)
