project(libwasapi)

include(CheckIncludeFile)

# Windows Audio Session API
check_include_file(Audioclient.h WASAPI_FOUND)

set(libwasapi_SRCS
  outputwasapi.cpp
  outputwasapifactory.cpp
  wasapisettingsdialog.cpp
  wasapisettingsdialog.ui
  translations/translations.qrc
)

# Don't forget to include output directory, otherwise
# the UI file won't be wrapped!
include_directories(${CMAKE_CURRENT_BINARY_DIR})

if(WASAPI_FOUND)
    add_library(wasapi MODULE ${libwasapi_SRCS})
    target_link_libraries(wasapi PRIVATE Qt5::Widgets libqmmp -lstrmiids -ldmoguids -lmsdmo -lole32 -loleaut32 -luuid -lgdi32 -lksuser)
    install(TARGETS wasapi DESTINATION ${PLUGIN_DIR}/Output)
endif(WASAPI_FOUND)
