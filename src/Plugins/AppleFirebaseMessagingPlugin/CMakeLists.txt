MENGINE_PROJECT(AppleFirebaseMessagingPlugin)

ADD_FILTER(
src
    AppleFirebaseMessagingInterface.h

    AppleFirebaseMessagingPlugin.h
    AppleFirebaseMessagingPlugin.cpp

    
    AppleFirebaseMessagingService.h
    AppleFirebaseMessagingService.mm
)


ADD_MENGINE_PLUGIN(MENGINE_PLUGIN_APPLE_FIREBASE_MESSAGING)

ADD_MENGINE_COCOAPOD("Firebase" "NO-GIT" "9.2.0")
ADD_MENGINE_COCOAPOD("Firebase/Messaging" "NO-GIT" "9.2.0")