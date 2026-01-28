MENGINE_PROJECT(LayoutEditorPlugin)

ADD_FILTER(
src
    LayoutEditorPlugin.h
    LayoutEditorPlugin.def
    LayoutEditorPlugin.cpp
    
    LayoutEditorModule.cpp
    LayoutEditorModule.h
)

INCLUDE_DIRECTORIES(${THIRDPARTY_CONFIG_DIR}/imgui)
INCLUDE_DIRECTORIES(${THIRDPARTY_DIR}/imgui)

ADD_MENGINE_PLUGIN(MENGINE_PLUGIN_LAYOUTEDITOR)

TARGET_LINK_LIBRARIES(${PROJECT_NAME} Kernel)