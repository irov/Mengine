
ADD_FILTER(
src
	FileStream.h
	FileStream.cpp
	DataStream.cpp
	DataStream.h
	FileHandleDataStream.cpp
	FileHandleDataStream.h
	FileStreamDataStream.cpp
	FileStreamDataStream.h
	FileStreamOutStream.cpp
	FileStreamOutStream.h
	MemoryDataStream.cpp
	MemoryDataStream.h
	FileSystem.cpp
	FileSystem.h
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