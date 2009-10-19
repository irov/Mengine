
CMAKE_MINIMUM_REQUIRED( VERSION 2.6 )
                                             
project( jpeg )

SET( CMAKE_CURRENT_SOURCE_DIR ${CMAKE_SOURCE_DIR}/../libJPEG )

INCLUDE( ../menge_macro.cmake )

MENGE_SET_PROJECT_DEFAULTS( jpeg )

configure_file( ${CMAKE_SOURCE_DIR}/libJPEG/jconfig.h ${CMAKE_CURRENT_SOURCE_DIR}/jconfig.h COPYONLY)
configure_file( ${CMAKE_SOURCE_DIR}/libJPEG/jmorecfg.h ${CMAKE_CURRENT_SOURCE_DIR}/jmorecfg.h COPYONLY)

# OUR SOURCES
set( jpeg_sources
${CMAKE_CURRENT_SOURCE_DIR}/jcapimin.c 
${CMAKE_CURRENT_SOURCE_DIR}/jcapistd.c
${CMAKE_CURRENT_SOURCE_DIR}/jccoefct.c 
${CMAKE_CURRENT_SOURCE_DIR}/jccolor.c 
${CMAKE_CURRENT_SOURCE_DIR}/jcdctmgr.c 
${CMAKE_CURRENT_SOURCE_DIR}/jchuff.c
${CMAKE_CURRENT_SOURCE_DIR}/jcinit.c 
${CMAKE_CURRENT_SOURCE_DIR}/jcmainct.c 
${CMAKE_CURRENT_SOURCE_DIR}/jcmarker.c 
${CMAKE_CURRENT_SOURCE_DIR}/jcmaster.c 
${CMAKE_CURRENT_SOURCE_DIR}/jcomapi.c 
${CMAKE_CURRENT_SOURCE_DIR}/jcparam.c
${CMAKE_CURRENT_SOURCE_DIR}/jcphuff.c 
${CMAKE_CURRENT_SOURCE_DIR}/jcprepct.c 
${CMAKE_CURRENT_SOURCE_DIR}/jcsample.c 
${CMAKE_CURRENT_SOURCE_DIR}/jctrans.c 
${CMAKE_CURRENT_SOURCE_DIR}/jdapimin.c 
${CMAKE_CURRENT_SOURCE_DIR}/jdapistd.c
${CMAKE_CURRENT_SOURCE_DIR}/jdatadst.c 
${CMAKE_CURRENT_SOURCE_DIR}/jdatasrc.c 
${CMAKE_CURRENT_SOURCE_DIR}/jdcoefct.c 
${CMAKE_CURRENT_SOURCE_DIR}/jdcolor.c 
${CMAKE_CURRENT_SOURCE_DIR}/jddctmgr.c 
${CMAKE_CURRENT_SOURCE_DIR}/jdhuff.c
${CMAKE_CURRENT_SOURCE_DIR}/jdinput.c 
${CMAKE_CURRENT_SOURCE_DIR}/jdmainct.c 
${CMAKE_CURRENT_SOURCE_DIR}/jdmarker.c 
${CMAKE_CURRENT_SOURCE_DIR}/jdmaster.c 
${CMAKE_CURRENT_SOURCE_DIR}/jdmerge.c 
${CMAKE_CURRENT_SOURCE_DIR}/jdphuff.c
${CMAKE_CURRENT_SOURCE_DIR}/jdpostct.c 
${CMAKE_CURRENT_SOURCE_DIR}/jdsample.c 
${CMAKE_CURRENT_SOURCE_DIR}/jdtrans.c 
${CMAKE_CURRENT_SOURCE_DIR}/jerror.c 
${CMAKE_CURRENT_SOURCE_DIR}/jfdctflt.c 
${CMAKE_CURRENT_SOURCE_DIR}/jfdctfst.c
${CMAKE_CURRENT_SOURCE_DIR}/jfdctint.c 
${CMAKE_CURRENT_SOURCE_DIR}/jidctflt.c 
${CMAKE_CURRENT_SOURCE_DIR}/jidctfst.c 
${CMAKE_CURRENT_SOURCE_DIR}/jidctint.c 
${CMAKE_CURRENT_SOURCE_DIR}/jidctred.c 
${CMAKE_CURRENT_SOURCE_DIR}/jquant1.c
${CMAKE_CURRENT_SOURCE_DIR}/jquant2.c 
${CMAKE_CURRENT_SOURCE_DIR}/jutils.c 
${CMAKE_CURRENT_SOURCE_DIR}/jmemmgr.c
)

#choose memory manager
if(WIN32)
	set(jpeg_sources ${jpeg_sources} ${CMAKE_CURRENT_SOURCE_DIR}/jmemnobs.c)
endif(WIN32)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})
add_library( ${jpeg_LIB_NAME} STATIC ${jpeg_sources} )
