
ADD_FILTER(
src
	FileStream.h
	FileStream.cpp
	FileStreamOutStream.cpp
	FileStreamOutStream.h
	MemoryDataStream.cpp
	MemoryDataStream.h
	FileSystem.cpp
	FileSystem.h
	DataStream.h
)

ADD_LIBRARY( MengeWin32FileSystem ${SRC_FILES} )


SET( APPLICATION_DEPENDENCIES
 ${APPLICATION_DEPENDENCIES}
 MengeWin32FileSystem
)

SET( APPLICATION_DEPENDENCIES
 ${APPLICATION_DEPENDENCIES}
 PARENT_SCOPE
)