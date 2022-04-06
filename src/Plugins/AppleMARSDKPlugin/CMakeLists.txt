MENGINE_PROJECT(AppleMARSDKPlugin)

ADD_FILTER(
src
    AppleMARSDKInterface.h

    AppleMARSDKPlugin.h
    AppleMARSDKPlugin.mm

    AppleMARSDKNative.h
    AppleMARSDKNative.m
)

ADD_FILTER(
Framework
    "${THIRDPARTY_DIR}/MARSDK/MARSDKCore.framework"
)

ADD_MENGINE_PLUGIN(MENGINE_PLUGIN_APPLE_MARSDK)


if(MENGINE_TARGET_IOS)
    set_target_properties(${PROJECT_NAME} PROPERTIES
        XCODE_EMBED_FRAMEWORKS "${THIRDPARTY_DIR}/MARSDK/MARSDKCore.framework"
        XCODE_ATTRIBUTE_OTHER_LDFLAGS "-ObjC")
endif()
