MENGINE_PROJECT(iOSUserNotificationCenterPlugin)

ADD_FILTER(
src
    iOSUserNotificationCenterPlugin.h
    iOSUserNotificationCenterPlugin.mm
)

if(MENGINE_BUILD_MENGINE_SCRIPT_EMBEDDED)
    ADD_FILTER(
    embedding
        iOSUserNotificationCenterScriptEmbedding.h
        iOSUserNotificationCenterScriptEmbedding.mm
    )

    INCLUDE_DIRECTORIES(${THIRDPARTY_DIR}/pybind/include)
endif()

ADD_MENGINE_PLUGIN(MENGINE_PLUGIN_IOS_USERNOTIFICATIONCENTER)

ADD_MENGINE_APPLE_APPLICATION_DELEGATE("iOSUserNotificationCenterPlugin")

if(MENGINE_BUILD_MENGINE_SCRIPT_EMBEDDED)
    TARGET_LINK_LIBRARIES(${PROJECT_NAME} Python)
endif()
