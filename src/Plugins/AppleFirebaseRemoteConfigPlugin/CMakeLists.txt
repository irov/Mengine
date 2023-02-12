MENGINE_PROJECT(AppleFirebaseRemoteConfigPlugin)

ADD_FILTER(
src
    AppleFirebaseRemoteConfigInterface.h

    AppleFirebaseRemoteConfigPlugin.h
    AppleFirebaseRemoteConfigPlugin.cpp
    
    AppleFirebaseRemoteConfigService.h
    AppleFirebaseRemoteConfigService.mm
)


ADD_MENGINE_PLUGIN(MENGINE_PLUGIN_APPLE_FIREBASE_REMOTECONFIG)

ADD_MENGINE_COCOAPOD("Firebase" "NO-GIT" "10.5.0")
ADD_MENGINE_COCOAPOD("Firebase/RemoteConfig" "NO-GIT" "10.5.0")