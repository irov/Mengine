MENGINE_PROJECT(iOSFirebaseAnalyticsPlugin)

if(MENGINE_BUILD_TYPE_DEBUG)
    set(FIREBASE_ANALYTICS_DEACTIVATED true CACHE INTERNAL "Deactivate Firebase Analytics collection" FORCE)
else()
    set(FIREBASE_ANALYTICS_DEACTIVATED false CACHE INTERNAL "Deactivate Firebase Analytics collection" FORCE)
endif()

ADD_FILTER(
src
    iOSFirebaseAnalyticsPlugin.h
    iOSFirebaseAnalyticsPlugin.mm
)

ADD_MENGINE_PLUGIN(MENGINE_PLUGIN_IOS_FIREBASE_ANALYTICS)

ADD_MENGINE_APPLE_APPLICATION_DELEGATE("iOSFirebaseAnalyticsPlugin")

ADD_MENGINE_SWIFT_PACKAGE("FirebaseAnalytics" "https://github.com/firebase/firebase-ios-sdk.git" "13.2.0")

TARGET_LINK_LIBRARIES(${PROJECT_NAME} iOSFirebasePlugin)
