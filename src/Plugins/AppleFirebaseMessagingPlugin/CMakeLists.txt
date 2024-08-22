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

ADD_MENGINE_COCOAPOD_GOOGLE_UTILITIES()
ADD_MENGINE_COCOAPOD("FirebaseMessaging" "NO-GIT" "11.1.0")

ADD_MENGINE_APPLE_APPLICATION_DELEGATE("AppleFirebaseMessagingApplicationDelegate")