project(libprojectm)

# projectM 4.x
find_package(projectM4 COMPONENTS Playlist)
if(projectM4_FOUND)
    message(STATUS "Found projectM 4.x")
    set(PROJECTM_FOUND TRUE CACHE INTERNAL "projectm")
    set(PROJECTM_PRESET_PATH ${CMAKE_INSTALL_FULL_DATADIR}/projectM/presets CACHE STRING "projectM preset search path")
    set(PROJECTM_TEXTURE_PATH ${CMAKE_INSTALL_FULL_DATADIR}/projectM/textures CACHE STRING "projectM texture search path")
    option(PROJECTM_FORCE_GLES "force OpenGL ES for projectM" OFF)
    set(libprojectm_DEFINES ${libprojectm_DEFINES}
        PROJECTM_4
        PROJECTM_PRESET_PATH=\"${PROJECTM_PRESET_PATH}\"
        PROJECTM_TEXTURE_PATH=\"${PROJECTM_TEXTURE_PATH}\"
    )
    if(PROJECTM_FORCE_GLES)
        set(libprojectm_DEFINES ${libprojectm_DEFINES} PROJECTM_FORCE_GLES)
    endif()
elseif(${Qt5Gui_OPENGL_IMPLEMENTATION} STREQUAL GL)
    message(STATUS "Found desktop OpenGL implementation")
    # projectM 2.x/3.x
    pkg_search_module(PROJECTM libprojectM>=2.0.0 IMPORTED_TARGET)
    if(PROJECTM_FOUND)
        find_file(PROJECTM_CONFIG config.inp PATHS "/usr/share/projectM/" "/usr/local/share/projectM/")
        if(PROJECTM_VERSION VERSION_GREATER_EQUAL 3.1.0)
            set(libprojectm_DEFINES ${libprojectm_DEFINES} PROJECTM_31)
            message(STATUS "Found projectM 3.x")
        else()
            message(STATUS "Found projectM 2.x")
        endif()
        if(PROJECTM_CONFIG)
            set(libprojectm_DEFINES ${libprojectm_DEFINES} PROJECTM_CONFIG=\"${PROJECTM_CONFIG}\")
            message(STATUS "Found projectM configuration: ${PROJECTM_CONFIG}")
        endif()
    endif()
else()
    message(WARNING "Unsuppoted OpenGL implementation, projectM disabled")
endif()

set(libprojectm_SRCS
    projectmplugin.cpp
    visualprojectmfactory.cpp
    translations/translations.qrc
)

if(projectM4_FOUND)
    set(libprojectm_SRCS ${libprojectm_SRCS} projectm4window.cpp)
else(projectM4_FOUND)
    set(libprojectm_SRCS ${libprojectm_SRCS} projectmwidget.cpp projectmwrapper.cpp)
endif(projectM4_FOUND)

# Don't forget to include output directory, otherwise
# the UI file won't be wrapped!
include_directories(${CMAKE_CURRENT_BINARY_DIR})

if(PROJECTM_FOUND)
    add_library(projectm MODULE ${libprojectm_SRCS})
    target_compile_definitions(projectm PRIVATE ${libprojectm_DEFINES})

    if(projectM4_FOUND)
        target_link_libraries(projectm PRIVATE Qt5::Widgets libqmmp libprojectM::playlist)
    else(projectM4_FOUND)
        target_link_libraries(projectm PRIVATE Qt5::Widgets libqmmp PkgConfig::PROJECTM)
        if(WIN32)
            target_link_libraries(projectm PRIVATE -lopengl32)
        else(WIN32)
            target_link_libraries(projectm PRIVATE -lGL)
        endif(WIN32)
    endif(projectM4_FOUND)
    install(TARGETS projectm DESTINATION ${PLUGIN_DIR}/Visual)
endif(PROJECTM_FOUND)
