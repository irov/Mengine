MENGINE_PROJECT(AppleGameCenterPlugin)

ADD_FILTER(
src
    AppleGameCenterPlugin.h
    AppleGameCenterPlugin.mm
    
    AppleGameCenterInterface.h
    
    AppleGameCenterApplicationDelegate.h
    AppleGameCenterApplicationDelegate.mm
)

if(MENGINE_BUILD_MENGINE_SCRIPT_EMBEDDED)
    ADD_FILTER(
    embedding
        AppleGameCenterScriptEmbedding.h
        AppleGameCenterScriptEmbedding.mm
        )
        
    INCLUDE_DIRECTORIES(${THIRDPARTY_DIR}/pybind/include)
endif()

ADD_MENGINE_PLUGIN(MENGINE_PLUGIN_APPLE_GAMECENTER)

ADD_MENGINE_APPLE_APPLICATION_DELEGATE("AppleGameCenterApplicationDelegate")

ADD_MENGINE_APPLE_FRAMEWORK(GameKit)

if(MENGINE_BUILD_MENGINE_SCRIPT_EMBEDDED)
    TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${THIRDPARTY_LIB_DIR}/${MENGINE_LIB_PREFIX}pybind${MENGINE_LIB_SUFFIX})
endif()
