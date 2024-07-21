project(libxmp)

# libqmmp
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../../../)
link_directories(${CMAKE_CURRENT_BINARY_DIR}/../../../qmmp)

# xmp
pkg_search_module(XMP libxmp IMPORTED_TARGET)

SET(libxmp_SRCS
  decoder_xmp.cpp
  decoderxmpfactory.cpp
  xmpmetadatamodel.cpp
  xmpsettingsdialog.cpp
  xmpmetadatamodel.cpp
  xmpsettingsdialog.ui
  translations/translations.qrc
)

# Don't forget to include output directory, otherwise
# the UI file won't be wrapped!
include_directories(${CMAKE_CURRENT_BINARY_DIR})

if(XMP_FOUND)
    add_library(xmp MODULE ${libxmp_SRCS} ${libxmp_UIS_H})
    target_link_libraries(xmp PRIVATE Qt6::Widgets libqmmp PkgConfig::XMP)
    install(TARGETS xmp DESTINATION ${PLUGIN_DIR}/Input)
endif(XMP_FOUND)
