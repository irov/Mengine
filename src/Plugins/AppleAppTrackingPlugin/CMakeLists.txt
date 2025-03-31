MENGINE_PROJECT(AppleAppTrackingPlugin)

ADD_FILTER(
src
    AppleAppTrackingInterface.h

    AppleAppTrackingPlugin.h
    AppleAppTrackingPlugin.mm
    
    AppleAppTrackingApplicationDelegate.h
    AppleAppTrackingApplicationDelegate.mm
)

if(MENGINE_BUILD_MENGINE_SCRIPT_EMBEDDED)
    ADD_FILTER(
    embedding
        AppleAppTrackingScriptEmbedding.h
        AppleAppTrackingScriptEmbedding.mm
        )
        
    INCLUDE_DIRECTORIES(${THIRDPARTY_DIR}/pybind/include)
endif()

ADD_MENGINE_PLUGIN(MENGINE_PLUGIN_APPLE_APPTRACKING)

ADD_MENGINE_APPLE_APPLICATION_DELEGATE("AppleAppTrackingApplicationDelegate")

ADD_MENGINE_APPLE_FRAMEWORK(AppTrackingTransparency)
ADD_MENGINE_APPLE_FRAMEWORK(AdSupport)

if(MENGINE_BUILD_MENGINE_SCRIPT_EMBEDDED)
    TARGET_LINK_LIBRARIES(${PROJECT_NAME} Python)
endif()
