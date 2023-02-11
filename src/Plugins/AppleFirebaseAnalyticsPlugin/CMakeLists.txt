MENGINE_PROJECT(AppleFirebaseAnalyticsPlugin)

ADD_FILTER(
src
    AppleFirebaseAnalyticsInterface.h

    AppleFirebaseAnalyticsPlugin.h
    AppleFirebaseAnalyticsPlugin.cpp
    
    AppleFirebaseAnalyticsService.h
    AppleFirebaseAnalyticsService.mm
)

ADD_MENGINE_PLUGIN(MENGINE_PLUGIN_APPLE_FIREBASE_ANALYTICS)

ADD_MENGINE_COCOAPOD("FirebaseAnalytics" "NO-GIT" "10.5.0")