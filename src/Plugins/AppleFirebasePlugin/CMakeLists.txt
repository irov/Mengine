MENGINE_PROJECT(AppleFirebasePlugin)

ADD_FILTER(
src
    AppleFirebasePlugin.h
    AppleFirebasePlugin.mm
    
    AppleFirebaseApplicationDelegate.h
    AppleFirebaseApplicationDelegate.mm
)

ADD_MENGINE_APPLE_PLUGIN(MENGINE_PLUGIN_APPLE_FIREBASE)

ADD_MENGINE_COCOAPOD("GoogleUtilities" "NO-GIT" "7.13.0")
ADD_MENGINE_COCOAPOD("Firebase" "NO-GIT" "10.23.0")

ADD_MENGINE_APPLE_APPLICATION_DELEGATE("AppleFirebaseApplicationDelegate")

IF(NOT MENGINE_APPLICATION_DEPLOY_FIREBASE)
    MESSAGE(FATAL_ERROR "Apple Firebase plugin required setup MENGINE_APPLICATION_DEPLOY_FIREBASE")
ENDIF()

ADD_MENGINE_APPLE_RESOURCES(${MENGINE_APPLICATION_DEPLOY_FIREBASE})
