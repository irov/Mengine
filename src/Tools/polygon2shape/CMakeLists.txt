set( MY_LIB_NAME polygon2shape )

PROJECT( ${MY_LIB_NAME} )

SET( SRC_FILES 
	main.cpp
	)

if (NOT MSVC)
	ADD_DEFINITIONS(-D"override = /* */")
endif(NOT MSVC)
	
INCLUDE_DIRECTORIES( 
	${TrimAlphaTool_SOURCE_DIR}/../Codecs
	${TrimAlphaTool_SOURCE_DIR}/../Utils
	${TrimAlphaTool_SOURCE_DIR}/../../../dependencies )
	
#LINK_DIRECTORIES( ${CMAKE_ARCHIVE_OUTPUT_DIRECTORY} )
ADD_EXECUTABLE( ${MY_LIB_NAME} ${SRC_FILES} )

TARGET_LINK_LIBRARIES(${MY_LIB_NAME} Core)