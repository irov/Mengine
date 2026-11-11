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

if(MENGINE_DEPLOY_PATH)
    if(EXISTS "${MENGINE_DEPLOY_PATH}/local_notifications.plist")
        if(NOT "local_notifications.plist" IN_LIST APPLICATION_APPLE_RESOURCES)
            ADD_MENGINE_APPLE_RESOURCES("local_notifications.plist")
        endif()
    endif()
endif()

if(MENGINE_BUILD_MENGINE_SCRIPT_EMBEDDED)
    TARGET_LINK_LIBRARIES(${PROJECT_NAME} Python)
endif()
