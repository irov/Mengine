MENGINE_PROJECT(AppleFacebookPlugin)

ADD_FILTER(
src
    AppleFacebookPlugin.h
    AppleFacebookPlugin.cpp

    AppleFacebookInterface.h

    AppleFacebookApplicationDelegate.h
    AppleFacebookApplicationDelegate.mm

    AppleFacebookService.h
    AppleFacebookHelper.swift

    AppleFacebookService.mm
    ObjC-Bridging-Header.h
)

ADD_MENGINE_PLUGIN(MENGINE_PLUGIN_APPLE_FACEBOOK)

ADD_MENGINE_COCOAPOD("FBSDKCoreKit" "NO-GIT" "13.2.0")
ADD_MENGINE_COCOAPOD("FBSDKLoginKit" "NO-GIT" "13.2.0")
ADD_MENGINE_COCOAPOD("FBSDKShareKit" "NO-GIT" "13.2.0")

ADD_MENGINE_APPLE_APPLICATION_DELEGATE("AppleFacebookApplicationDelegate")


set_property(TARGET ${PROJECT_NAME} PROPERTY XCODE_ATTRIBUTE_SWIFT_OBJC_BRIDGING_HEADER "${CMAKE_CURRENT_SOURCE_DIR}/ObjC-Bridging-Header.h")

set_target_properties(
    ${PROJECT_NAME}
    PROPERTIES
    XCODE_ATTRIBUTE_CLANG_ENABLE_OBJC_ARC YES  
)
