
CMAKE_MINIMUM_REQUIRED( VERSION 2.6 )
                                             
project(theora)

SET(TRUE_DIR ../../theora)
SET(theora_SOURCE_DIR ${TRUE_DIR})

INCLUDE( ../menge_macro.cmake )

# SET LIBNAME
# msvc does not append 'lib' - do it here to have consistent name
if(MSVC)
 set(theora_LIB_NAME lib)
endif(MSVC)
set( theora_LIB_NAME ${theora_LIB_NAME}theora_static )

# to distinguish between debug and release lib
set(CMAKE_DEBUG_POSTFIX "d")

# OUR SOURCES
set(libtheora_sources
	# disable encoder
	${TRUE_DIR}/lib/enc/encapiwrapper.c
	${TRUE_DIR}/lib/enc/encoder_disabled.c 
	# decoder
	${TRUE_DIR}/lib/dec/apiwrapper.c
	${TRUE_DIR}/lib/dec/bitpack.c
	${TRUE_DIR}/lib/dec/decapiwrapper.c
	${TRUE_DIR}/lib/dec/decinfo.c
	${TRUE_DIR}/lib/dec/decode.c
	${TRUE_DIR}/lib/dec/dequant.c
	${TRUE_DIR}/lib/dec/fragment.c
	${TRUE_DIR}/lib/dec/huffdec.c
	${TRUE_DIR}/lib/dec/idct.c
	${TRUE_DIR}/lib/dec/info.c
	${TRUE_DIR}/lib/dec/internal.c
	${TRUE_DIR}/lib/dec/quant.c
	${TRUE_DIR}/lib/dec/state.c
	${TRUE_DIR}/lib/cpu.c
)

# SOME NEEDED DEFINITIONS
if(MSVC)
  add_definitions(-D_CRT_SECURE_NO_DEPRECATE)
endif(MSVC)

include_directories(${theora_SOURCE_DIR}/include ${theora_SOURCE_DIR}/include/theora ../../ogg/include)
MENGINE_ADD_LIBRARY( ${theora_LIB_NAME} STATIC ${libtheora_sources} )
