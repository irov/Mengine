
CMAKE_MINIMUM_REQUIRED( VERSION 2.6 )
                                             
project( jpeg )

SET( CMAKE_CURRENT_SOURCE_DIR ${CMAKE_SOURCE_DIR}/../libJPEG )

INCLUDE( ../menge_macro.cmake )

MENGE_SET_PROJECT_DEFAULTS( jpeg )

configure_file( ${CMAKE_CURRENT_SOURCE_DIR}/jconfig.vc ${CMAKE_CURRENT_SOURCE_DIR}/jconfig.h COPYONLY)
configure_file( ${CMAKE_SOURCE_DIR}/libJPEG/jmorecfg.h ${CMAKE_CURRENT_SOURCE_DIR}/jmorecfg.h COPYONLY)

# OUR SOURCES
set( jpeg_sources
jaricom.c
jcapimin.c
jcapistd.c
jcarith.c
jccoefct.c
jccolor.c
jcdctmgr.c
jchuff.c
jcinit.c
jcmainct.c
jcmarker.c
jcmaster.c
jcomapi.c
jcparam.c
jcprepct.c
jcsample.c
jctrans.c
jdapimin.c
jdapistd.c
jdarith.c
jdatadst.c
jdatasrc.c
jdcoefct.c
jdcolor.c
jddctmgr.c
jdhuff.c
jdinput.c
jdmainct.c
jdmarker.c
jdmaster.c
jdmerge.c
jdpostct.c
jdsample.c
jdtrans.c
jerror.c
jfdctflt.c
jfdctfst.c
jfdctint.c
jidctflt.c
jidctfst.c
jidctint.c
jquant1.c
jquant2.c
jutils.c
jmemmgr.c
)

set( jpeg_sources_ )
foreach( src_file ${jpeg_sources} )
    set( jpeg_sources_ ${jpeg_sources_} ${CMAKE_CURRENT_SOURCE_DIR}/${src_file} )
endforeach( src_file ${jpeg_sources} )


#choose memory manager
if(WIN32)
	set(jpeg_sources_ ${jpeg_sources_} ${CMAKE_CURRENT_SOURCE_DIR}/jmemnobs.c)
else(WIN32)
	set(jpeg_sources_ ${jpeg_sources_} ${CMAKE_CURRENT_SOURCE_DIR}/jmemnobs.c)
endif(WIN32)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})
add_library( ${jpeg_LIB_NAME} STATIC ${jpeg_sources_} )
