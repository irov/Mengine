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

ADD_MENGINE_COCOAPOD("GoogleUtilities" "NO-GIT" "NO-TAG")
ADD_MENGINE_COCOAPOD("Firebase/RemoteConfig" "NO-GIT" "10.19.0")

ADD_MENGINE_APPLE_APPLICATION_DELEGATE("AppleFirebaseRemoteConfigApplicationDelegate")