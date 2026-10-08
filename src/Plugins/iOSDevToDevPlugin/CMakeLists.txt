MENGINE_PROJECT(iOSDevToDevPlugin)

ADD_FILTER(
src
    iOSDevToDevPlugin.h
    iOSDevToDevPlugin.mm
)

ADD_MENGINE_PLUGIN(MENGINE_PLUGIN_IOS_DEVTODEV)

ADD_MENGINE_SWIFT_PACKAGE("DTDAnalytics" "https://github.com/devtodev-analytics/ios-sdk-2.0.git" "2.6.4")

ADD_MENGINE_APPLE_APPLICATION_DELEGATE("iOSDevToDevPlugin")