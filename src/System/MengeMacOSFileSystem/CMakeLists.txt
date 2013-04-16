
set( MY_LIB_NAME MengeMacOSFileSystem )

ADD_FILTER(
src
	FileStreamDataStream.cpp
	FileStreamDataStream.h
	FileStreamOutStream.cpp
	FileStreamOutStream.h
	MemoryDataStream.cpp
	MemoryDataStream.h
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