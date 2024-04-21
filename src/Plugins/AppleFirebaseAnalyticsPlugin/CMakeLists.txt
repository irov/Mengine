MENGINE_PROJECT(AppleFirebaseAnalyticsPlugin)

ADD_FILTER(
src
    AppleFirebaseAnalyticsInterface.h

    AppleFirebaseAnalyticsPlugin.h
    AppleFirebaseAnalyticsPlugin.mm
    
    AppleFirebaseAnalyticsService.h
    AppleFirebaseAnalyticsService.mm

    AppleFirebaseAnalyticsApplicationDelegate.h
    AppleFirebaseAnalyticsApplicationDelegate.mm
)

ADD_MENGINE_APPLE_PLUGIN(MENGINE_PLUGIN_APPLE_FIREBASE_ANALYTICS)

ADD_MENGINE_APPLE_APPLICATION_DELEGATE("AppleFirebaseAnalyticsApplicationDelegate")

ADD_MENGINE_COCOAPOD("GoogleUtilities" "NO-GIT" "7.13.0")
ADD_MENGINE_COCOAPOD("FirebaseAnalytics" "NO-GIT" "10.24.0")
