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

if(MENGINE_PLATFORM_ANDROID)
    ADD_FILTER(
    android
        AndroidLogger.cpp
        AndroidLogger.h
    )
endif()

add_definitions(-DUNICODE)
add_definitions(-D_UNICODE)

INCLUDE_DIRECTORIES( 
    ${Mengine_SOURCE_DIR}/Utils
    ${THIRDPARTY_DIR}/pybind/include
    ${THIRDPARTY_DIR}/SDL2/include
    ${THIRDPARTY_DIR}/tinyini/include
    ${THIRDPARTY_DIR}/libmath/include
)

LINK_DIRECTORIES( ${CMAKE_ARCHIVE_OUTPUT_DIRECTORY} )

if(MENGINE_PLATFORM_WIN32)
    ADD_EXECUTABLE( ${MY_LIB_NAME} WIN32 ${SRC_FILES} )
endif()

if(MENGINE_PLATFORM_ANDROID)
    ADD_LIBRARY( ${MY_LIB_NAME} SHARED ${SRC_FILES} )
endif()

if(MENGINE_PLATFORM_IOS)
    set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "~/Desktop/deploy-ios-plist")

    set(myApp_MEDIA
        ~/Desktop/deploy-ios-media.xcassets
    )

    set_source_files_properties(${myApp_MEDIA} PROPERTIES MACOSX_PACKAGE_LOCATION "Resources")

    ADD_EXECUTABLE( ${MY_LIB_NAME} MACOSX_BUNDLE ${myApp_MEDIA} ${SRC_FILES} )

    SET_TARGET_PROPERTIES( ${MY_LIB_NAME} PROPERTIES MACOSX_BUNDLE YES )
    SET_TARGET_PROPERTIES( ${MY_LIB_NAME} PROPERTIES MACOSX_BUNDLE_INFO_PLIST info.plist.in )
    SET_TARGET_PROPERTIES( ${MY_LIB_NAME} PROPERTIES XCODE_ATTRIBUTE_INSTALL_PATH "$(LOCAL_APPS_DIR)" )
endif()

if(MENGINE_PLATFORM_MAC)
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

if(MENGINE_PLATFORM_MAC OR MENGINE_PLATFORM_WIN32 OR MENGINE_PLATFORM_IOS)
    TARGET_LINK_LIBRARIES( ${MY_LIB_NAME} ${THIRDPARTY_LIB_DIR}/${MENGINE_LIB_PREFIX}SDL2${MENGINE_LIB_SUFFIX})
endif()

if(MENGINE_PLATFORM_ANDROID)
    add_library( SDL2
            SHARED
            IMPORTED )
             
    set_target_properties( # Specifies the target library.
            SDL2
            # Specifies the parameter you want to define.
            PROPERTIES IMPORTED_LOCATION

            # Provides the path to the library you want to import.
            ${THIRDPARTY_LIB_DIR}/${MENGINE_LIB_PREFIX}SDL2${MENGINE_SO_SUFFIX} )             

    TARGET_LINK_LIBRARIES( ${MY_LIB_NAME} SDL2)
endif()

if(MENGINE_PLATFORM_MAC OR MENGINE_PLATFORM_WIN32 OR MENGINE_PLATFORM_ANDROID)
    TARGET_LINK_LIBRARIES( ${MY_LIB_NAME} ${THIRDPARTY_LIB_DIR}/${MENGINE_LIB_PREFIX}SDL2main${MENGINE_LIB_SUFFIX})
endif()

TARGET_LINK_LIBRARIES( ${MY_LIB_NAME} ${APPLICATION_LIBS} )

IF(MENGINE_PLATFORM_IOS)
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

set_target_properties(
    ${MY_LIB_NAME} PROPERTIES 
	XCODE_ATTRIBUTE_TARGETED_DEVICE_FAMILY "1,2"
	XCODE_ATTRIBUTE_COMBINE_HIDPI_IMAGES "NO"
)

#set_target_properties(${MY_LIB_NAME} PROPERTIES
#    XCODE_ATTRIBUTE_CODE_SIGN_ENTITLEMENTS
#    ~/Desktop/Project.entitlements)
ENDIF()

IF(MENGINE_PLATFORM_ANDROID)
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

IF(MENGINE_PLATFORM_MAC)
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

IF(MENGINE_PLATFORM_WIN32)
  TARGET_LINK_LIBRARIES( ${MY_LIB_NAME} Winmm Version Imm32 OpenGL32) 
ENDIF()

IF(MSVC)
  set_target_properties( ${MY_LIB_NAME} PROPERTIES LINK_FLAGS /FORCE:MULTIPLE )
ENDIF()


