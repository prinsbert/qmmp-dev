project(libytb)

# libqmmp
include_directories(${QMMP_INCLUDE_DIRS})
link_directories(${QMMP_LIBRARY_DIRS})

include_directories(${XMP_INCLUDE_DIRS})
link_directories(${XMP_LIBRARY_DIRS})
ADD_DEFINITIONS(${XMP_CFLAGS})

SET(libytb_SRCS
  bufferdevice.cpp
  ytbinputfactory.cpp
  ytbinputsource.cpp
  translations/translations.qrc
)

ADD_LIBRARY(ytb MODULE ${libytb_SRCS})
target_link_libraries(ytb PRIVATE Qt6::Widgets Qt6::Network ${QMMP_LDFLAGS} ${XMP_LDFLAGS})
install(TARGETS ytb DESTINATION ${PLUGIN_DIR}/Transports)
