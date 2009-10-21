
ADD_FILTER(
LoggerConsole
	LoggerConsole.cpp
	LoggerConsole.h
)

#ADD_FILTER(
#Resources
#	menge.ico
#	resource.h
#	WinApplication.rc
#)
	
ADD_FILTER(
src
	main.cpp
	MacOSApplication.cpp
	MacOSApplication.h
	OSXTimer.cpp
	OSXTimer.h
)

INCLUDE_DIRECTORIES( ${Mengine_SOURCE_DIR}/libs )
#MESSAGE( CMAKE_ARCHIVE_OUTPUT_DIRECTORY = "${CMAKE_ARCHIVE_OUTPUT_DIRECTORY}" )
LINK_DIRECTORIES( ${CMAKE_ARCHIVE_OUTPUT_DIRECTORY} )
ADD_EXECUTABLE( MacOSApplication ${SRC_FILES} )


SET( LIB_PREFIX "lib" )

foreach(dependency ${APPLICATION_DEPENDENCIES})
	#MESSAGE( ${LIB_PREFIX}${dependency}${MENGINE_LIB_SUFFIX} )
	TARGET_LINK_LIBRARIES( MacOSApplication debug ${LIB_PREFIX}${dependency}${CMAKE_DEBUG_POSTFIX}${MENGINE_LIB_SUFFIX} )
	TARGET_LINK_LIBRARIES( MacOSApplication optimized ${LIB_PREFIX}${dependency}${MENGINE_LIB_SUFFIX} )
endforeach(dependency ${APPLICATION_DEPENDENCIES})

TARGET_LINK_LIBRARIES( MacOSApplication ${APPLICATION_LIBS} )

set_target_properties( MacOSApplication PROPERTIES LINK_FLAGS -Wl,-Map,${MENGINE_REPOSITORY}/bin/MacOSApplication.map LINK_FLAGS "-framework IOKit -framework CoreAudio -framework AudioToolbox -framework AudioUnit -framework Carbon -framework AGL -framework OpenGL" )
