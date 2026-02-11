MENGINE_PROJECT(DevToDebugPlugin)

ADD_FILTER(
src
    DevToDebugInterface.h

    DevToDebugPlugin.h
    DevToDebugPlugin.def
    DevToDebugPlugin.cpp
    
    DevToDebugService.h
    DevToDebugService.cpp
    
    DevToDebugTab.h
    DevToDebugTab.cpp
    
    DevToDebugProperty.h
    DevToDebugProperty.cpp
    
    DevToDebugPropertyConstBoolean.h
    DevToDebugPropertyConstBoolean.cpp

    DevToDebugPropertyGetterBoolean.h
    DevToDebugPropertyGetterBoolean.cpp
    
    DevToDebugPropertyConstString.h
    DevToDebugPropertyConstString.cpp
    
    DevToDebugPropertyGetterString.h
    DevToDebugPropertyGetterString.cpp
    
    DevToDebugPropertyConstColor.h
    DevToDebugPropertyConstColor.cpp
    
    DevToDebugPropertyGetterColor.h
    DevToDebugPropertyGetterColor.cpp

    DevToDebugPropertyContent.h
    DevToDebugPropertyContent.cpp
    
    DevToDebugWidget.h
    DevToDebugWidget.cpp
    
    DevToDebugWidgetButton.h
    DevToDebugWidgetButton.cpp
    
    DevToDebugWidgetText.h
    DevToDebugWidgetText.cpp
    
    DevToDebugWidgetCheckbox.h
    DevToDebugWidgetCheckbox.cpp
    
    DevToDebugWidgetCommandLine.h
    DevToDebugWidgetCommandLine.cpp
    
    DevToDebugWidgetRadioButton.h
    DevToDebugWidgetRadioButton.cpp
    
    DevToDebugWidgetSelector.h
    DevToDebugWidgetSelector.cpp
)

if(MENGINE_BUILD_MENGINE_SCRIPT_EMBEDDED)
    ADD_FILTER(
    embedding
        DevToDebugScriptEmbedding.cpp
        DevToDebugScriptEmbedding.h
    )

    INCLUDE_DIRECTORIES(${THIRDPARTY_DIR}/pybind/include)
endif()

INCLUDE_DIRECTORIES(${THIRDPARTY_DIR}/jpp/include)

ADD_MENGINE_PLUGIN(MENGINE_PLUGIN_DEVTODEBUG)

TARGET_LINK_LIBRARIES(${PROJECT_NAME} Kernel)

if(MENGINE_BUILD_MENGINE_SCRIPT_EMBEDDED)
    TARGET_LINK_LIBRARIES(${PROJECT_NAME} Python)
endif()