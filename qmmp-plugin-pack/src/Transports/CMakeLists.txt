option(USE_YTB "enable/disable youtube plugin" ON)
option(USE_MMS "enable/disable mms plugin" ON)

if(USE_YTB)
    add_subdirectory(ytb)
endif(USE_YTB)

if(USE_MMS)
    add_subdirectory(mms)
endif(USE_MMS)
