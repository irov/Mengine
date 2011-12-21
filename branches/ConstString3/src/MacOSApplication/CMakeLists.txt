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


INCLUDE_DIRECTORIES ( /System/Library/Frameworks )
FIND_LIBRARY(Cocoa_LIBRARY Cocoa)
FIND_LIBRARY(AppKit_LIBRARY AppKit )
FIND_LIBRARY(CoreData_LIBRARY CoreData )
FIND_LIBRARY(Foundation_LIBRARY Foundation)
FIND_LIBRARY(OpenGL_LIBRARY OpenGL )
FIND_LIBRARY(OpenAL_LIBRARY OpenAL )
FIND_LIBRARY(Python_LIBRARY Python )

MARK_AS_ADVANCED (Cocoa_LIBRARY
                 AppKit_LIBRARY
                 CoreData_LIBRARY
                 Foundation_LIBRARY
                 OpenGL_LIBRARY
                 OpenAL_LIBRARY
                 Python_LIBRARY)
                 
SET(EXTRA_LIBS ${Cocoa_LIBRARY} ${AppKit_LIBRARY} ${CoreData_LIBRARY} ${Foundation_LIBRARY} ${OpenGL_LIBRARY} ${OpenAL_LIBRARY} ${Python_LIBRARY})

TARGET_LINK_LIBRARIES( MacOSApplication ${EXTRA_LIBS})
TARGET_LINK_LIBRARIES( MacOSApplication ${THIRDPARTY_DIR}/astralax/mac/magic.dylib )

set_target_properties( MacOSApplication PROPERTIES LINK_FLAGS -Wl,-Map,${MENGINE_REPOSITORY}/bin/MacOSApplication.map LINK_FLAGS "-framework IOKit -framework CoreAudio -framework AudioToolbox -framework AudioUnit -framework Carbon -framework AGL -framework OpenGL -framework QuickTime" )
