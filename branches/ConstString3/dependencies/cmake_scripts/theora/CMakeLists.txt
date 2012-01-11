
CMAKE_MINIMUM_REQUIRED( VERSION 2.6 )
                                             
project(theora)

SET( SOURCE_DIR ${CMAKE_SOURCE_DIR}/theora )

# OUR SOURCES
set(theora_sources
	# disable encoder
	${SOURCE_DIR}/lib/enc/encapiwrapper.c
	${SOURCE_DIR}/lib/enc/encoder_disabled.c 
	# decoder
	${SOURCE_DIR}/lib/dec/apiwrapper.c
	${SOURCE_DIR}/lib/dec/bitwise.c
	${SOURCE_DIR}/lib/dec/decapiwrapper.c
	${SOURCE_DIR}/lib/dec/decinfo.c
	${SOURCE_DIR}/lib/dec/decode.c
	${SOURCE_DIR}/lib/dec/dequant.c
	${SOURCE_DIR}/lib/dec/fragment.c
	${SOURCE_DIR}/lib/dec/huffdec.c
	${SOURCE_DIR}/lib/dec/idct.c
	${SOURCE_DIR}/lib/dec/info.c
	${SOURCE_DIR}/lib/dec/internal.c
	${SOURCE_DIR}/lib/dec/quant.c
	${SOURCE_DIR}/lib/dec/state.c
	${SOURCE_DIR}/lib/cpu.c
)

include_directories( 
	${SOURCE_DIR}/include 
	${SOURCE_DIR}/include/theora 
	${CMAKE_SOURCE_DIR}/../ogg/include)
	
add_library( ${PROJECT_NAME}_static STATIC ${theora_sources} )
