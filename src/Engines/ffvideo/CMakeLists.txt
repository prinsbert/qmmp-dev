project(libffvideo)

# fixes ffmpeg defines
add_definitions(-D__STDC_CONSTANT_MACROS)

# minimal supported version
# ffmpeg-3.4: libavcodec-57.107.100, libavformat-57.83.100, libavutil-55.78.100, libswscale-4.8.100
pkg_check_modules(FFMPEG libavcodec>=57.107.0 libavformat>=57.83.0 libavutil>=55.78.0 libswscale>=4.8.0 libswresample IMPORTED_TARGET)

SET(libffvideo_SRCS
  audiothread.cpp
  ffmpegengine.cpp
  ffvideodecoder.cpp
  ffvideofactory.cpp
  ffvideometadatamodel.cpp
  packetbuffer.cpp
  videothread.cpp
  videowindow.cpp
  translations/translations.qrc
)

add_compile_options(-Wno-missing-field-initializers)

if(FFMPEG_FOUND)
    ADD_LIBRARY(ffvideo MODULE ${libffvideo_SRCS} ${libffvideo_RCC_SRCS} ${libffvideo_HDRS})
    target_link_libraries(ffvideo PRIVATE Qt6::Widgets PkgConfig::QMMP PkgConfig::FFMPEG)
    install(TARGETS ffvideo DESTINATION ${PLUGIN_DIR}/Engines)
endif(FFMPEG_FOUND)
