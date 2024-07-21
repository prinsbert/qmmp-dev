project(libpipewire)

# pipewire
pkg_check_modules(PIPEWIRE libpipewire-0.3>=0.3.26 libspa-0.2>=0.2 IMPORTED_TARGET)

set(libpipewire_SRCS
  outputpipewire.cpp
  outputpipewirefactory.cpp
  translations/translations.qrc
)

# Don't forget to include output directory, otherwise
# the UI file won't be wrapped!
include_directories(${CMAKE_CURRENT_BINARY_DIR})

add_compile_options(-Wno-missing-field-initializers)

if(PIPEWIRE_FOUND)
    add_library(pipewire MODULE ${libpipewire_SRCS})
    add_dependencies(pipewire libqmmp)
    target_link_libraries(pipewire PRIVATE Qt6::Widgets libqmmp PkgConfig::PIPEWIRE)
    install(TARGETS pipewire DESTINATION ${PLUGIN_DIR}/Output)
endif(PIPEWIRE_FOUND)
