MENGINE_PROJECT(AppleDevToDevPlugin)

ADD_FILTER(
src
    AppleDevToDevPlugin.h
    AppleDevToDevPlugin.mm
    
    AppleDevToDevAnalyticsEventProvider.h
    AppleDevToDevAnalyticsEventProvider.mm
    
    AppleDevToDevApplicationDelegate.h
    AppleDevToDevApplicationDelegate.mm
)

ADD_MENGINE_PLUGIN(MENGINE_PLUGIN_APPLE_DEVTODEV)

ADD_MENGINE_COCOAPOD("DTDAnalytics" "NO-GIT" "2.5.1")

ADD_MENGINE_APPLE_APPLICATION_DELEGATE("AppleDevToDevApplicationDelegate")