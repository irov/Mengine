set( MY_LIB_NAME WinApplication )

ADD_FILTER(
MessageBoxLogger
    MessageBoxLogger.cpp
    MessageBoxLogger.h
)

#ADD_FILTER(
#Resources
#	menge.ico
#	resource.h
#	WinApplication.rc
#)

ADD_FILTER(
CriticalErrorsMonitor
	CriticalErrorsMonitor.h
	CriticalErrorsMonitor.cpp
)
	
ADD_FILTER(
src
	main.cpp	

	WinApplication.cpp
	WinApplication.h
)

add_definitions(-DUNICODE)

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

INCLUDE_DIRECTORIES( 
    ${Mengine_SOURCE_DIR}/Utils
    ${THIRDPARTY_DIR}/pybind/include
    ${THIRDPARTY_DIR}/tinyini/include
    )
    
    
#MESSAGE( CMAKE_ARCHIVE_OUTPUT_DIRECTORY = "${CMAKE_ARCHIVE_OUTPUT_DIRECTORY}" )
LINK_DIRECTORIES( ${CMAKE_ARCHIVE_OUTPUT_DIRECTORY} )
ADD_EXECUTABLE( ${MY_LIB_NAME} WIN32 ${SRC_FILES} )

foreach(dependency ${APPLICATION_DEPENDENCIES})
	#MESSAGE( ${LIB_PREFIX}${dependency}${MENGINE_LIB_SUFFIX} )
	TARGET_LINK_LIBRARIES( ${MY_LIB_NAME} debug ${dependency} )
	TARGET_LINK_LIBRARIES( ${MY_LIB_NAME} optimized ${dependency} )
endforeach(dependency ${APPLICATION_DEPENDENCIES})

TARGET_LINK_LIBRARIES( ${MY_LIB_NAME} ${APPLICATION_LIBS} ) 

if(NOT MSVC)
    TARGET_LINK_LIBRARIES( ${MY_LIB_NAME} libole32.a )
endif(NOT MSVC)

if(MSVC)
	set_target_properties( ${MY_LIB_NAME} PROPERTIES LINK_FLAGS /NODEFAULTLIB:"libci.lib" )
endif(MSVC)

if(MINGW)
	set_target_properties( ${MY_LIB_NAME} PROPERTIES LINK_FLAGS -Wl,-Map,${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/WinApplication.map )
endif(MINGW)
