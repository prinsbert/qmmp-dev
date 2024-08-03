project(seekoption)

include_directories(${CMAKE_CURRENT_BINARY_DIR})

SET(seekoption_SRCS
    seekoption.cpp
    translations/translations.qrc
)

add_library(seekoption MODULE ${seekoption_SRCS})
target_link_libraries(seekoption PRIVATE Qt5::Widgets libqmmpui libqmmp)
install(TARGETS seekoption DESTINATION ${PLUGIN_DIR}/CommandLineOptions)
