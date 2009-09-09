
PROJECT( TrimAlphaTool )

SET( SRC_FILES 
	main.cpp
	../Codecs/ImageDecoderJPEG.cpp
	../Codecs/ImageDecoderJPEG.h
	../Codecs/ImageDecoderPNG.cpp
	../Codecs/ImageDecoderPNG.h
	../Codecs/ImageEncoderPNG.cpp
	../Codecs/ImageEncoderPNG.h
	)

if (NOT MSVC)
	ADD_DEFINITIONS(-D"override = /* */")
endif(NOT MSVC)
	
INCLUDE_DIRECTORIES( 
	${TrimAlphaTool_SOURCE_DIR}/../Codecs
	${TrimAlphaTool_SOURCE_DIR}/../../ 
	${TrimAlphaTool_SOURCE_DIR}/../../../dependencies 
	${TrimAlphaTool_SOURCE_DIR}/../../../dependencies/zlib )
	
#LINK_DIRECTORIES( ${CMAKE_ARCHIVE_OUTPUT_DIRECTORY} )
ADD_EXECUTABLE( TrimAlphaTool ${SRC_FILES} )

if(MINGW)
TARGET_LINK_LIBRARIES( TrimAlphaTool
	${TrimAlphaTool_SOURCE_DIR}/../../../dependencies/libPNG/build_mingw/libpng_static.a 
	${TrimAlphaTool_SOURCE_DIR}/../../../dependencies/zlib/build_mingw/libzlib_static.a 
	${TrimAlphaTool_SOURCE_DIR}/../../../dependencies/libJPEG/build_mingw/libjpeg_static.a 
	${TrimAlphaTool_SOURCE_DIR}/../../../build_mingw/libMengeWin32FileSystem.a 
	)
elseif(MSVC)
TARGET_LINK_LIBRARIES( TrimAlphaTool
	${TrimAlphaTool_SOURCE_DIR}/../../../dependencies/libPNG/build_msvc8/png_staticd.lib 
	${TrimAlphaTool_SOURCE_DIR}/../../../dependencies/zlib/build_msvc8/zlib_staticd.lib 
	${TrimAlphaTool_SOURCE_DIR}/../../../dependencies/libJPEG/build_msvc8/jpeg_staticd.lib
	${TrimAlphaTool_SOURCE_DIR}/../../../build_msvc8/debug/MengeWin32FileSystemd.lib
	)
endif(MINGW)
