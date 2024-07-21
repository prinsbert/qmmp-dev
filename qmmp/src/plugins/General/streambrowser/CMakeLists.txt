project(libstreambrowser)

set(libstreambrowser_SRCS
    streambrowser.cpp
    streambrowserfactory.cpp
    streamwindow.cpp
    editstreamdialog.cpp
    streamwindow.ui
    editstreamdialog.ui
    translations/translations.qrc
)

# Don't forget to include output directory, otherwise
# the UI file won't be wrapped!
include_directories(${CMAKE_CURRENT_BINARY_DIR})

add_library(streambrowser MODULE ${libstreambrowser_SRCS})
target_link_libraries(streambrowser PRIVATE Qt6::Widgets Qt6::Network libqmmpui libqmmp)
install(TARGETS streambrowser DESTINATION ${PLUGIN_DIR}/General)
