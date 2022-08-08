project(rpmalloc C)

SET(SOURCE_DIR ${THIRDPARTY_DIR}/rpmalloc)

# OUR SOURCES
set( sources
    ${SOURCE_DIR}/rpmalloc/rpmalloc.c
)

include_directories( ${SOURCE_DIR}/rpmalloc )

ADD_DEFINITIONS(-DENABLE_PRELOAD=1)

if(MENGINE_BUILD_TYPE_DEBUG)
    ADD_DEFINITIONS(-DENABLE_ASSERTS=1)
    ADD_DEFINITIONS(-DENABLE_VALIDATE_ARGS=1)
endif()

add_library(${PROJECT_NAME} STATIC ${sources})
