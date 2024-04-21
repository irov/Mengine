MENGINE_PROJECT(AppleFirebaseRemoteConfigPlugin)

ADD_FILTER(
src
    AppleFirebaseRemoteConfigInterface.h

    AppleFirebaseRemoteConfigPlugin.h
    AppleFirebaseRemoteConfigPlugin.mm
    
    AppleFirebaseRemoteConfigApplicationDelegate.h
    AppleFirebaseRemoteConfigApplicationDelegate.mm
    
    AppleFirebaseRemoteConfigService.h
    AppleFirebaseRemoteConfigService.mm
)


ADD_MENGINE_APPLE_PLUGIN(MENGINE_PLUGIN_APPLE_FIREBASE_REMOTECONFIG)

ADD_MENGINE_COCOAPOD("GoogleUtilities" "NO-GIT" "7.13.0")
ADD_MENGINE_COCOAPOD("FirebaseRemoteConfig" "NO-GIT" "10.24.0")

ADD_MENGINE_APPLE_APPLICATION_DELEGATE("AppleFirebaseRemoteConfigApplicationDelegate")