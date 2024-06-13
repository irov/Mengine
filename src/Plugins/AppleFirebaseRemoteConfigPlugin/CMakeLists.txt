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

if(MENGINE_USE_SCRIPT_SERVICE)
    ADD_FILTER(
    embedding
        AppleFirebaseRemoteConfigScriptEmbedding.h
        AppleFirebaseRemoteConfigScriptEmbedding.mm
        )
        
    INCLUDE_DIRECTORIES(${THIRDPARTY_DIR}/pybind/include)
endif()

ADD_MENGINE_APPLE_PLUGIN(MENGINE_PLUGIN_APPLE_FIREBASE_REMOTECONFIG)

ADD_MENGINE_COCOAPOD_GOOGLE_UTILITIES()
ADD_MENGINE_COCOAPOD("FirebaseRemoteConfig" "NO-GIT" "10.27.0")

ADD_MENGINE_APPLE_APPLICATION_DELEGATE("AppleFirebaseRemoteConfigApplicationDelegate")

if(MENGINE_USE_SCRIPT_SERVICE)
    TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${THIRDPARTY_LIB_DIR}/${MENGINE_LIB_PREFIX}pybind${MENGINE_LIB_SUFFIX})
endif()