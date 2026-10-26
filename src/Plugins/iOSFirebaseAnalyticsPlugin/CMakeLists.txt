MENGINE_PROJECT(iOSFirebaseAnalyticsPlugin)

ADD_FILTER(
src
    iOSFirebaseAnalyticsPlugin.h
    iOSFirebaseAnalyticsPlugin.mm
)

ADD_MENGINE_PLUGIN(MENGINE_PLUGIN_IOS_FIREBASE_ANALYTICS)

ADD_MENGINE_APPLE_APPLICATION_DELEGATE("iOSFirebaseAnalyticsPlugin")

ADD_MENGINE_SWIFT_PACKAGE("FirebaseAnalytics" "https://github.com/firebase/firebase-ios-sdk.git" "13.1.1")

TARGET_LINK_LIBRARIES(${PROJECT_NAME} iOSFirebasePlugin)
