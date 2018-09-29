CMAKE_MINIMUM_REQUIRED( VERSION 2.6 )
                                             
project(theora)

INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR}/include)
INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR}/lib)

INCLUDE_DIRECTORIES(${THIRDPARTY_DIR}/libogg/include)
INCLUDE_DIRECTORIES(${THIRDPARTY_DIR}/stdex/include)

# OUR SOURCES
set( sources
    ${CMAKE_CURRENT_SOURCE_DIR}/lib/analyze.c
    ${CMAKE_CURRENT_SOURCE_DIR}/lib/apiwrapper.c
    ${CMAKE_CURRENT_SOURCE_DIR}/lib/bitpack.c    
	${CMAKE_CURRENT_SOURCE_DIR}/lib/cpu.c
    ${CMAKE_CURRENT_SOURCE_DIR}/lib/decapiwrapper.c    
    ${CMAKE_CURRENT_SOURCE_DIR}/lib/decinfo.c
    ${CMAKE_CURRENT_SOURCE_DIR}/lib/decode.c
    ${CMAKE_CURRENT_SOURCE_DIR}/lib/dequant.c    
    ${CMAKE_CURRENT_SOURCE_DIR}/lib/fdct.c
    ${CMAKE_CURRENT_SOURCE_DIR}/lib/fragment.c
    ${CMAKE_CURRENT_SOURCE_DIR}/lib/huffdec.c
    ${CMAKE_CURRENT_SOURCE_DIR}/lib/idct.c
    ${CMAKE_CURRENT_SOURCE_DIR}/lib/info.c
    ${CMAKE_CURRENT_SOURCE_DIR}/lib/internal.c
    ${CMAKE_CURRENT_SOURCE_DIR}/lib/mathops.c
    ${CMAKE_CURRENT_SOURCE_DIR}/lib/quant.c
    ${CMAKE_CURRENT_SOURCE_DIR}/lib/rate.c
    ${CMAKE_CURRENT_SOURCE_DIR}/lib/state.c
    ${CMAKE_CURRENT_SOURCE_DIR}/lib/tokenize.c
    )
	
set( sources_ext )	
	
if(WIN32 AND NOT MENGINE_PLATFORM_WIN64)
	if(MSVC)
		add_definitions(-DOC_X86_ASM)
		
		set( sources_ext
			${CMAKE_CURRENT_SOURCE_DIR}/lib/x86_vc/mmxencfrag.c
			${CMAKE_CURRENT_SOURCE_DIR}/lib/x86_vc/mmxfdct.c
			${CMAKE_CURRENT_SOURCE_DIR}/lib/x86_vc/mmxfrag.c
			${CMAKE_CURRENT_SOURCE_DIR}/lib/x86_vc/mmxidct.c
			${CMAKE_CURRENT_SOURCE_DIR}/lib/x86_vc/mmxstate.c
			${CMAKE_CURRENT_SOURCE_DIR}/lib/x86_vc/x86state.c	
			)
	else()
		add_definitions(-DOC_X86_ASM)		
	
		set( sources_ext
			${CMAKE_CURRENT_SOURCE_DIR}/lib/x86/mmxencfrag.c
			${CMAKE_CURRENT_SOURCE_DIR}/lib/x86/mmxfdct.c
			${CMAKE_CURRENT_SOURCE_DIR}/lib/x86/mmxfrag.c
			${CMAKE_CURRENT_SOURCE_DIR}/lib/x86/mmxidct.c
			${CMAKE_CURRENT_SOURCE_DIR}/lib/x86/mmxstate.c			
			${CMAKE_CURRENT_SOURCE_DIR}/lib/x86/sse2fdct.c
			${CMAKE_CURRENT_SOURCE_DIR}/lib/x86/x86state.c	
			)
	endif()
endif()	

add_library(${PROJECT_NAME} STATIC ${sources} ${sources_ext})
