MENGINE_PROJECT(AppleFacebookPlugin)

ADD_FILTER(
src
    AppleFacebookPlugin.h
    AppleFacebookPlugin.cpp

    ApplleFacebookInterface.h

    AppleFacebookApplicationDelegate.h
    AppleFacebookApplicationDelegate.mm

    AppleFacebookService.h
    AppleFacebookService.mm
)

ADD_MENGINE_PLUGIN(MENGINE_PLUGIN_APPLE_FACEBOOK)

ADD_MENGINE_COCOAPOD("FBSDKCoreKit" "NO-GIT" "13.2.0")
ADD_MENGINE_COCOAPOD("FBSDKLoginKit" "NO-GIT" "13.2.0")
ADD_MENGINE_COCOAPOD("FBSDKShareKit" "NO-GIT" "13.2.0")

ADD_MENGINE_APPLE_APPLICATION_DELEGATE("AppleFacebookApplicationDelegate")
