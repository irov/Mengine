MENGINE_PROJECT(iOSGameCenterPlugin)

ADD_FILTER(
src
    iOSGameCenterInterface.h

    iOSGameCenterPlugin.h
    iOSGameCenterPlugin.mm
)

if(MENGINE_BUILD_MENGINE_SCRIPT_EMBEDDED)
    ADD_FILTER(
    embedding
        iOSGameCenterScriptEmbedding.h
        iOSGameCenterScriptEmbedding.mm
        )

    INCLUDE_DIRECTORIES(${THIRDPARTY_DIR}/pybind/include)
endif()

ADD_MENGINE_PLUGIN(MENGINE_PLUGIN_IOS_GAMECENTER)

ADD_MENGINE_APPLE_APPLICATION_DELEGATE("iOSGameCenterPlugin")

ADD_MENGINE_APPLE_FRAMEWORK(GameKit)

if(MENGINE_BUILD_MENGINE_SCRIPT_EMBEDDED)
    TARGET_LINK_LIBRARIES(${PROJECT_NAME} Python)
endif()
