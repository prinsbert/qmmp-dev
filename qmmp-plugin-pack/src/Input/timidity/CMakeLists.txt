project(libtimidity)

# timidity
pkg_search_module(TIMIDITY libtimidity>=0.2.7 IMPORTED_TARGET)

set(libtimidity_SRCS
  decoder_timidity.cpp
  decodertimidityfactory.cpp
  timiditysettingsdialog.cpp
  timidityhelper.cpp
  timiditysettingsdialog.ui
  translations/translations.qrc
)

if(TIMIDITY_FOUND)
    add_library(timidity MODULE ${libtimidity_SRCS})
    target_link_libraries(timidity Qt6::Widgets PkgConfig::QMMP PkgConfig::TIMIDITY)
    install(TARGETS timidity DESTINATION ${PLUGIN_DIR}/Input)
endif(TIMIDITY_FOUND)
