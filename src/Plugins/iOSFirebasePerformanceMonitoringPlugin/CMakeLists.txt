MENGINE_PROJECT(iOSFirebasePerformanceMonitoringPlugin)

ADD_FILTER(
src
    iOSFirebasePerformanceMonitoringPlugin.h
    iOSFirebasePerformanceMonitoringPlugin.mm
)

ADD_MENGINE_PLUGIN(MENGINE_PLUGIN_IOS_FIREBASE_PERFORMANCEMONITORING)

ADD_MENGINE_APPLE_APPLICATION_DELEGATE("iOSFirebasePerformanceMonitoringPlugin")

ADD_MENGINE_SWIFT_PACKAGE("FirebasePerformance" "https://github.com/firebase/firebase-ios-sdk.git" "13.1.0")

TARGET_LINK_LIBRARIES(${PROJECT_NAME} iOSFirebasePlugin)
