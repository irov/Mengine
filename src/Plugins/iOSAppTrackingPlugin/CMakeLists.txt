MENGINE_PROJECT(iOSAppTrackingPlugin)

ADD_FILTER(
src
    iOSAppTrackingInterface.h

    iOSAppTrackingPlugin.h
    iOSAppTrackingPlugin.mm
)

if(MENGINE_BUILD_MENGINE_SCRIPT_EMBEDDED)
    ADD_FILTER(
    embedding
        iOSAppTrackingScriptEmbedding.h
        iOSAppTrackingScriptEmbedding.mm
        )

    INCLUDE_DIRECTORIES(${THIRDPARTY_DIR}/pybind/include)
endif()

ADD_MENGINE_PLUGIN(MENGINE_PLUGIN_IOS_APPTRACKING)

ADD_MENGINE_APPLE_APPLICATION_DELEGATE("iOSAppTrackingPlugin")

ADD_MENGINE_APPLE_FRAMEWORK(AppTrackingTransparency)
ADD_MENGINE_APPLE_FRAMEWORK(AdSupport)

if(MENGINE_BUILD_MENGINE_SCRIPT_EMBEDDED)
    TARGET_LINK_LIBRARIES(${PROJECT_NAME} Python)
endif()
