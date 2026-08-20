MENGINE_PROJECT(iOSAppleSignInPlugin)

ADD_FILTER(
src
    iOSAppleSignInInterface.h

    iOSAppleSignInPlugin.h
    iOSAppleSignInPlugin.mm

    iOSAppleSignIn.entitlements
)

if(MENGINE_BUILD_MENGINE_SCRIPT_EMBEDDED)
    ADD_FILTER(
    embedding
        iOSAppleSignInScriptEmbedding.h
        iOSAppleSignInScriptEmbedding.mm
    )

    INCLUDE_DIRECTORIES(${THIRDPARTY_DIR}/pybind/include)
endif()

ADD_MENGINE_PLUGIN(MENGINE_PLUGIN_IOS_APPLE_SIGNIN)

ADD_MENGINE_APPLE_APPLICATION_DELEGATE("iOSAppleSignInPlugin")
ADD_MENGINE_APPLE_ENTITLEMENTS_PLIST_FILE("${CMAKE_CURRENT_SOURCE_DIR}/iOSAppleSignIn.entitlements")
ADD_MENGINE_APPLE_FRAMEWORK(AuthenticationServices)

if(MENGINE_BUILD_MENGINE_SCRIPT_EMBEDDED)
    TARGET_LINK_LIBRARIES(${PROJECT_NAME} Python)
endif()
