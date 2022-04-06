MENGINE_PROJECT(AppleMARSDKPlugin)

ADD_FILTER(
src
    AppleMARSDKInterface.h

    AppleMARSDKPlugin.h
    AppleMARSDKPlugin.mm

    AppleMARSDKNative.h
    AppleMARSDKNative.m
)

ADD_MENGINE_PLUGIN(MENGINE_PLUGIN_APPLE_MARSDK)

if(MENGINE_TARGET_IOS)
    find_library(FRAMEWORK_MARSDK NAMES MARSDKCore PATHS "/Users/yuriylevchenko/Desktop/Mengine/dependencies/marsdk-ios" NO_CMAKE_FIND_ROOT_PATH)

    if( ${FRAMEWORK_MARSDK} STREQUAL FRAMEWORK_MARSDK-NOTFOUND)
        MESSAGE(FATAL_ERROR ": Framework MARSDK not found")
    else()
        target_link_libraries(${PROJECT_NAME} "-framework /Users/yuriylevchenko/Desktop/Mengine/dependencies/marsdk-ios/MARSDKCore.framework")
        MESSAGE(STATUS "Framework MARSDK found at ${FRAMEWORK_MARSDK}")
    endif()
endif()
