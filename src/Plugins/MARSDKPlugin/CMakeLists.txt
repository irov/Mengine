MENGINE_PROJECT(MARSDKPlugin)

ADD_FILTER(
src
    MARSDKInterface.h

    MARSDKPlugin.h
    MARSDKPlugin.def
    MARSDKPlugin.cpp

    IOSMarSDKNative.h
    IOSMarSDKNative.m
)

ADD_FILTER(
Framework
    "${CMAKE_CURRENT_SOURCE_DIR}/../../../dependencies/MARSDK/MARSDKCore.framework"
)

ADD_MENGINE_PLUGIN(MENGINE_PLUGIN_MARSDK)


if(MENGINE_TARGET_IOS)

    set_target_properties(${PROJECT_NAME} PROPERTIES
        XCODE_EMBED_FRAMEWORKS "${CMAKE_CURRENT_SOURCE_DIR}/../../../dependencies/MARSDK/MARSDKCore.framework"
        XCODE_LINK_BINARY_WITH_LIBRARIES "${CMAKE_CURRENT_SOURCE_DIR}/../../../dependencies/MARSDK/MARSDKCore.framework"
        XCODE_ATTRIBUTE_OTHER_LDFLAGS "-ObjC")

endif()