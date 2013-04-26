set( MY_LIB_NAME MengeWin32FileSystem )

ADD_FILTER(
src
	Win32OutputStream.cpp
	Win32OutputStream.h
	Win32FileSystem.cpp
	Win32FileSystem.h
	Win32InputStream.h
	Win32InputStream.cpp
	Win32MappedInputStream.h
	Win32MappedInputStream.cpp
)
add_definitions(-DUNICODE)

ADD_LIBRARY( ${MY_LIB_NAME} ${SRC_FILES} )

TARGET_LINK_LIBRARIES(${MY_LIB_NAME} WindowsLayer)

SET( APPLICATION_DEPENDENCIES
 ${APPLICATION_DEPENDENCIES}
 ${MY_LIB_NAME}
 Factory
 Core
)

SET( APPLICATION_DEPENDENCIES
 ${APPLICATION_DEPENDENCIES}
 PARENT_SCOPE
)

SET( APPLICATION_LIBS
 ${APPLICATION_LIBS}
 PARENT_SCOPE
)

SET( APPLICATION_LIB_PATHS
 ${APPLICATION_LIB_PATHS}
 PARENT_SCOPE
)