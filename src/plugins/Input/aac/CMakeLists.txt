project(libaac)

# libfaad
pkg_search_module(FAAD faad2>=2.9 IMPORTED_TARGET)

set(libaac_SRCS
  decoder_aac.cpp
  decoderaacfactory.cpp
  aacfile.cpp
  translations/translations.qrc
)

# Don't forget to include output directory, otherwise
# the UI file won't be wrapped!
include_directories(${CMAKE_CURRENT_BINARY_DIR})

if(FAAD_FOUND)
    add_library(aac MODULE ${libaac_SRCS} ${libaac_RCC_SRCS})
    target_link_libraries(aac PRIVATE Qt6::Widgets libqmmp PkgConfig::FAAD PkgConfig::TAGLIB)
    install(TARGETS aac DESTINATION ${PLUGIN_DIR}/Input)
endif(FAAD_FOUND)
