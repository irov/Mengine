MENGINE_PROJECT(AppleFacebookPlugin)

ADD_FILTER(
src
    AppleFacebookPlugin.h
    AppleFacebookPlugin.mm

    AppleFacebookInterface.h

    AppleFacebookApplicationDelegate.h
    AppleFacebookApplicationDelegate.mm

    AppleFacebookService.h
    AppleFacebookService.mm

    AppleFacebookShareDelegate.h
    AppleFacebookShareDelegate.mm
)

if(MENGINE_USE_SCRIPT_SERVICE)
    ADD_FILTER(
    embedding
        AppleFacebookScriptEmbedding.h
        AppleFacebookScriptEmbedding.mm
    )
    
    INCLUDE_DIRECTORIES(${THIRDPARTY_DIR}/pybind/include)
endif()

ADD_MENGINE_APPLE_PLUGIN(MENGINE_PLUGIN_APPLE_FACEBOOK)

ADD_MENGINE_COCOAPOD("FBSDKCoreKit" "NO-GIT" "17.4.0")
ADD_MENGINE_COCOAPOD("FBSDKLoginKit" "NO-GIT" "17.4.0")
ADD_MENGINE_COCOAPOD("FBSDKShareKit" "NO-GIT" "17.4.0")

ADD_MENGINE_APPLE_APPLICATION_DELEGATE("AppleFacebookApplicationDelegate")

if(MENGINE_USE_SCRIPT_SERVICE)
    TARGET_LINK_LIBRARIES(${PROJECT_NAME} Python)
endif()
