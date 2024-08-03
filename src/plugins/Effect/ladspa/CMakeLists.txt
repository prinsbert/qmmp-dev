project(libladspa)

set(libladspa_SRCS
    ladspahost.cpp
    ladspahelper.cpp
    ladspaslider.cpp
    ladspabutton.cpp
    ladspasettingsdialog.cpp
    effectladspafactory.cpp
    ladspasettingsdialog.ui
    translations/translations.qrc
)

# Don't forget to include output directory, otherwise
# the UI file won't be wrapped!
include_directories(${CMAKE_CURRENT_BINARY_DIR})

add_library(ladspa MODULE ${libladspa_SRCS})
target_link_libraries(ladspa PRIVATE Qt5::Widgets libqmmp ${CMAKE_DL_LIBS})
install(TARGETS ladspa DESTINATION ${PLUGIN_DIR}/Effect)

