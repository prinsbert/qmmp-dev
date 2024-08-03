project(libmms)

# libmms
pkg_search_module(MMS libmms>=0.4 IMPORTED_TARGET)
include_directories(${MMS_INCLUDE_DIRS})

set(libmms_SRCS
    mmsstreamreader.cpp
    mmsinputfactory.cpp
    mmsinputsource.cpp
    mmssettingsdialog.cpp
    mmssettingsdialog.ui
    translations/translations.qrc
)

# Don't forget to include output directory, otherwise
# the UI file won't be wrapped!
include_directories(${CMAKE_CURRENT_BINARY_DIR})

if(MMS_FOUND)
    add_library(mms MODULE ${libmms_SRCS})
    target_link_libraries(mms PRIVATE Qt5::Widgets libqmmp PkgConfig::MMS)
    install(TARGETS mms DESTINATION ${PLUGIN_DIR}/Transports)
endif(MMS_FOUND)
