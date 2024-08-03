project(libytb)

SET(libytb_SRCS
  bufferdevice.cpp
  ytbinputfactory.cpp
  ytbinputsource.cpp
  translations/translations.qrc
)

ADD_LIBRARY(ytb MODULE ${libytb_SRCS})
target_link_libraries(ytb PRIVATE Qt5::Widgets Qt5::Network PkgConfig::QMMP)
install(TARGETS ytb DESTINATION ${PLUGIN_DIR}/Transports)
