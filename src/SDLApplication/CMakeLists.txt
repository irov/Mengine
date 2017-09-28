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

add_definitions(-DUNICODE)
add_definitions(-D_UNICODE)

INCLUDE_DIRECTORIES( 
    ${Mengine_SOURCE_DIR}/Utils
    ${THIRDPARTY_DIR}/pybind/include
    ${THIRDPARTY_DIR}/pybind/include
    ${THIRDPARTY_DIR}/SDL2/include
    ${THIRDPARTY_DIR}/tinyini/include
)

#MESSAGE( CMAKE_ARCHIVE_OUTPUT_DIRECTORY = "${CMAKE_ARCHIVE_OUTPUT_DIRECTORY}" )
LINK_DIRECTORIES( ${CMAKE_ARCHIVE_OUTPUT_DIRECTORY} )

if(WIN32)
    ADD_EXECUTABLE( ${MY_LIB_NAME} WIN32 ${SRC_FILES} )
endif()

if(MENGINE_PLATFORM_IOS)
    set(myApp_ICON 
        ~/Desktop/deploy-ios-icons/Default-568h@2x.png
        ~/Desktop/deploy-ios-icons/Default-568h@2x.png
        ~/Desktop/deploy-ios-icons/Default-667h@2x.png
        ~/Desktop/deploy-ios-icons/Default-736h@3x.png
        ~/Desktop/deploy-ios-icons/Default-Landscape-568h@2x.png
        ~/Desktop/deploy-ios-icons/Default-Landscape-667h@2x.png
        ~/Desktop/deploy-ios-icons/Default-Landscape-736h@2x.png
        ~/Desktop/deploy-ios-icons/Default-Landscape-1366@2x.png
        ~/Desktop/deploy-ios-icons/Default-Landscape.png
        ~/Desktop/deploy-ios-icons/Default-Landscape@2x.png
        ~/Desktop/deploy-ios-icons/Default-Portrait-1366@2x.png
        ~/Desktop/deploy-ios-icons/Default-Portrait.png
        ~/Desktop/deploy-ios-icons/Default-Portrait@2x.png
        ~/Desktop/deploy-ios-icons/Default.png
        ~/Desktop/deploy-ios-icons/Default@2x.png
        ~/Desktop/deploy-ios-icons/Icon-App-20x20@1x.png
        ~/Desktop/deploy-ios-icons/Icon-App-20x20@2x.png
        ~/Desktop/deploy-ios-icons/Icon-App-20x20@3x.png
        ~/Desktop/deploy-ios-icons/Icon-App-29x29@1x.png
        ~/Desktop/deploy-ios-icons/Icon-App-29x29@2x.png
        ~/Desktop/deploy-ios-icons/Icon-App-29x29@3x.png
        ~/Desktop/deploy-ios-icons/Icon-App-40x40@1x.png
        ~/Desktop/deploy-ios-icons/Icon-App-40x40@2x.png
        ~/Desktop/deploy-ios-icons/Icon-App-40x40@3x.png
        ~/Desktop/deploy-ios-icons/Icon-App-57x57@1x.png
        ~/Desktop/deploy-ios-icons/Icon-App-57x57@2x.png
        ~/Desktop/deploy-ios-icons/Icon-App-60x60@1x.png
        ~/Desktop/deploy-ios-icons/Icon-App-60x60@2x.png
        ~/Desktop/deploy-ios-icons/Icon-App-60x60@3x.png
        ~/Desktop/deploy-ios-icons/Icon-App-72x72@1x.png
        ~/Desktop/deploy-ios-icons/Icon-App-72x72@2x.png
        ~/Desktop/deploy-ios-icons/Icon-App-76x76@1x.png
        ~/Desktop/deploy-ios-icons/Icon-App-76x76@2x.png
        ~/Desktop/deploy-ios-icons/Icon-App-76x76@3x.png
        ~/Desktop/deploy-ios-icons/Icon-App-83.5x83.5@2x.png
        ~/Desktop/deploy-ios-icons/Icon-Small-50x50@1x.png
        ~/Desktop/deploy-ios-icons/Icon-Small-50x50@2x.png
        ~/Desktop/deploy-ios-icons/iTunesArtwork@1x.png
        ~/Desktop/deploy-ios-icons/iTunesArtwork@2x.png
        ~/Desktop/deploy-ios-icons/iTunesArtwork@3x.png
    )
    set_source_files_properties(${myApp_ICON} PROPERTIES
           MACOSX_PACKAGE_LOCATION "Resources")

    set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "~/Desktop")
           
    ADD_EXECUTABLE( ${MY_LIB_NAME} MACOSX_BUNDLE ${myApp_ICON} ${SRC_FILES} )

    SET_TARGET_PROPERTIES( ${MY_LIB_NAME} PROPERTIES MACOSX_BUNDLE_INFO_PLIST Info.plist.in )
    SET_TARGET_PROPERTIES( ${MY_LIB_NAME} PROPERTIES XCODE_ATTRIBUTE_INSTALL_PATH "$(LOCAL_APPS_DIR)" )
endif()

if(MENGINE_PLATFORM_MAC)
    set(myApp_ICON ~/Desktop/icon.icns)
    set_source_files_properties(${myApp_ICON} PROPERTIES
           MACOSX_PACKAGE_LOCATION "Resources")

    set(myApp_ASTRALAX ~/Desktop/magic3d.dylib)
    set_source_files_properties(${myApp_ASTRALAX} PROPERTIES
           MACOSX_PACKAGE_LOCATION "MacOS")

    set(myApp_STEAM ~/Desktop/libsteam_api.dylib)
    set_source_files_properties(${myApp_STEAM} PROPERTIES
           MACOSX_PACKAGE_LOCATION "MacOS")


    set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "~/Desktop")
           
    ADD_EXECUTABLE( ${MY_LIB_NAME} MACOSX_BUNDLE ${myApp_ICON} ${myApp_ASTRALAX} ${myApp_STEAM} ${SRC_FILES} )

    set_target_properties( ${MY_LIB_NAME} PROPERTIES MACOSX_BUNDLE_INFO_PLIST Info.plist.in)
endif()

#if (WIN32)
#	set_target_properties( ${MY_LIB_NAME} PROPERTIES WIN32_EXECUTABLE TRUE)
#endif(WIN32)

foreach(dependency ${APPLICATION_DEPENDENCIES})
	#MESSAGE( ${LIB_PREFIX}${dependency}${MENGINE_LIB_SUFFIX} )
	TARGET_LINK_LIBRARIES( ${MY_LIB_NAME} debug ${dependency} )
	TARGET_LINK_LIBRARIES( ${MY_LIB_NAME} optimized ${dependency} )
endforeach(dependency ${APPLICATION_DEPENDENCIES})

TARGET_LINK_LIBRARIES( ${MY_LIB_NAME} ${THIRDPARTY_LIB_DIR}/${MENGINE_LIB_PREFIX}SDL2${MENGINE_LIB_SUFFIX})

if(MENGINE_PLATFORM_MAC OR MENGINE_PLATFORM_WIN32)
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

IF(WIN32)
  TARGET_LINK_LIBRARIES( ${MY_LIB_NAME} Winmm Version Imm32 OpenGL32) 
ENDIF()

IF(MSVC)
  set_target_properties( ${MY_LIB_NAME} PROPERTIES LINK_FLAGS /FORCE:MULTIPLE )
ENDIF()


