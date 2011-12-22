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
    Mengine-Info.plist
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


# Set the OS X Bundle specific CMake variables which will be used to populate the plist for
# the application bundle
set(MACOSX_BUNDLE_GUI_IDENTIFIER "Mengine")
set(MACOSX_BUNDLE_BUNDLE_NAME ${PROJECT_NAME})

# These variables are specific to our plist and are NOT standard CMake variables
set(MACOSX_BUNDLE_NSMAIN_NIB_FILE "MainMenu")
set(MACOSX_BUNDLE_NSPRINCIPAL_CLASS "NSApplication")


TARGET_LINK_LIBRARIES( MacOSApplication ${EXTRA_LIBS})
TARGET_LINK_LIBRARIES( MacOSApplication ${THIRDPARTY_DIR}/astralax/mac/magic.dylib )
set_target_properties( MacOSApplication PROPERTIES MACOSX_BUNDLE_INFO_PLIST ${MacOSApplication_BINARY_DIR}/MacOSApplication.plist)

#set_target_properties( MacOSApplication PROPERTIES MACOSX_BUNDLE_INFO_PLIST ${MacOSApplication_SOURCE_DIR}/Mengine-Info.plist)
