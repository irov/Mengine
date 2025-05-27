MENGINE_PROJECT(AppleFirebaseAnalyticsPlugin)

ADD_FILTER(
src
    AppleFirebaseAnalyticsPlugin.h
    AppleFirebaseAnalyticsPlugin.mm
    
    AppleFirebaseAnalyticsEventProvider.h
    AppleFirebaseAnalyticsEventProvider.mm

    AppleFirebaseAnalyticsApplicationDelegate.h
    AppleFirebaseAnalyticsApplicationDelegate.mm
)

ADD_MENGINE_PLUGIN(MENGINE_PLUGIN_APPLE_FIREBASE_ANALYTICS)

ADD_MENGINE_APPLE_APPLICATION_DELEGATE("AppleFirebaseAnalyticsApplicationDelegate")

ADD_MENGINE_COCOAPOD_GOOGLE_UTILITIES()
ADD_MENGINE_COCOAPOD("FirebaseAnalytics" "NO-GIT" "11.13.0")

TARGET_LINK_LIBRARIES(${PROJECT_NAME} AppleFirebasePlugin)
