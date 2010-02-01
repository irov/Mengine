
set( MY_LIB_NAME MengeWin32FileSystem )

if( MENGE_USE_MT )
	set( MY_LIB_NAME ${MY_LIB_NAME}_mt )
endif( MENGE_USE_MT )

ADD_FILTER(
src
	#FileStream.h
	#FileStream.cpp
	Win32OutputStream.cpp
	Win32OutputStream.h
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