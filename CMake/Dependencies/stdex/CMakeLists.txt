CMAKE_MINIMUM_REQUIRED( VERSION 2.6 )
                                             
project( stdex )

SET( SOURCE_DIR ${THIRDPARTY_DIR}/stdex )

# OUR SOURCES
set( sources
${SOURCE_DIR}/src/memorycopy.cpp
${SOURCE_DIR}/src/allocator.cpp
${SOURCE_DIR}/src/exception.cpp
${SOURCE_DIR}/src/memory_cache.cpp
${SOURCE_DIR}/src/const_string2.cpp
${SOURCE_DIR}/src/const_string2_holder.cpp
${SOURCE_DIR}/src/const_string2_holder_none.cpp
${SOURCE_DIR}/src/xml_sax_parser.cpp
${SOURCE_DIR}/src/ini.cpp
${SOURCE_DIR}/src/thread_guard.cpp
${SOURCE_DIR}/src/stack.cpp
${SOURCE_DIR}/src/sha1.cpp
)

if(NOT MSVC)
    if(NOT EMSCRIPTEN)
        ADD_DEFINITIONS(-D"override = /* */")
        ADD_DEFINITIONS(-D"nullptr=0")
        ADD_DEFINITIONS(-D"nullptr_t=void*")     
    endif()
endif() 

#add_definitions(-Dstdex_allocator_disable)

include_directories( ${SOURCE_DIR}/include )

add_library( ${PROJECT_NAME} STATIC ${sources} )
