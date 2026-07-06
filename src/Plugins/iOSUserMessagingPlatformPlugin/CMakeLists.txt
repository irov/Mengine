MENGINE_PROJECT(iOSUserMessagingPlatformPlugin)

ADD_FILTER(
src
    iOSUserMessagingPlatformInterface.h

    iOSUserMessagingPlatformPlugin.h
    iOSUserMessagingPlatformPlugin.mm
)

if(MENGINE_BUILD_MENGINE_SCRIPT_EMBEDDED)
    ADD_FILTER(
    embedding
        iOSUserMessagingPlatformScriptEmbedding.h
        iOSUserMessagingPlatformScriptEmbedding.mm
    )

    INCLUDE_DIRECTORIES(${THIRDPARTY_DIR}/pybind/include)
endif()

ADD_MENGINE_PLUGIN(MENGINE_PLUGIN_IOS_USERMESSAGINGPLATFORM)

ADD_MENGINE_APPLE_APPLICATION_DELEGATE("iOSUserMessagingPlatformPlugin")

ADD_MENGINE_COCOAPOD_GOOGLE_UTILITIES()
ADD_MENGINE_COCOAPOD("GoogleUserMessagingPlatform" "NO-GIT" "3.1.0")

if(MENGINE_BUILD_MENGINE_SCRIPT_EMBEDDED)
    TARGET_LINK_LIBRARIES(${PROJECT_NAME} Python)
endif()
