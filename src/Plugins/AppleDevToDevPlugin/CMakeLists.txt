MENGINE_PROJECT(AppleDevToDevPlugin)

ADD_FILTER(
src
    AppleDevToDevApplicationDelegate.h
    AppleDevToDevApplicationDelegate.mm
)

ADD_MENGINE_PLUGIN(MENGINE_PLUGIN_APPLE_DEVTODEV)

ADD_MENGINE_COCOAPOD("DTDAnalytics" "NO-GIT" "2.6.0")

ADD_MENGINE_APPLE_APPLICATION_DELEGATE("AppleDevToDevApplicationDelegate")