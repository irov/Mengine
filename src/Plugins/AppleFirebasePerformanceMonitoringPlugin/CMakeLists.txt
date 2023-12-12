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

ADD_MENGINE_COCOAPOD("Firebase/Performance" "NO-GIT" "10.19.0")