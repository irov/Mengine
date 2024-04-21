MENGINE_PROJECT(AppleFirebaseMessagingPlugin)

ADD_FILTER(
src
    AppleFirebaseMessagingInterface.h

    AppleFirebaseMessagingPlugin.h
    AppleFirebaseMessagingPlugin.mm
    
    AppleFirebaseMessagingApplicationDelegate.h
    AppleFirebaseMessagingApplicationDelegate.mm
    
    AppleFirebaseMessagingService.h
    AppleFirebaseMessagingService.mm
)

ADD_MENGINE_APPLE_PLUGIN(MENGINE_PLUGIN_APPLE_FIREBASE_MESSAGING)

ADD_MENGINE_COCOAPOD("GoogleUtilities" "NO-GIT" "7.13.0")
ADD_MENGINE_COCOAPOD("FirebaseMessaging" "NO-GIT" "10.24.0")

ADD_MENGINE_APPLE_APPLICATION_DELEGATE("AppleFirebaseMessagingApplicationDelegate")