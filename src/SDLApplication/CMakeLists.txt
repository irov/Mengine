set( MY_LIB_NAME SDLApplication )

ADD_FILTER(
src
    main.cpp
    SDLApplication.cpp
    SDLApplication.h
    SDLMessageBoxLogger.cpp
    SDLMessageBoxLogger.h
    SDLStdioLogger.cpp
    SDLStdioLogger.h
)

if(MENGINE_TARGET_ANDROID)
    ADD_FILTER(
    android
        AndroidLogger.cpp
        AndroidLogger.h
    )
endif()

INCLUDE_DIRECTORIES( 
    ${THIRDPARTY_DIR}/pybind/include
    ${THIRDPARTY_DIR}/SDL2/include
    ${THIRDPARTY_DIR}/tinyini/include
    ${THIRDPARTY_DIR}/libmath/include
)

LINK_DIRECTORIES( ${CMAKE_ARCHIVE_OUTPUT_DIRECTORY} )

if(MENGINE_TARGET_WINDOWS OR MENGINE_TARGET_MINGW)
    ADD_EXECUTABLE( ${MY_LIB_NAME} WIN32 ${SRC_FILES} )
endif()

if(MENGINE_TARGET_ANDROID)
    ADD_LIBRARY( ${MY_LIB_NAME} SHARED ${SRC_FILES} )
endif()

if(MENGINE_TARGET_IOS AND NOT MENGINE_BUILD_ONLY_EXECUTABLE_MODULE)
    set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "~/Desktop/deploy-ios-plist")

    set(myApp_MEDIA ~/Desktop/deploy-ios-media.xcassets)
    set_source_files_properties(${myApp_MEDIA} PROPERTIES MACOSX_PACKAGE_LOCATION "Resources")

    set(DEPLOY_DATA $ENV{HOME}/Desktop/deploy-ios-data)
    file(GLOB_RECURSE myApp_DATAS "${DEPLOY_DATA}/*")
    list(FILTER myApp_DATAS EXCLUDE REGEX ".*.DS_Store$")
    foreach(myApp_DATA ${myApp_DATAS})
        file(RELATIVE_PATH RES ${DEPLOY_DATA} ${myApp_DATA})
        get_filename_component(RES_LOC ${RES} PATH)

        set_source_files_properties(${myApp_DATA} PROPERTIES MACOSX_PACKAGE_LOCATION "deploy-ios-data/${RES_LOC}" HEADER_FILE_ONLY 1)
        string(REPLACE "/" "\\" ide_group_name "Data/${RES_LOC}")
        source_group(${ide_group_name} FILES ${myApp_DATA})
    endforeach()

    string(TIMESTAMP MACOSX_FRAMEWORK_BUNDLE_VERSION "%Y.%m.%d.%H%M%S")
    string(TIMESTAMP MACOSX_FRAMEWORK_SHORT_VERSION_STRING "%Y.%m.%d")
    set(CMAKE_OSX_DEPLOYMENT_TARGET "9.0" CACHE STRING "Minimum OS X deployment version" FORCE)

    ADD_EXECUTABLE( ${MY_LIB_NAME} MACOSX_BUNDLE ${myApp_MEDIA} ${myApp_DATAS} ${SRC_FILES} )

    SET_TARGET_PROPERTIES( ${MY_LIB_NAME} PROPERTIES MACOSX_BUNDLE YES )
    SET_TARGET_PROPERTIES( ${MY_LIB_NAME} PROPERTIES MACOSX_BUNDLE_INFO_PLIST info.plist.in )
    SET_TARGET_PROPERTIES( ${MY_LIB_NAME} PROPERTIES XCODE_ATTRIBUTE_INSTALL_PATH "$(LOCAL_APPS_DIR)" )
    SET_TARGET_PROPERTIES( ${MY_LIB_NAME} PROPERTIES XCODE_ATTRIBUTE_TARGETED_DEVICE_FAMILY "1,2" )
    SET_TARGET_PROPERTIES( ${MY_LIB_NAME} PROPERTIES XCODE_ATTRIBUTE_COMBINE_HIDPI_IMAGES "NO" )

    SET_TARGET_PROPERTIES( ${MY_LIB_NAME} PROPERTIES XCODE_ATTRIBUTE_ASSETCATALOG_COMPILER_APPICON_NAME AppIcon)
    SET_TARGET_PROPERTIES( ${MY_LIB_NAME} PROPERTIES XCODE_ATTRIBUTE_ASSETCATALOG_COMPILER_LAUNCHIMAGE_NAME LaunchImage)

endif()

if(MENGINE_TARGET_IOS AND MENGINE_BUILD_ONLY_EXECUTABLE_MODULE)    
    ADD_EXECUTABLE( ${MY_LIB_NAME} MACOSX_BUNDLE ${SRC_FILES} )
