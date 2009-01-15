
ADD_FILTER(
LoggerConsole
	LoggerConsole.cpp
	LoggerConsole.h
)

ADD_FILTER(
Resources
	menge.ico
	resource.h
	WinApplication.rc
)
	
ADD_FILTER(
src
	main.cpp
	SystemDLL.cpp
	SystemDLL.h
	WinApplication.cpp
	WinApplication.h
	WinApplication.def
	WinTimer.cpp
	WinTimer.h
)


INCLUDE_DIRECTORIES( ${Mengine_SOURCE_DIR}/libs )
#MESSAGE( CMAKE_ARCHIVE_OUTPUT_DIRECTORY = "${CMAKE_ARCHIVE_OUTPUT_DIRECTORY}" )
LINK_DIRECTORIES( ${CMAKE_ARCHIVE_OUTPUT_DIRECTORY} )
ADD_EXECUTABLE( WinApplication WIN32 ${SRC_FILES} )
TARGET_LINK_LIBRARIES( WinApplication ${APPLICATION_LIBS} )

SET( LIB_PREFIX )
if(NOT MSVC)
	SET( LIB_PREFIX "lib" )
endif(NOT MSVC)
foreach(dependency ${APPLICATION_DEPENDENCIES})
	TARGET_LINK_LIBRARIES( WinApplication ${LIB_PREFIX}${dependency}${MENGINE_LIB_SUFFIX} )
endforeach(dependency ${APPLICATION_DEPENDENCIES})

if(MSVC)
	set_target_properties( WinApplication PROPERTIES LINK_FLAGS /NODEFAULTLIB:"libci.lib" )
endif(MSVC)