MENGINE_PROJECT(iOSFacebookPlugin)

ADD_FILTER(
src
    iOSFacebookInterface.h

    iOSFacebookPlugin.h
    iOSFacebookPlugin.mm

    iOSFacebookShareDelegate.h
    iOSFacebookShareDelegate.mm
)

if(MENGINE_BUILD_MENGINE_SCRIPT_EMBEDDED)
    ADD_FILTER(
    embedding
        iOSFacebookScriptEmbedding.h
        iOSFacebookScriptEmbedding.mm
    )

    INCLUDE_DIRECTORIES(${THIRDPARTY_DIR}/pybind/include)
endif()

ADD_MENGINE_PLUGIN(MENGINE_PLUGIN_IOS_FACEBOOK)

ADD_MENGINE_SWIFT_PACKAGE("FacebookCore" "https://github.com/facebook/facebook-ios-sdk.git" "18.1.1")
ADD_MENGINE_SWIFT_PACKAGE("FacebookLogin" "https://github.com/facebook/facebook-ios-sdk.git" "18.1.1")
ADD_MENGINE_SWIFT_PACKAGE("FacebookShare" "https://github.com/facebook/facebook-ios-sdk.git" "18.1.1")

ADD_MENGINE_APPLE_APPLICATION_DELEGATE("iOSFacebookPlugin")

if(MENGINE_BUILD_MENGINE_SCRIPT_EMBEDDED)
    TARGET_LINK_LIBRARIES(${PROJECT_NAME} Python)
endif()
