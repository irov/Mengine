MENGINE_PROJECT(LayoutEditorPlugin)

ADD_FILTER(
src
    LayoutEditorPlugin.h
    LayoutEditorPlugin.def
    LayoutEditorPlugin.cpp
    
    LayoutEditorWidgetType.h
    
    LayoutEditorLogger.cpp
    LayoutEditorLogger.h
    
    LayoutEditorModule.cpp
    LayoutEditorModule.h
)

if(MENGINE_BUILD_MENGINE_SCRIPT_EMBEDDED)
    ADD_FILTER(
    embedding
        LayoutEditorScriptEmbedding.cpp
        LayoutEditorScriptEmbedding.h
    )
        
    INCLUDE_DIRECTORIES(${THIRDPARTY_DIR}/pybind/include)
endif()

INCLUDE_DIRECTORIES(${THIRDPARTY_CONFIG_DIR}/imgui)
INCLUDE_DIRECTORIES(${THIRDPARTY_DIR}/imgui)

ADD_MENGINE_PLUGIN(MENGINE_PLUGIN_LAYOUTEDITOR)

TARGET_LINK_LIBRARIES(${PROJECT_NAME} Kernel)