set( MY_LIB_NAME IOSApplication )

ADD_FILTER(
src
    EAGLView.h
    EAGLView.mm
    ES1Renderer.h
    ES1Renderer.mm
    ESRenderer.h
    MengineAppDelegate.h
    MengineAppDelegate.mm
    
    iOSApplication.cpp
    iOSApplication.h
    iOSFileSystem.cpp
    iOSFileSystem.h
)

SET(TARGETSDK_NAME iPhoneSimulator)
SET(TARGETSDK_VERSION 5.0)
SET(CMAKE_OSX_SYSROOT /Developer/Platforms/${TARGETSDK_NAME}.platform/Developer/SDKs/${TARGETSDK_NAME}${TARGETSDK_VERSION}.sdk)
SET(CMAKE_FRAMEWORK_PATH ${CMAKE_OSX_SYSROOT}/System/Library/Frameworks)

set(CMAKE_OSX_ARCHITECTURES "$(ARCHS_STANDARD_32_BIT)")
set(CMAKE_XCODE_EFFECTIVE_PLATFORMS "-iphoneos;-iphonesimulator")


INCLUDE_DIRECTORIES( ${Mengine_SOURCE_DIR}/Utils ${Mengine_SOURCE_DIR}/Interface )
#MESSAGE( CMAKE_ARCHIVE_OUTPUT_DIRECTORY = "${CMAKE_ARCHIVE_OUTPUT_DIRECTORY}" )
LINK_DIRECTORIES( ${CMAKE_ARCHIVE_OUTPUT_DIRECTORY} )
ADD_EXECUTABLE( IOSApplication MACOSX_BUNDLE ${SRC_FILES} )

SET( LIB_PREFIX "lib" )

foreach(dependency ${APPLICATION_DEPENDENCIES})
	#MESSAGE( ${LIB_PREFIX}${dependency}${MENGINE_LIB_SUFFIX} )
	TARGET_LINK_LIBRARIES( IOSApplication debug ${LIB_PREFIX}${dependency}${CMAKE_DEBUG_POSTFIX}${MENGINE_LIB_SUFFIX} )
	TARGET_LINK_LIBRARIES( IOSApplication optimized ${LIB_PREFIX}${dependency}${MENGINE_LIB_SUFFIX} )
endforeach(dependency ${APPLICATION_DEPENDENCIES})

TARGET_LINK_LIBRARIES( IOSApplication ${APPLICATION_LIBS} )
 

#INCLUDE_DIRECTORIES ( ${CMAKE_OSX_SYSROOT}/System/Library/Frameworks )


FIND_LIBRARY(QuartzCore_LIBRARY QuartzCore)
FIND_LIBRARY(OpenGLES_LIBRARY OpenGLES)
FIND_LIBRARY(OpenAL_LIBRARY OpenAL)
FIND_LIBRARY(UIKit_LIBRARY UIKit)
FIND_LIBRARY(Foundation_LIBRARY Foundation)

MARK_AS_ADVANCED (QuartzCore_LIBRARY
		OpenGLES_LIBRARY
                 OpenAL_LIBRARY
                 UIKit_LIBRARY
		 Foundation_LIBRARY)
                 
SET(EXTRA_LIBS ${QuartzCore_LIBRARY} ${OpenGLES_LIBRARY} ${OpenAL_LIBRARY} ${UIKit_LIBRARY} ${Foundation_LIBRARY})

TARGET_LINK_LIBRARIES( IOSApplication ${EXTRA_LIBS})
                 
set(CMAKE_OSX_ARCHITECTURES "$(ARCHS_UNIVERSAL_IPHONE_OS)")
                 
#SET(EXTRA_LIBS ${Cocoa_LIBRARY} ${AppKit_LIBRARY} ${CoreData_LIBRARY} ${Foundation_LIBRARY} ${OpenGL_LIBRARY} ${OpenAL_LIBRARY})

#TARGET_LINK_LIBRARIES( MacOSApplication ${EXTRA_LIBS})
#TARGET_LINK_LIBRARIES( MacOSApplication ${THIRDPARTY_DIR}/astralax/mac/magic.dylib )

#set_property( TARGET MacOSApplication PROPERTY MACOSX_BUNDLE_INFO_PLIST ${CMAKE_CURRENT_SOURCE_DIR}/Mengine-Info.plist)