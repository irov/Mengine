MENGINE_PROJECT(AppleMARSDKPlugin)

ADD_FILTER(
src
    AppleMARSDKInterface.h

    AppleMARSDKPlugin.h
    AppleMARSDKPlugin.cpp

    AppleMARSDKApplicationDelegate.h
    AppleMARSDKApplicationDelegate.mm

    AppleMARSDKDelegate.h
    AppleMARSDKDelegate.mm
    
    AppleMARSDKService.h
    AppleMARSDKService.mm
)

if(MENGINE_USE_SCRIPT_SERVICE)
    ADD_FILTER(
    embedding
        AppleMARSDKScriptEmbedding.h
        AppleMARSDKScriptEmbedding.cpp
        )
        
    INCLUDE_DIRECTORIES(${THIRDPARTY_DIR}/pybind/include)
endif()

ADD_MENGINE_PLUGIN(MENGINE_PLUGIN_APPLE_MARSDK)

ADD_MENGINE_APPLE_APPLICATION_DELEGATE("AppleMARSDKApplicationDelegate")

ADD_MENGINE_APPLE_FRAMEWORK_WITH_PATH(MARSDKCore "${THIRDPARTY_DIR}/marsdk-ios")

TARGET_LINK_LIBRARIES(${PROJECT_NAME} libsqlite3.tbd)
TARGET_LINK_LIBRARIES(${PROJECT_NAME} libz.tbd)
TARGET_LINK_LIBRARIES(${PROJECT_NAME} libc++.tbd)
TARGET_LINK_LIBRARIES(${PROJECT_NAME} libicucore.tbd)

if(MENGINE_USE_SCRIPT_SERVICE)
    TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${THIRDPARTY_LIB_DIR}/${MENGINE_LIB_PREFIX}pybind${MENGINE_LIB_SUFFIX})
endif()
