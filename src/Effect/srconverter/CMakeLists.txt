project(libsrconverter)

pkg_check_modules(SAMPLERATE samplerate IMPORTED_TARGET)

SET(libsrconverter_SRCS
    srconverter.cpp
    srconvertersettingsdialog.cpp
    effectsrconverterfactory.cpp
    srconvertersettingsdialog.ui
    translations/translations.qrc
)

if(SAMPLERATE_FOUND)
    add_library(srconverter MODULE ${libsrconverter_SRCS})
    target_link_libraries(srconverter PRIVATE Qt6::Widgets PkgConfig::QMMP PkgConfig::SAMPLERATE)
    install(TARGETS srconverter DESTINATION ${PLUGIN_DIR}/Effect)
endif(SAMPLERATE_FOUND)
