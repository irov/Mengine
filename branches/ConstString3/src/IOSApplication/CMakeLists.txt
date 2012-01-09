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
)

INCLUDE_DIRECTORIES( ${Mengine_SOURCE_DIR}/libs ${Mengine_SOURCE_DIR}/Utils )
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

SET(TARGETSDK iPhoneOS4.1.sdk)
SET(CMAKE_OSX_SYSROOT /Developer/Platforms/iPhoneOS.platform/Developer/SDKs/${TARGETSDK})

INCLUDE_DIRECTORIES ( /System/Library/Frameworks )
FIND_LIBRARY(Cocoa_LIBRARY Cocoa)
FIND_LIBRARY(AppKit_LIBRARY AppKit )
FIND_LIBRARY(CoreData_LIBRARY CoreData )
FIND_LIBRARY(Foundation_LIBRARY Foundation)
FIND_LIBRARY(OpenGL_LIBRARY OpenGL )
FIND_LIBRARY(OpenAL_LIBRARY OpenAL )

MARK_AS_ADVANCED (Cocoa_LIBRARY
                 AppKit_LIBRARY
                 CoreData_LIBRARY
                 Foundation_LIBRARY
                 OpenGL_LIBRARY
                 OpenAL_LIBRARY)
                 
SET(EXTRA_LIBS ${Cocoa_LIBRARY} ${AppKit_LIBRARY} ${CoreData_LIBRARY} ${Foundation_LIBRARY} ${OpenGL_LIBRARY} ${OpenAL_LIBRARY})

TARGET_LINK_LIBRARIES( IOSApplication ${EXTRA_LIBS})
                 
set(CMAKE_OSX_ARCHITECTURES "$(ARCHS_UNIVERSAL_IPHONE_OS)")
                 
#SET(EXTRA_LIBS ${Cocoa_LIBRARY} ${AppKit_LIBRARY} ${CoreData_LIBRARY} ${Foundation_LIBRARY} ${OpenGL_LIBRARY} ${OpenAL_LIBRARY})

#TARGET_LINK_LIBRARIES( MacOSApplication ${EXTRA_LIBS})
#TARGET_LINK_LIBRARIES( MacOSApplication ${THIRDPARTY_DIR}/astralax/mac/magic.dylib )

#set_property( TARGET MacOSApplication PROPERTY MACOSX_BUNDLE_INFO_PLIST ${CMAKE_CURRENT_SOURCE_DIR}/Mengine-Info.plist)