set( MY_LIB_NAME SDLApplication )

ADD_FILTER(
src
	main.cpp	
	SDLApplication.cpp
	SDLApplication.h
    SDLInput.cpp
    SDLInput.h
    SDLLayer.cpp
    SDLLayer.h
    SDLLogger.cpp
    SDLLogger.h
    SDLTimer.cpp
    SDLTimer.h
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
ADD_EXECUTABLE( ${MY_LIB_NAME} WIN32 ${SRC_FILES} )

foreach(dependency ${APPLICATION_DEPENDENCIES})
	#MESSAGE( ${LIB_PREFIX}${dependency}${MENGINE_LIB_SUFFIX} )
	TARGET_LINK_LIBRARIES( ${MY_LIB_NAME} debug ${dependency} )
	TARGET_LINK_LIBRARIES( ${MY_LIB_NAME} optimized ${dependency} )
endforeach(dependency ${APPLICATION_DEPENDENCIES})

TARGET_LINK_LIBRARIES( ${MY_LIB_NAME} ${THIRDPARTY_DIR}/${MENGINE_LIB_DIR}/${CMAKE_BUILD_TYPE}/${MENGINE_LIB_PREFIX}SDL2${MENGINE_LIB_SUFFIX})

TARGET_LINK_LIBRARIES( ${MY_LIB_NAME} ${APPLICATION_LIBS} ) 