set( MY_LIB_NAME WinApplication )

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
	WindowsIncluder.h
	FPSMonitor.cpp
	FPSMonitor.h	
	AlreadyRunningMonitor.h
	AlreadyRunningMonitor.cpp
	CriticalErrorsMonitor.h
	CriticalErrorsMonitor.cpp
	WinApplication.cpp
	WinApplication.h
	WinTimer.cpp
	WinTimer.h
	DynamicLibrary.h
	DynamicLibrary.cpp
	StringConversion.h
	StringConversion.cpp
)

if(MINGW)
    # resource compilation for mingw
    ADD_CUSTOM_COMMAND(OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/WinApplication_rc.o
                       COMMAND windres.exe -I${CMAKE_CURRENT_SOURCE_DIR}
                                           -i${CMAKE_CURRENT_SOURCE_DIR}/WinApplication.rc
                                           -o ${CMAKE_CURRENT_BINARY_DIR}/WinApplication_rc.o)
    SET(SRC_FILES ${SRC_FILES} ${CMAKE_CURRENT_BINARY_DIR}/WinApplication_rc.o)
  else(MINGW)
    SET(SRC_FILES ${SRC_FILES} WinApplication.rc)
  endif(MINGW)

INCLUDE_DIRECTORIES( ${Mengine_SOURCE_DIR}/Utils )
#MESSAGE( CMAKE_ARCHIVE_OUTPUT_DIRECTORY = "${CMAKE_ARCHIVE_OUTPUT_DIRECTORY}" )
LINK_DIRECTORIES( ${CMAKE_ARCHIVE_OUTPUT_DIRECTORY} )
ADD_EXECUTABLE( WinApplication WIN32 ${SRC_FILES} )

SET( LIB_PREFIX )
if(NOT MSVC)
	SET( LIB_PREFIX "lib" )
endif(NOT MSVC)

foreach(dependency ${APPLICATION_DEPENDENCIES})
	#MESSAGE( ${LIB_PREFIX}${dependency}${MENGINE_LIB_SUFFIX} )
	TARGET_LINK_LIBRARIES( WinApplication debug ${LIB_PREFIX}${dependency}${CMAKE_DEBUG_POSTFIX}${MENGINE_LIB_POSTFIX} )
	TARGET_LINK_LIBRARIES( WinApplication optimized ${LIB_PREFIX}${dependency}${MENGINE_LIB_POSTFIX} )
endforeach(dependency ${APPLICATION_DEPENDENCIES})

if(NOT MSVC)
	SET( APPLICATION_LIBS ${APPLICATION_LIBS} libole32.a )
endif(NOT MSVC)

TARGET_LINK_LIBRARIES( WinApplication ${APPLICATION_LIBS} comctl32 )

if(MSVC)
	set_target_properties( WinApplication PROPERTIES LINK_FLAGS /NODEFAULTLIB:"libci.lib" )
endif(MSVC)

if(MINGW)
	set_target_properties( WinApplication PROPERTIES LINK_FLAGS -Wl,-Map,${MENGINE_REPOSITORY}/bin/WinApplication.map )
endif(MINGW)