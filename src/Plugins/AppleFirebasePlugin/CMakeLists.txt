MENGINE_PROJECT(AppleFirebasePlugin)

ADD_FILTER(
src
    AppleFirebasePlugin.h
    AppleFirebasePlugin.cpp
    
    AppleFirebaseApplicationDelegate.h
    AppleFirebaseApplicationDelegate.mm
)

ADD_MENGINE_PLUGIN(MENGINE_PLUGIN_APPLE_FIREBASE)

ADD_MENGINE_COCOAPOD("Firebase" "NO-GIT" "10.5.0")

ADD_MENGINE_APPLE_APPLICATION_DELEGATE("AppleFirebaseApplicationDelegate")
