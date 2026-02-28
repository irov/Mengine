MENGINE_PROJECT(AppleFirebasePerformanceMonitoringPlugin)

ADD_FILTER(
src    
    AppleFirebasePerformanceMonitoringPlugin.h
    AppleFirebasePerformanceMonitoringPlugin.mm
)

ADD_MENGINE_PLUGIN(MENGINE_PLUGIN_APPLE_FIREBASE_PERFORMANCEMONITORING)

ADD_MENGINE_APPLE_APPLICATION_DELEGATE("AppleFirebasePerformanceMonitoringPlugin")

ADD_MENGINE_COCOAPOD_GOOGLE_UTILITIES()
ADD_MENGINE_COCOAPOD("FirebasePerformance" "NO-GIT" "12.10.0")

TARGET_LINK_LIBRARIES(${PROJECT_NAME} AppleFirebasePlugin)
