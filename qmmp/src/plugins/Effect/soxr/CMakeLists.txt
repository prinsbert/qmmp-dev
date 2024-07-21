project(libsoxr)

pkg_check_modules(SOXR soxr IMPORTED_TARGET)

set(libsoxr_SRCS
    soxresampler.cpp
    soxrsettingsdialog.cpp
    effectsoxrfactory.cpp
    soxrsettingsdialog.ui
    translations/translations.qrc
)

# Don't forget to include output directory, otherwise
# the UI file won't be wrapped!
include_directories(${CMAKE_CURRENT_BINARY_DIR})

if(SOXR_FOUND)
    add_library(soxr MODULE ${libsoxr_SRCS})
    add_dependencies(soxr libqmmp)
    target_link_libraries(soxr PRIVATE Qt6::Widgets libqmmp PkgConfig::SOXR)
    install(TARGETS soxr DESTINATION ${PLUGIN_DIR}/Effect)
endif(SOXR_FOUND)
