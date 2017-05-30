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
)

#MESSAGE( CMAKE_ARCHIVE_OUTPUT_DIRECTORY = "${CMAKE_ARCHIVE_OUTPUT_DIRECTORY}" )
LINK_DIRECTORIES( ${CMAKE_ARCHIVE_OUTPUT_DIRECTORY} )

if(WIN32)
ADD_EXECUTABLE( ${MY_LIB_NAME} WIN32 ${SRC_FILES} )
endif()

if(APPLE)
#set(MACOSX_BUNDLE true)
#set(MACOSX_BUNDLE_INFO_PLIST "~/Desktop/Info.plist.in")

#set(MACOSX_BUNDLE_BUNDLE_NAME "The Wisbey Mystery")
#set(MACOSX_BUNDLE_DISPLAY_NAME "The Wisbey Mystery")
#set(MACOSX_BUNDLE_COPYRIGHT "Argali LLC 2017")
#set(MACOSX_BUNDLE_ICON_FILE icon.icns)

set(myApp_ICON ~/Desktop/icon.icns)
set_source_files_properties(${myApp_ICON} PROPERTIES
       MACOSX_PACKAGE_LOCATION "Resources")

set(myApp_ASTRALAX ~/Desktop/magic3d.dylib)
set_source_files_properties(${myApp_ASTRALAX} PROPERTIES
       MACOSX_PACKAGE_LOCATION "MacOS")

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "~/Desktop")
       
ADD_EXECUTABLE( ${MY_LIB_NAME} MACOSX_BUNDLE ${myApp_ICON} ${myApp_ASTRALAX} ${SRC_FILES} )

set_target_properties( ${MY_LIB_NAME} PROPERTIES MACOSX_BUNDLE_INFO_PLIST Info.plist.in)
endif()

if (WIN32) #disable console mode for VC++
	set_target_properties( ${MY_LIB_NAME} PROPERTIES WIN32_EXECUTABLE TRUE)
endif(WIN32)

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

if(APPLE)
INCLUDE_DIRECTORIES ( /System/Library/Frameworks )

FIND_LIBRARY(Cocoa_LIBRARY Cocoa)
FIND_LIBRARY(Carbon_LIBRARY Carbon)
FIND_LIBRARY(AppKit_LIBRARY AppKit )
FIND_LIBRARY(CoreData_LIBRARY CoreData )
FIND_LIBRARY(CoreAudio_LIBRARY CoreAudio )
FIND_LIBRARY(CoreVideo_LIBRARY CoreVideo )
FIND_LIBRARY(CoreFoundation_LIBRARY CoreFoundation )
FIND_LIBRARY(AudioUnit_LIBRARY AudioUnit)
FIND_LIBRARY(AudioToolBox_LIBRARY AudioToolBox)
FIND_LIBRARY(IOKit_LIBRARY IOKit )
FIND_LIBRARY(ForceFeedback_LIBRARY ForceFeedback )
FIND_LIBRARY(Foundation_LIBRARY Foundation )
FIND_LIBRARY(OpenGL_LIBRARY OpenGL )
FIND_LIBRARY(OpenAL_LIBRARY OpenAL )

MARK_AS_ADVANCED (Cocoa_LIBRARY
		 Carbon_LIBRARY
                 AppKit_LIBRARY
                 CoreData_LIBRARY
		 CoreAudio_LIBRARY
		 CoreFoundation_LIBRARY
		AudioUnit_LIBRARY
		 AudioToolBox_LIBRARY
		 IOKit_LIBRARY
                 ForceFeedback_LIBRARY
		 CoreVideo_LIBRARY
                 Foundation_LIBRARY
                 OpenGL_LIBRARY
		OpenAL_LIBRARY)

SET(EXTRA_LIBS ${Cocoa_LIBRARY} ${Carbon_LIBRARY} ${AppKit_LIBRARY} ${CoreData_LIBRARY} ${CoreAudio_LIBRARY} ${CoreFoundation_LIBRARY} ${ForceFeedback_LIBRARY} ${IOKit_LIBRARY} ${CoreVideo_LIBRARY} ${AudioUnit_LIBRARY} ${AudioToolBox_LIBRARY} ${Foundation_LIBRARY} ${OpenGL_LIBRARY} ${OpenAL_LIBRARY})

TARGET_LINK_LIBRARIES( ${MY_LIB_NAME} ${EXTRA_LIBS})
TARGET_LINK_LIBRARIES( ${MY_LIB_NAME} iconv)
endif()

if(WIN32)
TARGET_LINK_LIBRARIES( ${MY_LIB_NAME} Winmm Version Imm32 OpenGL32) 
endif()

if(MSVC)
    set_target_properties( ${MY_LIB_NAME} PROPERTIES LINK_FLAGS /FORCE:MULTIPLE )
endif(MSVC)


