set( MY_LIB_NAME MacOSApplication )

ADD_FILTER(
src
    MengineOpenGLView.mm
    MengineOpenGLView.h
    MacOSXApplication.mm
    MacOSXApplication.h
    AppDelegate.mm
    AppDelegate.h
    MengineWindow.mm
    MengineWindow.h
    MainMenu.xib
    main.m
)

INCLUDE_DIRECTORIES( ${Mengine_SOURCE_DIR}/libs ${Mengine_SOURCE_DIR}/Utils )
#MESSAGE( CMAKE_ARCHIVE_OUTPUT_DIRECTORY = "${CMAKE_ARCHIVE_OUTPUT_DIRECTORY}" )
LINK_DIRECTORIES( ${CMAKE_ARCHIVE_OUTPUT_DIRECTORY} )
ADD_EXECUTABLE( MacOSApplication MACOSX_BUNDLE ${SRC_FILES} )


SET( LIB_PREFIX "lib" )

foreach(dependency ${APPLICATION_DEPENDENCIES})
	#MESSAGE( ${LIB_PREFIX}${dependency}${MENGINE_LIB_SUFFIX} )
	TARGET_LINK_LIBRARIES( MacOSApplication debug ${LIB_PREFIX}${dependency}${CMAKE_DEBUG_POSTFIX}${MENGINE_LIB_SUFFIX} )
	TARGET_LINK_LIBRARIES( MacOSApplication optimized ${LIB_PREFIX}${dependency}${MENGINE_LIB_SUFFIX} )
endforeach(dependency ${APPLICATION_DEPENDENCIES})

TARGET_LINK_LIBRARIES( MacOSApplication ${APPLICATION_LIBS} )

IF(APPLE)
TARGET_LINK_LIBRARIES( MacOSApplication ${CMAKE_OSX_SYSROOT}/System/Library/Frameworks/Cocoa.framework )

TARGET_LINK_LIBRARIES( MacOSApplication ${CMAKE_OSX_SYSROOT}/System/Library/Frameworks/AppKit.framework )

TARGET_LINK_LIBRARIES( MacOSApplication ${CMAKE_OSX_SYSROOT}/System/Library/Frameworks/CoreData.framework )

TARGET_LINK_LIBRARIES( MacOSApplication ${CMAKE_OSX_SYSROOT}/System/Library/Frameworks/Foundation.framework )

TARGET_LINK_LIBRARIES( MacOSApplication ${CMAKE_OSX_SYSROOT}/System/Library/Frameworks/OpenGL.framework )

TARGET_LINK_LIBRARIES( MacOSApplication ${CMAKE_OSX_SYSROOT}/System/Library/Frameworks/OpenAL.framework )

TARGET_LINK_LIBRARIES( MacOSApplication ${CMAKE_OSX_SYSROOT}/System/Library/Frameworks/Python.framework )

TARGET_LINK_LIBRARIES( MacOSApplication ${THIRDPARTY_DIR}/astralax/mac/magic.dylib )
ENDIF(APPLE)

set_target_properties( MacOSApplication PROPERTIES LINK_FLAGS -Wl,-Map,${MENGINE_REPOSITORY}/bin/MacOSApplication.map LINK_FLAGS "-framework IOKit -framework CoreAudio -framework AudioToolbox -framework AudioUnit -framework Carbon -framework AGL -framework OpenGL -framework QuickTime" )
