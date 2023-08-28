MENGINE_PROJECT(AppleGameCenterPlugin)

ADD_FILTER(
src
    AppleGameCenterInterface.h

    AppleGameCenterPlugin.h
    AppleGameCenterPlugin.mm
    
    AppleGameCenterService.h
    AppleGameCenterService.mm
    
    AppleGameCenterDelegate.h
    AppleGameCenterDelegate.mm
)

if(MENGINE_USE_SCRIPT_SERVICE)
    ADD_FILTER(
    embedding
        AppleGameCenterScriptEmbedding.h
        AppleGameCenterScriptEmbedding.mm
        )
        
    INCLUDE_DIRECTORIES(${THIRDPARTY_DIR}/pybind/include)
endif()

ADD_MENGINE_APPLE_PLUGIN(MENGINE_PLUGIN_APPLE_GAMECENTER)

ADD_MENGINE_APPLE_FRAMEWORK(GameKit)

if(MENGINE_USE_SCRIPT_SERVICE)
    TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${THIRDPARTY_LIB_DIR}/${MENGINE_LIB_PREFIX}pybind${MENGINE_LIB_SUFFIX})
endif()
