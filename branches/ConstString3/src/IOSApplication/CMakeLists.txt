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

MainWindow.xib
main.m
)

#MESSAGE( CMAKE_ARCHIVE_OUTPUT_DIRECTORY = "${CMAKE_ARCHIVE_OUTPUT_DIRECTORY}" )
LINK_DIRECTORIES( ${CMAKE_ARCHIVE_OUTPUT_DIRECTORY} )
ADD_EXECUTABLE( IOSApplication MACOSX_BUNDLE ${SRC_FILES} )

foreach(dependency ${APPLICATION_DEPENDENCIES})
	MESSAGE( ${dependency} )
	TARGET_LINK_LIBRARIES( IOSApplication debug ${dependency} )
	TARGET_LINK_LIBRARIES( IOSApplication optimized ${dependency} )
endforeach(dependency ${APPLICATION_DEPENDENCIES})

TARGET_LINK_LIBRARIES( IOSApplication ${APPLICATION_LIBS} )
 

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
            
set_property( TARGET IOSApplication PROPERTY MACOSX_BUNDLE_INFO_PLIST ${CMAKE_CURRENT_SOURCE_DIR}/Mengine-Info.plist)

MESSAGE( ${CMAKE_CURRENT_SOURCE_DIR}/Mengine-Info.plist )