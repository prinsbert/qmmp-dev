include(../../plugins.pri)

TARGET = $$PLUGINS_PREFIX/Visual/goom

CONFIG += warn_on \
    plugin \
    link_pkgconfig
TEMPLATE = lib

#RESOURCES = translations/translations.qrc

DEFINES += YY_NO_INPUT CPU_X86 ARCH_X86 HAVE_MMX

QMAKE_CFLAGS += -Werror=implicit-function-declaration

unix {
    isEmpty(LIB_DIR):LIB_DIR = /lib
    target.path = $$LIB_DIR/qmmp/Visual
    INSTALLS += target

    QMAKE_LIBDIR += ../../../../lib
    LIBS += -lqmmp
    QMAKE_CLEAN = $$PLUGINS_PREFIX/Visual/libgoom.so
}

win32 {
    LIBS += -lqmmp0
}

HEADERS += \
    goomwidget.h \
    visualgoomfactory.h \
    cpu_info.h \
    default_scripts.h \
    drawmethods.h \
    gfontlib.h \
    goom.h \
    goom_config.h \
    goom_config_param.h \
    goom_filters.h \
    goom_fx.h \
    goom_graphic.h \
    goom_plugin_info.h \
    goomsl.h \
    goomsl_hash.h \
    goomsl_heap.h \
    goomsl_private.h \
    goomsl_yacc.h \
    goom_tools.h \
    goom_typedefs.h \
    goom_visual_fx.h \
    ifs.h \
    lines.h \
    mathtools.h \
    mmx.h \
    motif_goom1.h \
    motif_goom2.h \
    ppc_drawings.h \
    ppc_zoom_ultimate.h \
    sound_tester.h \
    surf3d.h \
    tentacle3d.h \
    v3d.h

SOURCES += \
    goomwidget.cpp \
    visualgoomfactory.cpp \
    config_param.c \
    convolve_fx.c \
    cpu_info.c \
    drawmethods.c \
    filters.c \
    flying_stars_fx.c \
    gfontlib.c \
    gfontrle.c \
    goom_core.c \
    goomsl.c \
    goomsl_hash.c \
    goomsl_heap.c \
    goomsl_lex.c \
    goomsl_yacc.c \
    goom_tools.c \
    graphic.c \
    ifs.c \
    lines.c \
    mathtools.c \
    mmx.c \
    plugin_info.c \
    sound_tester.c \
    surf3d.c \
    tentacle3d.c \
    v3d.c \
    xmmx.c

