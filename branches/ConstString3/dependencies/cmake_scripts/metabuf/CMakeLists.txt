CMAKE_MINIMUM_REQUIRED( VERSION 2.6 )
                                             
project(metabuf)

SET( SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../../metabuf )

if(NOT MSVC)
	ADD_DEFINITIONS(-D"override = /* */")
endif(NOT MSVC)

set( metabuf_sources
${SOURCE_DIR}/metabuf/Metabuf.cpp
)

add_library( metabuf_static STATIC ${metabuf_sources} )

if(MSVC)
    set( xml2metabuf_sources
    ${SOURCE_DIR}/xml2metabuf/Xml2Metabuf.cpp
    ${SOURCE_DIR}/xml2metabuf/Xml2Metacode.cpp
    ${SOURCE_DIR}/xml2metabuf/XmlProtocol.cpp
    )

    include_directories(${SOURCE_DIR}/include ${CMAKE_CURRENT_SOURCE_DIR}/../../pugixml/src)

    add_library( xml2metabuf_static STATIC ${xml2metabuf_sources} )
endif(MSVC)
