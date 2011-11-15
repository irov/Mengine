
CMAKE_MINIMUM_REQUIRED( VERSION 2.6 )
                                             
project(theora)

SET( CMAKE_CURRENT_SOURCE_DIR ${CMAKE_SOURCE_DIR}/theora )

INCLUDE( ../menge_macro.cmake )

MENGE_SET_PROJECT_DEFAULTS( theora )

# OUR SOURCES
set(theora_sources
	# disable encoder
	${CMAKE_CURRENT_SOURCE_DIR}/lib/enc/encapiwrapper.c
	${CMAKE_CURRENT_SOURCE_DIR}/lib/enc/encoder_disabled.c 
	# decoder
	${CMAKE_CURRENT_SOURCE_DIR}/lib/dec/apiwrapper.c
	${CMAKE_CURRENT_SOURCE_DIR}/lib/dec/bitwise.c
	${CMAKE_CURRENT_SOURCE_DIR}/lib/dec/decapiwrapper.c
	${CMAKE_CURRENT_SOURCE_DIR}/lib/dec/decinfo.c
	${CMAKE_CURRENT_SOURCE_DIR}/lib/dec/decode.c
	${CMAKE_CURRENT_SOURCE_DIR}/lib/dec/dequant.c
	${CMAKE_CURRENT_SOURCE_DIR}/lib/dec/fragment.c
	${CMAKE_CURRENT_SOURCE_DIR}/lib/dec/huffdec.c
	${CMAKE_CURRENT_SOURCE_DIR}/lib/dec/idct.c
	${CMAKE_CURRENT_SOURCE_DIR}/lib/dec/info.c
	${CMAKE_CURRENT_SOURCE_DIR}/lib/dec/internal.c
	${CMAKE_CURRENT_SOURCE_DIR}/lib/dec/quant.c
	${CMAKE_CURRENT_SOURCE_DIR}/lib/dec/state.c
	${CMAKE_CURRENT_SOURCE_DIR}/lib/cpu.c
)

include_directories( 
	${CMAKE_CURRENT_SOURCE_DIR}/include 
	${CMAKE_CURRENT_SOURCE_DIR}/include/theora 
	${CMAKE_SOURCE_DIR}/../ogg/include)
	
add_library( ${theora_LIB_NAME} STATIC ${theora_sources} )
