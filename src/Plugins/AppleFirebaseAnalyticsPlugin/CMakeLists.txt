MENGINE_PROJECT(AppleFirebaseAnalyticsPlugin)

ADD_FILTER(
src
    AppleFirebaseAnalyticsInterface.h

    AppleFirebaseAnalyticsPlugin.h
    AppleFirebaseAnalyticsPlugin.mm
    
    AppleFirebaseAnalyticsService.h
    AppleFirebaseAnalyticsService.mm
)

ADD_MENGINE_APPLE_PLUGIN(MENGINE_PLUGIN_APPLE_FIREBASE_ANALYTICS)

ADD_MENGINE_COCOAPOD("Firebase/Analytics" "NO-GIT" "10.13.0")
