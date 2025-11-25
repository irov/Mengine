MENGINE_PROJECT(AppleFirebaseAnalyticsPlugin)

ADD_FILTER(
src
    AppleFirebaseAnalyticsApplicationDelegate.h
    AppleFirebaseAnalyticsApplicationDelegate.mm
)

ADD_MENGINE_APPLE_APPLICATION_DELEGATE("AppleFirebaseAnalyticsApplicationDelegate")

ADD_MENGINE_COCOAPOD_GOOGLE_UTILITIES()
ADD_MENGINE_COCOAPOD("FirebaseAnalytics" "NO-GIT" "12.6.0")

TARGET_LINK_LIBRARIES(${PROJECT_NAME} AppleFirebasePlugin)