endif()

if(MENGINE_TARGET_MAC)
    set(myApp_ICON ~/Desktop/deploy-mac-icons/icon.icns)
    set_source_files_properties(${myApp_ICON} PROPERTIES
           MACOSX_PACKAGE_LOCATION "Resources")

    set(myApp_ASTRALAX ~/Desktop/magic3d.dylib)
    set_source_files_properties(${myApp_ASTRALAX} PROPERTIES
           MACOSX_PACKAGE_LOCATION "MacOS")

    if(MENGINE_PLUGIN_STEAM)
        set(myApp_STEAM ~/Desktop/libsteam_api.dylib)
        set_source_files_properties(${myApp_STEAM} PROPERTIES
              MACOSX_PACKAGE_LOCATION "MacOS")
    else()
        set(myApp_STEAM)
    endif()

    set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "~/Desktop/deploy-mac-plist")
           
    ADD_EXECUTABLE( ${MY_LIB_NAME} MACOSX_BUNDLE ${myApp_ICON} ${myApp_ASTRALAX} ${myApp_STEAM} ${SRC_FILES} )

    set_target_properties( ${MY_LIB_NAME} PROPERTIES MACOSX_BUNDLE_ICON_FILE icon.icns )
    set_target_properties( ${MY_LIB_NAME} PROPERTIES MACOSX_BUNDLE_INFO_PLIST info.plist.in)
endif()

foreach(dependency ${APPLICATION_DEPENDENCIES})
	#MESSAGE( ${LIB_PREFIX}${dependency}${MENGINE_LIB_SUFFIX} )
	TARGET_LINK_LIBRARIES( ${MY_LIB_NAME} debug ${dependency} )
	TARGET_LINK_LIBRARIES( ${MY_LIB_NAME} optimized ${dependency} )
endforeach(dependency ${APPLICATION_DEPENDENCIES})

if(MENGINE_TOOLCHAIN_MSVC OR MENGINE_TOOLCHAIN_XCODE_MAC)
    TARGET_LINK_LIBRARIES( ${MY_LIB_NAME} ${THIRDPARTY_LIB_DIR}/${MENGINE_LIB_PREFIX}SDL2${MENGINE_DEPENDENCIES_DEBUG_SUFFIX}${MENGINE_LIB_SUFFIX})
endif()

if(MENGINE_TARGET_IOS AND NOT MENGINE_BUILD_ONLY_EXECUTABLE_MODULE)
    TARGET_LINK_LIBRARIES( ${MY_LIB_NAME} ${THIRDPARTY_DIR}/SDL2/Xcode-iOS/SDL/build/Debug-iphoneos/libSDL2.a)
endif()

if(MENGINE_TARGET_IOS AND MENGINE_BUILD_ONLY_EXECUTABLE_MODULE)
    TARGET_LINK_LIBRARIES( ${MY_LIB_NAME} ${THIRDPARTY_LIB_DIR}/${MENGINE_LIB_PREFIX}SDL2${MENGINE_DEPENDENCIES_DEBUG_SUFFIX}${MENGINE_LIB_SUFFIX})
endif()

if(MENGINE_TOOLCHAIN_MSVC OR MENGINE_TOOLCHAIN_XCODE_MAC)
    TARGET_LINK_LIBRARIES( ${MY_LIB_NAME} ${THIRDPARTY_LIB_DIR}/${MENGINE_LIB_PREFIX}SDL2main${MENGINE_DEPENDENCIES_DEBUG_SUFFIX}${MENGINE_LIB_SUFFIX})
endif()


TARGET_LINK_LIBRARIES( ${MY_LIB_NAME} ${APPLICATION_LIBS} )

IF(MENGINE_TARGET_IOS)
    #ADD_FRAMEWORK(${MY_LIB_NAME} Cocoa)
    #ADD_FRAMEWORK(${MY_LIB_NAME} Carbon)
    #ADD_FRAMEWORK(${MY_LIB_NAME} AppKit)
    ADD_FRAMEWORK(${MY_LIB_NAME} CoreData)
    ADD_FRAMEWORK(${MY_LIB_NAME} CoreAudio)
    ADD_FRAMEWORK(${MY_LIB_NAME} CoreVideo)
    ADD_FRAMEWORK(${MY_LIB_NAME} CoreFoundation)
    ADD_FRAMEWORK(${MY_LIB_NAME} CoreMotion)
    ADD_FRAMEWORK(${MY_LIB_NAME} AudioToolbox)
    ADD_FRAMEWORK(${MY_LIB_NAME} AVFoundation)
    ADD_FRAMEWORK(${MY_LIB_NAME} IOKit)
    ADD_FRAMEWORK(${MY_LIB_NAME} UIKit)
    ADD_FRAMEWORK(${MY_LIB_NAME} GameController)
    ADD_FRAMEWORK(${MY_LIB_NAME} MediaPlayer)
    ADD_FRAMEWORK(${MY_LIB_NAME} QuartzCore)
    ADD_FRAMEWORK(${MY_LIB_NAME} Foundation)
    ADD_FRAMEWORK(${MY_LIB_NAME} OpenGLES)
    ADD_FRAMEWORK(${MY_LIB_NAME} CoreGraphics)
    ADD_FRAMEWORK(${MY_LIB_NAME} OpenAL)
    ADD_FRAMEWORK(${MY_LIB_NAME} Security)
    ADD_FRAMEWORK(${MY_LIB_NAME} Metal)
