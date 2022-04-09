MENGINE_PROJECT(AppleGameCenterPlugin)

ADD_FILTER(
src
    AppleGameCenterInterface.h

    AppleGameCenterPlugin.h
    AppleGameCenterPlugin.cpp
    AppleGameCenterPlugin.def
    
    AppleGameCenterService.h
    AppleGameCenterService.mm
    
    AppleGameCenterNative.h
    AppleGameCenterNative.m
)

if(MENGINE_USE_SCRIPT_SERVICE)
    ADD_FILTER(
    embedding
        AppleGameCenterScriptEmbedding.h
        AppleGameCenterScriptEmbedding.cpp
        )
        
    INCLUDE_DIRECTORIES(${THIRDPARTY_DIR}/pybind/include)
endif()

ADD_MENGINE_PLUGIN(MENGINE_PLUGIN_APPLE_GAMECENTER)

ADD_APPLE_FRAMEWORK(GameKit)

if(MENGINE_USE_SCRIPT_SERVICE)
    TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${THIRDPARTY_LIB_DIR}/${MENGINE_LIB_PREFIX}pybind${MENGINE_LIB_SUFFIX})
endif()
