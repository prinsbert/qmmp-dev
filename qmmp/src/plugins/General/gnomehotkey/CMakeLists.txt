project(libgnomehotkey)

set(libgnomehotkey_SRCS
    gnomehotkeyfactory.cpp
    mediakeys.cpp
    translations/translations.qrc
)

# Don't forget to include output directory, otherwise
# the UI file won't be wrapped!
include_directories(${CMAKE_CURRENT_BINARY_DIR})

add_library(gnomehotkey MODULE ${libgnomehotkey_SRCS} ${libgnomehotkey_RCC_SRCS})
target_link_libraries(gnomehotkey PRIVATE Qt6::Widgets Qt6::DBus libqmmpui libqmmp)
install(TARGETS gnomehotkey DESTINATION ${PLUGIN_DIR}/General)
