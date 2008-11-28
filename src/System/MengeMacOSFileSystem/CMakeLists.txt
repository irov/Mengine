
ADD_FILTER(
src
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
	FileSystem.def
)

ADD_LIBRARY( MengeMacOSFileSystem ${SRC_FILES} )