ENDIF()

IF(MENGINE_TARGET_ANDROID)
    add_library( SDL2
            SHARED
            IMPORTED )
             
    set_target_properties( # Specifies the target library.
            SDL2
            # Specifies the parameter you want to define.
            PROPERTIES IMPORTED_LOCATION

            # Provides the path to the library you want to import.
            ${MENGINE_REPOSITORY}/gradle/libraries/SDL2/build/intermediates/ndkBuild/${CMAKE_BUILD_TYPE}/obj/local/${ANDROID_ABI}/libSDL2.so
            )

    TARGET_LINK_LIBRARIES( ${MY_LIB_NAME} SDL2)
    
    add_library( hidapi
            SHARED
            IMPORTED )
             
    set_target_properties( # Specifies the target library.
            hidapi
            # Specifies the parameter you want to define.
            PROPERTIES IMPORTED_LOCATION

            # Provides the path to the library you want to import.
            ${MENGINE_REPOSITORY}/gradle/libraries/SDL2/build/intermediates/ndkBuild/${CMAKE_BUILD_TYPE}/obj/local/${ANDROID_ABI}/libhidapi.so
            )

    TARGET_LINK_LIBRARIES( ${MY_LIB_NAME} hidapi)    

    target_link_libraries(  ${MY_LIB_NAME}
                            stdc++
                            c
                            m
                            dl
                            log
                            android
                            OpenSLES
                            GLESv1_CM
                            GLESv2
    )
endif()

IF(MENGINE_TARGET_MACOS)
    ADD_FRAMEWORK(${MY_LIB_NAME} Cocoa)
    ADD_FRAMEWORK(${MY_LIB_NAME} Carbon)
    ADD_FRAMEWORK(${MY_LIB_NAME} AppKit)
    ADD_FRAMEWORK(${MY_LIB_NAME} CoreData)
    ADD_FRAMEWORK(${MY_LIB_NAME} CoreAudio)
    ADD_FRAMEWORK(${MY_LIB_NAME} CoreVideo)
    ADD_FRAMEWORK(${MY_LIB_NAME} CoreFoundation)
    ADD_FRAMEWORK(${MY_LIB_NAME} AudioUnit)
    ADD_FRAMEWORK(${MY_LIB_NAME} AudioToolBox)
    ADD_FRAMEWORK(${MY_LIB_NAME} IOKit)
    ADD_FRAMEWORK(${MY_LIB_NAME} ForceFeedback)
    ADD_FRAMEWORK(${MY_LIB_NAME} Foundation)
    ADD_FRAMEWORK(${MY_LIB_NAME} OpenGL)
    ADD_FRAMEWORK(${MY_LIB_NAME} OpenAL)
    ADD_FRAMEWORK(${MY_LIB_NAME} Security)
    
    TARGET_LINK_LIBRARIES( ${MY_LIB_NAME} iconv)
ENDIF()

IF(MENGINE_TARGET_WINDOWS OR MENGINE_TARGET_MINGW)
    TARGET_LINK_LIBRARIES( ${MY_LIB_NAME} Winmm Version Imm32) 
ENDIF()

IF(MENGINE_TOOLCHAIN_MSVC)
    set_target_properties( ${MY_LIB_NAME} PROPERTIES LINK_FLAGS /FORCE:MULTIPLE )
ENDIF()

IF(MENGINE_TARGET_IOS AND NOT MENGINE_BUILD_ONLY_EXECUTABLE_MODULE)
    SET_XCODE_PROPERTY(${MY_LIB_NAME} CODE_SIGN_IDENTITY "iPhone Developer")
    SET_XCODE_PROPERTY(${MY_LIB_NAME} DEVELOPMENT_TEAM "$ENV{XCODE_ATTRIBUTE_DEVELOPMENT_TEAM}")
ENDIF()

IF(MENGINE_TARGET_IOS AND MENGINE_BUILD_ONLY_EXECUTABLE_MODULE)
    SET_XCODE_PROPERTY(${MY_LIB_NAME} CODE_SIGNING_REQUIRED NO)
ENDIF()

ADD_DEPENDENCIES( ${MY_LIB_NAME} ${APPLICATION_DEPENDENCIES} )