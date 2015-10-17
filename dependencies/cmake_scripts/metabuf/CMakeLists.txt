CMAKE_MINIMUM_REQUIRED( VERSION 2.6 )
                                             
project(metabuf)

SET( SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../../metabuf )

configure_file(Metaconfig.hpp ${SOURCE_DIR}/metabuf/Metaconfig.hpp COPYONLY)

if(NOT MSVC)
    if(NOT EMSCRIPTEN)
        ADD_DEFINITIONS(-D"override = /* */")
        ADD_DEFINITIONS(-D"nullptr=0")
        ADD_DEFINITIONS(-D"nullptr_t=void*")    
    endif()
endif()

set( metabuf_sources
    ${SOURCE_DIR}/metabuf/Metabuf.cpp
    )
    
include_directories(${SOURCE_DIR})
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../../stdex/include)         

add_library( metabuf_static STATIC ${metabuf_sources} )

if( DEFINED MENGINE_TARGET_MSVC )
    set( xml2metabuf_sources
    ${SOURCE_DIR}/xml2metabuf/Xml2Metabuf.cpp
    ${SOURCE_DIR}/xml2metabuf/Xml2Metacode.cpp
    ${SOURCE_DIR}/xml2metabuf/XmlProtocol.cpp
    )

    include_directories(${SOURCE_DIR}/include ${CMAKE_CURRENT_SOURCE_DIR}/../../pugixml/src)

    add_library( xml2metabuf_static STATIC ${xml2metabuf_sources} )
endif()
