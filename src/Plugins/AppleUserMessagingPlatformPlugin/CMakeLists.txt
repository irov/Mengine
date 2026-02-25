MENGINE_PROJECT(AppleUserMessagingPlatformPlugin)

ADD_FILTER(
src
    AppleUserMessagingPlatformInterface.h

    AppleUserMessagingPlatformPlugin.h
    AppleUserMessagingPlatformPlugin.mm
)

if(MENGINE_BUILD_MENGINE_SCRIPT_EMBEDDED)
    ADD_FILTER(
    embedding
        AppleUserMessagingPlatformScriptEmbedding.h
        AppleUserMessagingPlatformScriptEmbedding.mm
    )
    
    INCLUDE_DIRECTORIES(${THIRDPARTY_DIR}/pybind/include)
endif()

ADD_MENGINE_PLUGIN(MENGINE_PLUGIN_APPLE_USERMESSAGINGPLATFORM)

ADD_MENGINE_APPLE_APPLICATION_DELEGATE("AppleUserMessagingPlatformPlugin")

ADD_MENGINE_COCOAPOD("GoogleUserMessagingPlatform" "NO-GIT" "3.1.0")

if(MENGINE_BUILD_MENGINE_SCRIPT_EMBEDDED)
    TARGET_LINK_LIBRARIES(${PROJECT_NAME} Python)
endif()

