
PROJECT( AtlasTool )

SET( SRC_FILES 
	main.cpp
	atlas.h
	atlas.cpp
	ImageDecoderJPEG.cpp
	ImageDecoderJPEG.h
	ImageDecoderPNG.cpp
	ImageDecoderPNG.h
	ImageEncoderPNG.cpp
	ImageEncoderPNG.h
	)

if (NOT MSVC)
	ADD_DEFINITIONS(-D"override = /* */")
endif(NOT MSVC)
	
INCLUDE_DIRECTORIES( 
	${AtlasTool_SOURCE_DIR}/../../ 
	${AtlasTool_SOURCE_DIR}/../../../dependencies 
	${AtlasTool_SOURCE_DIR}/../../../dependencies/zlib )
	
#LINK_DIRECTORIES( ${CMAKE_ARCHIVE_OUTPUT_DIRECTORY} )
ADD_EXECUTABLE( AtlasTool ${SRC_FILES} )

if(MINGW)
TARGET_LINK_LIBRARIES( AtlasTool
	${AtlasTool_SOURCE_DIR}/../../../dependencies/libPNG/build_mingw/libpng_static.a 
	${AtlasTool_SOURCE_DIR}/../../../dependencies/zlib/build_mingw/libzlib_static.a 
	${AtlasTool_SOURCE_DIR}/../../../dependencies/libJPEG/build_mingw/libjpeg_static.a 
	${AtlasTool_SOURCE_DIR}/../../../build_mingw/libMengeWin32FileSystem.a 
	)
elseif(MSVC)
TARGET_LINK_LIBRARIES( AtlasTool
	${AtlasTool_SOURCE_DIR}/../../../dependencies/libPNG/build_msvc8/png_staticd.lib 
	${AtlasTool_SOURCE_DIR}/../../../dependencies/zlib/build_msvc8/zlib_staticd.lib 
	${AtlasTool_SOURCE_DIR}/../../../dependencies/libJPEG/build_msvc8/jpeg_staticd.lib
	${AtlasTool_SOURCE_DIR}/../../../build_msvc8/debug/MengeWin32FileSystemd.lib
	)
endif(MINGW)