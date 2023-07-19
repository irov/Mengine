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


ADD_MENGINE_PLUGIN(MENGINE_PLUGIN_APPLE_FIREBASE_REMOTECONFIG)

ADD_MENGINE_COCOAPOD("Firebase" "NO-GIT" "10.12.0")
ADD_MENGINE_COCOAPOD("Firebase/RemoteConfig" "NO-GIT" "10.12.0")

ADD_MENGINE_APPLE_APPLICATION_DELEGATE("AppleFirebaseRemoteConfigApplicationDelegate")