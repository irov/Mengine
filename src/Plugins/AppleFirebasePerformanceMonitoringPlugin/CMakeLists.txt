MENGINE_PROJECT(AppleFirebasePerformanceMonitoringPlugin)

ADD_FILTER(
src
    AppleFirebasePerformanceMonitoringInterface.h

    AppleFirebasePerformanceMonitoringPlugin.h
    AppleFirebasePerformanceMonitoringPlugin.mm
    
    AppleFirebasePerformanceMonitoringService.h
    AppleFirebasePerformanceMonitoringService.mm
)

ADD_MENGINE_APPLE_PLUGIN(MENGINE_PLUGIN_APPLE_FIREBASE_PERFORMANCEMONITORING)

ADD_MENGINE_COCOAPOD("GoogleUtilities" "NO-GIT" "7.13.0")
ADD_MENGINE_COCOAPOD("FirebasePerformance" "NO-GIT" "10.23.0")