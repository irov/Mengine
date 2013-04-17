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

add_definitions(-D_UNICODE)
add_definitions(-DSI_CONVERT_ICU)

INCLUDE_DIRECTORIES( 
    ${Mengine_SOURCE_DIR}/Utils
    ${THIRDPARTY_DIR}/simpleini
    ${THIRDPARTY_DIR}/icu/source/common
) 

#SET(MENGINE_IPAD_SIMULATOR ${MENGINE_IPAD_SIMULATOR})

IF(${MENGINE_TARGET} STREQUAL "IPAD_SIMULATOR")
	ADD_DEFINITIONS(-x objective-c++)
ENDIF(${MENGINE_TARGET} STREQUAL "IPAD_SIMULATOR")

#MESSAGE( CMAKE_ARCHIVE_OUTPUT_DIRECTORY = "${CMAKE_ARCHIVE_OUTPUT_DIRECTORY}" )
LINK_DIRECTORIES( ${CMAKE_ARCHIVE_OUTPUT_DIRECTORY} )
ADD_EXECUTABLE( IOSApplication MACOSX_BUNDLE ${SRC_FILES} )

foreach(dependency ${APPLICATION_DEPENDENCIES})
	MESSAGE( ${dependency} )
	TARGET_LINK_LIBRARIES( IOSApplication debug ${dependency} )
	TARGET_LINK_LIBRARIES( IOSApplication optimized ${dependency} )
endforeach(dependency ${APPLICATION_DEPENDENCIES})


SET( APPLICATION_LIBS
 ${APPLICATION_LIBS}
${THIRDPARTY_DIR}/${MENGINE_LIB_DIR}/${CMAKE_BUILD_TYPE}/${MENGINE_LIB_PREFIX}icu_static${MENGINE_LIB_SUFFIX}  
 ${THIRDPARTY_DIR}/${MENGINE_LIB_DIR}/${CMAKE_BUILD_TYPE}/${MENGINE_LIB_PREFIX}simpleini_static${MENGINE_LIB_SUFFIX}
)

TARGET_LINK_LIBRARIES( IOSApplication ${APPLICATION_LIBS} )
 
FIND_LIBRARY(QuartzCore_LIBRARY QuartzCore)
FIND_LIBRARY(OpenGLES_LIBRARY OpenGLES)
FIND_LIBRARY(OpenAL_LIBRARY OpenAL)
FIND_LIBRARY(UIKit_LIBRARY UIKit)
FIND_LIBRARY(Foundation_LIBRARY Foundation)
FIND_LIBRARY(AudioToolbox_LIBRARY AudioToolbox)

                 
SET(EXTRA_LIBS ${QuartzCore_LIBRARY} ${OpenGLES_LIBRARY} ${OpenAL_LIBRARY} ${UIKit_LIBRARY} ${Foundation_LIBRARY} ${AudioToolbox_LIBRARY})

TARGET_LINK_LIBRARIES( IOSApplication ${EXTRA_LIBS})
            
set_property( TARGET IOSApplication PROPERTY MACOSX_BUNDLE_INFO_PLIST ${CMAKE_CURRENT_SOURCE_DIR}/Mengine-Info.plist)


MESSAGE( ${CMAKE_CURRENT_SOURCE_DIR}/Mengine-Info.plist )