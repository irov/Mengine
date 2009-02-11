
CMAKE_MINIMUM_REQUIRED( VERSION 2.6 )
                                             
project(theora)

SET(TRUE_DIR ../../theora)
SET(theora_SOURCE_DIR ${TRUE_DIR})

INCLUDE( ../menge_macro.cmake )

MENGE_SET_PROJECT_DEFAULTS( theora )

# OUR SOURCES
set(theora_sources
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

include_directories(${theora_SOURCE_DIR}/include ${theora_SOURCE_DIR}/include/theora ../../ogg/include)
add_library( ${theora_LIB_NAME} STATIC ${theora_sources} )
