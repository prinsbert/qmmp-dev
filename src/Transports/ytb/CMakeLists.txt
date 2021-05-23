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
)

SET(libytb_RCCS translations/translations.qrc)

QT5_ADD_RESOURCES(libytb_RCC_SRCS ${libytb_RCCS})

# Don't forget to include output directory, otherwise
# the UI file won't be wrapped!
include_directories(${CMAKE_CURRENT_BINARY_DIR})

ADD_LIBRARY(ytb MODULE ${libytb_SRCS} ${libytb_UIS_H} ${libytb_RCC_SRCS} ${libytb_HDRS})
target_link_libraries(ytb Qt5::Widgets Qt5::Network ${QMMP_LDFLAGS} ${XMP_LDFLAGS})
install(TARGETS ytb DESTINATION ${PLUGIN_DIR}/Transports)
