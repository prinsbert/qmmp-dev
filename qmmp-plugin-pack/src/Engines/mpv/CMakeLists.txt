project(libmpv)

# fixes ffmpeg defines
add_definitions(-D__STDC_CONSTANT_MACROS)

# minimal supported version
# ffmpeg-3.4: libavcodec-57.107.100, libavformat-57.83.100, libavutil-55.78.100
pkg_check_modules(MPV mpv libavcodec>=57.107.0 libavformat>=57.83.0 libavutil>=55.78.0 IMPORTED_TARGET)

SET(libmpv_SRCS
  mpvengine.cpp
  mpvenginefactory.cpp
  mpvmetadatamodel.cpp
  translations/translations.qrc
)

add_compile_options(-Wno-missing-field-initializers)

if(MPV_FOUND)
    add_library(mpv MODULE ${libmpv_SRCS} ${libmpv_RCC_SRCS} ${libmpv_HDRS})
    target_link_libraries(mpv PRIVATE Qt6::Widgets PkgConfig::QMMP PkgConfig::MPV)
    install(TARGETS mpv DESTINATION ${PLUGIN_DIR}/Engines)
endif(MPV_FOUND)
