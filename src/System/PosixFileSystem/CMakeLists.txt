
set( MY_LIB_NAME PosixFileSystem )

if( MENGE_USE_MD )
	set( MY_LIB_NAME ${MY_LIB_NAME}_md )
endif( MENGE_USE_MD )

ADD_FILTER(
src
	PosixFileSystem.cpp
	PosixFileSystem.h
	PosixInputStream.cpp
	PosixInputStream.h
	PosixOutputStream.cpp
	PosixOutputStream.h
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