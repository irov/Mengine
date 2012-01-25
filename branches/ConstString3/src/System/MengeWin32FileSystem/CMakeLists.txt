
set( MY_LIB_NAME MengeWin32FileSystem )

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
add_definitions(-DUNICODE)
ADD_LIBRARY( ${MY_LIB_NAME} ${SRC_FILES} )


SET( APPLICATION_DEPENDENCIES
 ${APPLICATION_DEPENDENCIES}
 ${MY_LIB_NAME}
)

SET( APPLICATION_DEPENDENCIES
 ${APPLICATION_DEPENDENCIES}
 PARENT_SCOPE
)