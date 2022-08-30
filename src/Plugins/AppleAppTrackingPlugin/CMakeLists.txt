MENGINE_PROJECT(AppleAppTrackingPlugin)

ADD_FILTER(
src
    AppleAppTrackingInterface.h

    AppleAppTrackingPlugin.h
    AppleAppTrackingPlugin.cpp
    
    AppleAppTrackingService.h
    AppleAppTrackingService.mm
)

if(MENGINE_USE_SCRIPT_SERVICE)
    ADD_FILTER(
    embedding
        AppleAppTrackingScriptEmbedding.h
        AppleAppTrackingScriptEmbedding.cpp
        )
        
    INCLUDE_DIRECTORIES(${THIRDPARTY_DIR}/pybind/include)    
endif()

ADD_MENGINE_PLUGIN(MENGINE_PLUGIN_APPLE_APPTRACKING)

ADD_MENGINE_APPLE_FRAMEWORK(AppTrackingTransparency)
ADD_MENGINE_APPLE_FRAMEWORK(AdSupport)

if(MENGINE_USE_SCRIPT_SERVICE)
    TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${THIRDPARTY_LIB_DIR}/${MENGINE_LIB_PREFIX}pybind${MENGINE_LIB_SUFFIX})
endif()
