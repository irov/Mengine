CMAKE_MINIMUM_REQUIRED(VERSION 3.10)

project(theora)

SET(SOURCE_DIR ${THIRDPARTY_DIR}/libtheora)

INCLUDE_DIRECTORIES(${SOURCE_DIR}/include)
INCLUDE_DIRECTORIES(${SOURCE_DIR}/lib)

INCLUDE_DIRECTORIES(${OGG_INCLUDE_DIRS})

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
    ${SOURCE_DIR}/lib/encoder_disabled.c
    )

add_library(${PROJECT_NAME} STATIC ${sources} ${sources_ext})
