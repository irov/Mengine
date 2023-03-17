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

ADD_MENGINE_PLUGIN(MENGINE_PLUGIN_APPLE_FACEBOOK)

ADD_MENGINE_COCOAPOD("FBSDKCoreKit" "NO-GIT" "14.0.0")
ADD_MENGINE_COCOAPOD("FBSDKLoginKit" "NO-GIT" "14.0.0")
ADD_MENGINE_COCOAPOD("FBSDKShareKit" "NO-GIT" "14.0.0")

ADD_MENGINE_APPLE_APPLICATION_DELEGATE("AppleFacebookApplicationDelegate")


set_target_properties(
    ${PROJECT_NAME}
    PROPERTIES
    XCODE_ATTRIBUTE_CLANG_ENABLE_OBJC_ARC YES  
)

if(MENGINE_USE_SCRIPT_SERVICE)
    TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${THIRDPARTY_LIB_DIR}/${MENGINE_LIB_PREFIX}pybind${MENGINE_LIB_SUFFIX})
endif()
