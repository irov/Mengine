MENGINE_PROJECT(AppleDatadogPlugin)

ADD_FILTER(
src
    AppleDatadogApplicationDelegate.h
    AppleDatadogApplicationDelegate.mm
)

ADD_MENGINE_PLUGIN(MENGINE_PLUGIN_APPLE_DATADOG)

ADD_MENGINE_APPLE_APPLICATION_DELEGATE("AppleDatadogApplicationDelegate")

ADD_MENGINE_COCOAPOD_GOOGLE_UTILITIES()
ADD_MENGINE_COCOAPOD("DatadogObjc" "NO-GIT" "2.24.0")