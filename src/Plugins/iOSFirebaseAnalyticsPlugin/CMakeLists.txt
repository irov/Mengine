MENGINE_PROJECT(iOSFirebaseAnalyticsPlugin)

ADD_FILTER(
src
    iOSFirebaseAnalyticsPlugin.h
    iOSFirebaseAnalyticsPlugin.mm
)

ADD_MENGINE_PLUGIN(MENGINE_PLUGIN_IOS_FIREBASE_ANALYTICS)

ADD_MENGINE_APPLE_APPLICATION_DELEGATE("iOSFirebaseAnalyticsPlugin")

ADD_MENGINE_SWIFT_PACKAGE("FirebaseAnalytics" "https://github.com/firebase/firebase-ios-sdk.git" "13.0.0")

if(MENGINE_TARGET_IOS_SIMULATOR)
    SET(FIREBASE_ANALYTICS_PLATFORM "ios-arm64_x86_64-simulator")
else()
    SET(FIREBASE_ANALYTICS_PLATFORM "ios-arm64")
endif()

TARGET_COMPILE_OPTIONS(${PROJECT_NAME} PRIVATE "-F${CMAKE_BINARY_DIR}/SourcePackages/artifacts/firebase-ios-sdk/FirebaseAnalytics/FirebaseAnalytics.xcframework/${FIREBASE_ANALYTICS_PLATFORM}")

TARGET_LINK_LIBRARIES(${PROJECT_NAME} iOSFirebasePlugin)
