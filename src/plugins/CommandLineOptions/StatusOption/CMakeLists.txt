project(statusoption)

include_directories(${CMAKE_CURRENT_BINARY_DIR})

set(statusoption_SRCS
    statusoption.cpp
    translations/translations.qrc
)

add_library(statusoption MODULE ${statusoption_SRCS})
target_link_libraries(statusoption Qt6::Widgets libqmmpui libqmmp)
install(TARGETS statusoption DESTINATION ${PLUGIN_DIR}/CommandLineOptions)
