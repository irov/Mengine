MENGINE_PROJECT(AppleAmplitudePlugin)

ADD_FILTER(
src
    AppleAmplitudePlugin.h
    AppleAmplitudePlugin.mm

    AppleAmplitudeAnalyticsEventProvider.h
    AppleAmplitudeAnalyticsEventProvider.mm

    AppleAmplitudeApplicationDelegate.h
    AppleAmplitudeApplicationDelegate.mm
)

ADD_MENGINE_PLUGIN(MENGINE_PLUGIN_APPLE_AMPLITUDE)

ADD_MENGINE_COCOAPOD("Amplitude" "NO-GIT" "8.22.2")

ADD_MENGINE_APPLE_APPLICATION_DELEGATE("AppleAmplitudeApplicationDelegate")

