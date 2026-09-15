MENGINE_PROJECT(DebugPanelPlugin)

ADD_FILTER(
src
    DebugPanelPlugin.h
    DebugPanelPlugin.def
    DebugPanelPlugin.cpp
    
    DebugPanelModule.cpp
    DebugPanelModule.h
)

INCLUDE_DIRECTORIES(SYSTEM ${THIRDPARTY_DIR}/mosaic/include)

ADD_MENGINE_PLUGIN(MENGINE_PLUGIN_DEBUGPANEL)

TARGET_LINK_LIBRARIES(${PROJECT_NAME} Kernel)
TARGET_LINK_LIBRARIES(${PROJECT_NAME} MosaicFramework)