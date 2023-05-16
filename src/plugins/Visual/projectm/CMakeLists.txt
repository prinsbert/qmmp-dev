project(libprojectm)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})

# libqmmp
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../../../)
link_directories(${CMAKE_CURRENT_BINARY_DIR}/../../../qmmp)


# projectM

IF(QT_FEATURE_opengl)
  MESSAGE(STATUS "Found desktop OpenGL implementation")
  # projectM 4.0
  find_package(projectM4 COMPONENTS Playlist)
  IF(projectM4_FOUND)
    ADD_DEFINITIONS(-DPROJECTM_4)
    SET(PROJECTM_FOUND TRUE CACHE INTERNAL "projectm")
    SET(PROJECTM_PRESET_PATH ${CMAKE_INSTALL_DATADIR}/presets CACHE STRING "projectM preset search path")
    SET(PROJECTM_TEXTURE_PATH ${CMAKE_INSTALL_DATADIR}/textures CACHE STRING "projectM texture search path")
    SET(libprojectm_DEFINES ${libprojectm_DEFINES}
        PROJECTM_PRESET_PATH=\"${PROJECTM_PRESET_PATH}\"
        PROJECTM_TEXTURE_PATH=\"${PROJECTM_TEXTURE_PATH}\"
    )
    MESSAGE(STATUS "Found projectM 4.x")
  ELSE(projectM4_FOUND)
  # projectM 2.x/3.x 
    pkg_search_module(PROJECTM libprojectM>=2.0.0)
    IF(PROJECTM_FOUND)
      ADD_DEFINITIONS(${PROJECTM_CFLAGS})
      find_file(PROJECTM_CONFIG config.inp
                PATHS "/usr/share/projectM/" "/usr/local/share/projectM/")
      IF(PROJECTM_VERSION VERSION_GREATER_EQUAL 3.1.0)
        ADD_DEFINITIONS(-DPROJECTM_31)
        MESSAGE(STATUS "Found projectM 3.x")
      ELSE()
        MESSAGE(STATUS "Found projectM 2.x")
      ENDIF()
    ENDIF(PROJECTM_FOUND)
  ENDIF(projectM4_FOUND)
ELSE(QT_FEATURE_opengl)
  MESSAGE(WARNING "Unsuppoted OpenGL implementation, projectM disabled")
ENDIF(QT_FEATURE_opengl)

IF(PROJECTM_CONFIG)
  SET(libprojectm_DEFINES ${libprojectm_DEFINES} PROJECTM_CONFIG=\"${PROJECTM_CONFIG}\")
  MESSAGE(STATUS "Found projectM configuration: ${PROJECTM_CONFIG}")
ENDIF(PROJECTM_CONFIG)

SET(libprojectm_SRCS
    projectmplugin.cpp
    visualprojectmfactory.cpp
    translations/translations.qrc
)

IF(projectM4_FOUND)
    SET(libprojectm_SRCS ${libprojectm_SRCS} projectm4widget.cpp)
ELSE(projectM4_FOUND)
    SET(libprojectm_SRCS ${libprojectm_SRCS} projectmwidget.cpp projectmwrapper.cpp)
ENDIF(projectM4_FOUND)

# Don't forget to include output directory, otherwise
# the UI file won't be wrapped!
include_directories(${CMAKE_CURRENT_BINARY_DIR})
IF(PROJECTM_FOUND)
  ADD_LIBRARY(projectm MODULE ${libprojectm_SRCS})
  
  add_dependencies(projectm libqmmp)
  IF(projectM4_FOUND)
    target_link_libraries(projectm PRIVATE Qt6::OpenGLWidgets libqmmp libprojectM::playlist)
  ELSE(projectM4_FOUND)
    target_compile_definitions(projectm PRIVATE ${libprojectm_DEFINES})
    target_link_libraries(projectm PRIVATE Qt6::OpenGLWidgets libqmmp ${PROJECTM_LDFLAGS} -lGL)
  ENDIF(projectM4_FOUND)
  install(TARGETS projectm DESTINATION ${PLUGIN_DIR}/Visual)
ENDIF(PROJECTM_FOUND)
