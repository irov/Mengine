MENGINE_PROJECT(DebugPanelPlugin)

ADD_FILTER(
src
    DebugPanelPlugin.h
    DebugPanelPlugin.def
    DebugPanelPlugin.cpp
    
    ModuleDebugPanel.cpp
    ModuleDebugPanel.h
)

INCLUDE_DIRECTORIES(${THIRDPARTY_CONFIG_DIR}/imgui)
INCLUDE_DIRECTORIES(${THIRDPARTY_DIR}/imgui)

ADD_MENGINE_PLUGIN(MENGINE_PLUGIN_DEBUGPANEL)

TARGET_LINK_LIBRARIES(${PROJECT_NAME} Kernel)