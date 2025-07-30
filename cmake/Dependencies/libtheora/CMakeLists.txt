CMAKE_MINIMUM_REQUIRED(VERSION 3.10)

project(theora)

SET(SOURCE_DIR ${THIRDPARTY_DIR}/libtheora)

INCLUDE_DIRECTORIES(${SOURCE_DIR}/include)
INCLUDE_DIRECTORIES(${SOURCE_DIR}/lib)

INCLUDE_DIRECTORIES(${OGG_INCLUDE_DIRS})

# OUR SOURCES

set(sources
    ${SOURCE_DIR}/lib/analyze.c
    ${SOURCE_DIR}/lib/apiwrapper.c
    ${SOURCE_DIR}/lib/bitpack.c
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
    ${SOURCE_DIR}/lib/encoder_disabled.c
)

set(sources_ext)

set(sources_arm
    ${SOURCE_DIR}/lib/arm/armcpu.c
    ${SOURCE_DIR}/lib/arm/armstate.c
    
    ${SOURCE_DIR}/lib/arm/armbits.s
    ${SOURCE_DIR}/lib/arm/armfrag.s
    ${SOURCE_DIR}/lib/arm/armidct.s
    ${SOURCE_DIR}/lib/arm/armloop.s
)

set(sources_x86
    ${SOURCE_DIR}/lib/x86/mmxencfrag.c
    ${SOURCE_DIR}/lib/x86/mmxfdct.c
    ${SOURCE_DIR}/lib/x86/mmxfrag.c
    ${SOURCE_DIR}/lib/x86/mmxidct.c
    ${SOURCE_DIR}/lib/x86/mmxstate.c
    ${SOURCE_DIR}/lib/x86/sse2fdct.c
    ${SOURCE_DIR}/lib/x86/sse2idct.c
    ${SOURCE_DIR}/lib/x86/x86cpu.c
    ${SOURCE_DIR}/lib/x86/x86enquant.c
    ${SOURCE_DIR}/lib/x86/x86state.c
)

set(sources_x86_vc
    ${SOURCE_DIR}/lib/x86_vc/mmxencfrag.c
    ${SOURCE_DIR}/lib/x86_vc/mmxfdct.c
    ${SOURCE_DIR}/lib/x86_vc/mmxfrag.c
    ${SOURCE_DIR}/lib/x86_vc/mmxidct.c
    ${SOURCE_DIR}/lib/x86_vc/mmxstate.c
    ${SOURCE_DIR}/lib/x86_vc/x86cpu.c
    ${SOURCE_DIR}/lib/x86_vc/x86state.c
)

if(MSVC)
    set(sources_ext ${sources_ext} ${sources_x86_vc})
    
    if(MENGINE_ARCHITECTURE_X86)
        add_definitions(-DOC_X86_ASM)
    endif()
    
    if(MENGINE_ARCHITECTURE_X64)
        add_definitions(-DOC_X86_64_ASM)
    endif()
endif()

if(MINGW)
    set(sources_ext ${sources_ext} ${sources_x86})
    
    if(MENGINE_ARCHITECTURE_X86)
        add_definitions(-DOC_X86_ASM)
    endif()
    
    if(MENGINE_ARCHITECTURE_X64)
        add_definitions(-DOC_X86_64_ASM)
    endif()
endif()

if(ANDROID)
    if (${ANDROID_ABI} STREQUAL "x86_64")
        set(sources_ext ${sources_ext} ${sources_x86})
        add_definitions(-DOC_X86_64_ASM)
    elseif (${ANDROID_ABI} STREQUAL "x86")
        set(sources_ext ${sources_ext} ${sources_x86})
        add_definitions(-DOC_X86_ASM)
    elseif (${ANDROID_ABI} STREQUAL "arm64-v8a")
        set(sources_ext ${sources_ext})
    elseif (${ANDROID_ABI} STREQUAL "armeabi-v7a")
        set(sources_ext ${sources_ext})
    endif()
endif()

if(APPLE)
    set(sources_ext ${sources_ext})
endif()

add_library(${PROJECT_NAME} STATIC ${sources} ${sources_ext})
