project(libqtmultimedia)

SET(libqtmultimedia_SRCS
  outputqtmultimedia.cpp
  outputqtmultimediafactory.cpp
  qtmultimediasettingsdialog.cpp
  qtmultimediasettingsdialog.ui
  translations/translations.qrc
)

# Don't forget to include output directory, otherwise
# the UI file won't be wrapped!
include_directories(${CMAKE_CURRENT_BINARY_DIR})

add_library(qtmultimedia MODULE ${libqtmultimedia_SRCS} ${libqtmultimedia_UIS_H} ${libqtmultimedia_RCC_SRCS})
target_link_libraries(qtmultimedia PRIVATE Qt5::Widgets Qt5::Multimedia libqmmp)
install(TARGETS qtmultimedia DESTINATION ${PLUGIN_DIR}/Output)
