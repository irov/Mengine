CMAKE_MINIMUM_REQUIRED(VERSION 3.0)

project( theora )

SET( SOURCE_DIR ${THIRDPARTY_DIR}/libtheora )

INCLUDE_DIRECTORIES( ${SOURCE_DIR}/include )
INCLUDE_DIRECTORIES( ${SOURCE_DIR}/lib )

INCLUDE_DIRECTORIES( ${THIRDPARTY_DIR}/libogg/include )

# OUR SOURCES
set( sources
    ${SOURCE_DIR}/lib/analyze.c
    ${SOURCE_DIR}/lib/apiwrapper.c
    ${SOURCE_DIR}/lib/bitpack.c
	${SOURCE_DIR}/lib/cpu.c
    ${SOURCE_DIR}/lib/decapiwrapper.c
    ${SOURCE_DIR}/lib/decinfo.c
    ${SOURCE_DIR}/lib/decode.c
    ${SOURCE_DIR}/lib/dequant.c
    ${SOURCE_DIR}/lib/fdct.c
    ${SOURCE_DIR}/lib/fragment.c
    ${SOURCE_DIR}/lib/huffdec.c
    ${SOURCE_DIR}/lib/idct.c
    ${SOURCE_DIR}/lib/info.c
    ${SOURCE_DIR}/lib/internal.c
    ${SOURCE_DIR}/lib/mathops.c
    ${SOURCE_DIR}/lib/quant.c
    ${SOURCE_DIR}/lib/rate.c
    ${SOURCE_DIR}/lib/state.c
    ${SOURCE_DIR}/lib/tokenize.c
    )

set( sources_ext )

if(WIN32 AND MENGINE_ARCHITECTURE_X86)
    if(MENGINE_TOOLCHAIN_MSVC)
        add_definitions(-DOC_X86_ASM)

        set( sources_ext
            ${SOURCE_DIR}/lib/x86_vc/mmxencfrag.c
            ${SOURCE_DIR}/lib/x86_vc/mmxfdct.c
            ${SOURCE_DIR}/lib/x86_vc/mmxfrag.c
            ${SOURCE_DIR}/lib/x86_vc/mmxidct.c
            ${SOURCE_DIR}/lib/x86_vc/mmxstate.c
            ${SOURCE_DIR}/lib/x86_vc/x86state.c	
            )
    else()
        add_definitions(-DOC_X86_ASM)

        set( sources_ext
            ${SOURCE_DIR}/lib/x86/mmxencfrag.c
            ${SOURCE_DIR}/lib/x86/mmxfdct.c
            ${SOURCE_DIR}/lib/x86/mmxfrag.c
            ${SOURCE_DIR}/lib/x86/mmxidct.c
            ${SOURCE_DIR}/lib/x86/mmxstate.c
            ${SOURCE_DIR}/lib/x86/sse2fdct.c
            ${SOURCE_DIR}/lib/x86/x86state.c
            )
    endif()
endif()

add_library( ${PROJECT_NAME} STATIC ${sources} ${sources_ext} )
