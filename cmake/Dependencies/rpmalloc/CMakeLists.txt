project(rpmalloc)

SET(SOURCE_DIR ${THIRDPARTY_DIR}/rpmalloc)

# OUR SOURCES
set( sources
    ${SOURCE_DIR}/rpmalloc/rpmalloc.c
)

include_directories( ${SOURCE_DIR}/rpmalloc )

add_library(${PROJECT_NAME} STATIC ${sources})
