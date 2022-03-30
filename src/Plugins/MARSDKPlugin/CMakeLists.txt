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
        XCODE_EMBED_FRAMEWORKS "${CMAKE_CURRENT_SOURCE_DIR}/../../../dependencies/MARSDK/MARSDKCore.framework")

    set_target_properties(${PROJECT_NAME} PROPERTIES
        XCODE_LINK_BINARY_WITH_LIBRARIES "${CMAKE_CURRENT_SOURCE_DIR}/../../../dependencies/MARSDK/MARSDKCore.framework")

    set_target_properties(${PROJECT_NAME} PROPERTIES
                          LINK_FLAGS "-ObjC")


    # set(RESOURCE_FILES
    #     "${CMAKE_CURRENT_SOURCE_DIR}/../../../dependencies/MARSDK/MARSDK/Framework/MARSDKCore.framework"
    #     MARSDKCore.framework)

    # set_target_properties(${PROJECT_NAME} PROPERTIES
    #     RESOURCE "${RESOURCE_FILES}")


    # TARGET_LINK_LIBRARIES(${PROJECT_NAME} "${CMAKE_CURRENT_SOURCE_DIR}/../../../dependencies/MARSDK/MARSDK/Framework/MARSDKCore")
    # TARGET_LINK_LIBRARIES(${PROJECT_NAME} "-ObjC")

    # TARGET_LINK_LIBRARIES(${PROJECT_NAME} "${CMAKE_CURRENT_SOURCE_DIR}/../../../dependencies/MARSDK/MARSDK/Framework/MARSDKCore")
    # TARGET_LINK_LIBRARIES(${PROJECT_NAME} "-ObjC")
    # set_target_properties(program PROPERTIES LINK_FLAGS "-Wl,-F./2../../../dependencies/MARSDK/Framework")
    # ADD_MENGINE_COCOAPOD(MARSDK "https://github.com/7General/MarSDK" "0.0.1")
    # ADD_MENGINE_COCOAPOD(MARSDK "https://github.com/getsentry/sentry-cocoa.git" "7.10.2")

    # // pod 'MARSDK', :podspec => './../../../dependencies/MARSDK/'
endif()