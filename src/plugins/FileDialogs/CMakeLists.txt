set(USE_QMMP_DIALOG TRUE CACHE BOOL "enable/disable Qmmp file dialog")
set(USE_TWO_PANEL_DIALOG TRUE CACHE BOOL "enable/disable Two-panel file dialog")

if(USE_QMMP_DIALOG)
    add_subdirectory(QmmpFileDialog)
endif(USE_QMMP_DIALOG)

if(USE_TWO_PANEL_DIALOG)
    add_subdirectory(TwoPanelFileDialog)
endif(USE_TWO_PANEL_DIALOG)
