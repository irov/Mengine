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


# Set the OS X Bundle specific CMake variables which will be used to populate the plist for
# the application bundle
set(MACOSX_BUNDLE_GUI_IDENTIFIER "Mengine")
set(MACOSX_BUNDLE_BUNDLE_NAME ${PROJECT_NAME})

# These variables are specific to our plist and are NOT standard CMake variables
set(MACOSX_BUNDLE_NSMAIN_NIB_FILE "MainMenu")
set(MACOSX_BUNDLE_NSPRINCIPAL_CLASS "NSApplication")

# these are the OS X Interface Builder Files
set(MacOSApplication_XIBS
  MainMenu
)

# Make sure we can find the 'ibtool' program. If we can NOT find it we
# skip generation of this project
find_program(IBTOOL ibtool HINTS "/usr/bin" "${OSX_DEVELOPER_ROOT}/usr/bin")
if (${IBTOOL} STREQUAL "IBTOOL-NOTFOUND")
  message(SEND_ERROR "ibtool can not be found and is needed to compile the .xib files. It should have been installed with the Apple developer tools. The default system paths were searched in addition to ${OSX_DEVELOPER_ROOT}/usr/bin")
endif()

foreach(xib ${MacOSApplication_XIBS})
  add_custom_command (TARGET MacOSApplication POST_BUILD
                      COMMAND ${IBTOOL} --errors --warnings --notices --output-format human-readable-text --compile ${EXECUTABLE_OUTPUT_PATH}/\${CONFIGURATION}/MacOSApplication.app/Contents/Resources/${xib}.nib ${MacOSApplication_SOURCE_DIR}/${xib}.xib
                      COMMENT "Compiling ${SimpleCocoaVTK_SOURCE_DIR}/${xib}.xib")
endforeach()


TARGET_LINK_LIBRARIES( MacOSApplication ${EXTRA_LIBS})
TARGET_LINK_LIBRARIES( MacOSApplication ${THIRDPARTY_DIR}/astralax/mac/magic.dylib )

set_target_properties( MacOSApplication PROPERTIES LINK_FLAGS -Wl,-Map,${MENGINE_REPOSITORY}/bin/MacOSApplication.map LINK_FLAGS "-framework IOKit -framework CoreAudio -framework AudioToolbox -framework AudioUnit -framework Carbon -framework AGL -framework OpenGL -framework QuickTime" )
