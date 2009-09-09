
set( MY_LIB_NAME MengeWin32FileSystem )

if( MENGE_USE_MD )
	set( MY_LIB_NAME ${MY_LIB_NAME}_md )
endif( MENGE_USE_MD )

ADD_FILTER(
src
	#FileStream.h
	#FileStream.cpp
	FileStreamOutStream.cpp
	FileStreamOutStream.h
	Win32FileSystem.cpp
	Win32FileSystem.h
	Win32InputStream.h
	Win32InputStream.cpp
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