
PROJECT( MNEConvertTool )

SET( SRC_FILES 
	main.cpp
	../Codecs/ImageEncoderJPEG.cpp
	../Codecs/ImageEncoderJPEG.h
	../Codecs/ImageDecoderPNG.cpp
	../Codecs/ImageDecoderPNG.h
	../Codecs/ImageEncoderPNG.cpp
	../Codecs/ImageEncoderPNG.h
	)

if (NOT MSVC)
	ADD_DEFINITIONS(-D"override = /* */")
endif(NOT MSVC)
	
INCLUDE_DIRECTORIES( 
	${MNEConvertTool_SOURCE_DIR}/../Codecs
	${MNEConvertTool_SOURCE_DIR}/../../ 
	${MNEConvertTool_SOURCE_DIR}/../../../dependencies 
	${MNEConvertTool_SOURCE_DIR}/../../../dependencies/zlib )
	
#LINK_DIRECTORIES( ${CMAKE_ARCHIVE_OUTPUT_DIRECTORY} )
ADD_EXECUTABLE( MNEConvertTool ${SRC_FILES} )

if(MINGW)
TARGET_LINK_LIBRARIES( MNEConvertTool
	${MNEConvertTool_SOURCE_DIR}/../../../dependencies/libPNG/build_mingw/libpng_static.a 
	${MNEConvertTool_SOURCE_DIR}/../../../dependencies/zlib/build_mingw/libzlib_static.a 
	${MNEConvertTool_SOURCE_DIR}/../../../dependencies/libJPEG/build_mingw/libjpeg_static.a 
	${MNEConvertTool_SOURCE_DIR}/../../../build_mingw/libMengeWin32FileSystem.a 
	)
elseif(MSVC)
TARGET_LINK_LIBRARIES( MNEConvertTool
	${MNEConvertTool_SOURCE_DIR}/../../../dependencies/libPNG/build_msvc8/png_staticd.lib 
	${MNEConvertTool_SOURCE_DIR}/../../../dependencies/zlib/build_msvc8/zlib_staticd.lib 
	${MNEConvertTool_SOURCE_DIR}/../../../dependencies/libJPEG/build_msvc8/jpeg_staticd.lib
	${MNEConvertTool_SOURCE_DIR}/../../../build_msvc8/debug/MengeWin32FileSystemd.lib
	)
endif(MINGW)
