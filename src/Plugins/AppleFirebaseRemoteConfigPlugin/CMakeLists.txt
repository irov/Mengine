MENGINE_PROJECT(AppleFirebaseRemoteConfigPlugin)

SET(MENGINE_APPLICATION_DEPLOY_FIREBASE_REMOTECONFIG_PLIST_NAME "RemoteConfigDefaults" CACHE STRING "Mengine apple Firebase RemoteConfig plist name")

IF(NOT MENGINE_APPLICATION_DEPLOY_FIREBASE_REMOTECONFIG_PLIST_PATH)
    MESSAGE(FATAL_ERROR "Apple FirebaseRemoteConfig plugin required setup MENGINE_APPLICATION_DEPLOY_FIREBASE_REMOTECONFIG_PLIST_PATH")
ENDIF()

MESSAGE("Apple Firebase RemoteConfig plist path: ${MENGINE_APPLICATION_DEPLOY_FIREBASE_REMOTECONFIG_PLIST_PATH}")
MESSAGE("Apple Firebase RemoteConfig plist name: ${MENGINE_APPLICATION_DEPLOY_FIREBASE_REMOTECONFIG_PLIST_NAME}")

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

if(MENGINE_USE_SCRIPT_SERVICE)
    ADD_FILTER(
    embedding
        AppleFirebaseRemoteConfigScriptEmbedding.h
        AppleFirebaseRemoteConfigScriptEmbedding.mm
        )
        
    INCLUDE_DIRECTORIES(${THIRDPARTY_DIR}/pybind/include)
endif()

MENGINE_ADD_DEFINITION(MENGINE_FIREBASE_REMOTECONFIG_PLIST_NAME="${MENGINE_APPLICATION_DEPLOY_FIREBASE_REMOTECONFIG_PLIST_NAME}")

ADD_MENGINE_APPLE_PLUGIN(MENGINE_PLUGIN_APPLE_FIREBASE_REMOTECONFIG)

ADD_MENGINE_COCOAPOD_GOOGLE_UTILITIES()
ADD_MENGINE_COCOAPOD("FirebaseRemoteConfig" "NO-GIT" "10.29.0")

ADD_MENGINE_APPLE_APPLICATION_DELEGATE("AppleFirebaseRemoteConfigApplicationDelegate")



ADD_MENGINE_APPLE_RESOURCES(${MENGINE_APPLICATION_DEPLOY_FIREBASE_REMOTECONFIG_PLIST_PATH})

if(MENGINE_USE_SCRIPT_SERVICE)
    TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${THIRDPARTY_LIB_DIR}/${MENGINE_LIB_PREFIX}pybind${MENGINE_LIB_SUFFIX})
endif()