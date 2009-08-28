
set( MY_LIB_NAME MengeWin32FileSystem )

if( MENGE_USE_MD )
	set( MY_LIB_NAME ${MY_LIB_NAME}_md )
endif( MENGE_USE_MD )

ADD_FILTER(
src
	FileStream.h
	FileStream.cpp
	FileStreamOutStream.cpp
	FileStreamOutStream.h
	FileSystem.cpp
	FileSystem.h
)

ADD_LIBRARY( ${MY_LIB_NAME} ${SRC_FILES} )


SET( APPLICATION_DEPENDENCIES
 ${APPLICATION_DEPENDENCIES}
 ${MY_LIB_NAME}
)

SET( APPLICATION_DEPENDENCIES
 ${APPLICATION_DEPENDENCIES}
 PARENT_SCOPE
)