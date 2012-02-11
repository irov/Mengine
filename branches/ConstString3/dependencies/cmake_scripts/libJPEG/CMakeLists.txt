
CMAKE_MINIMUM_REQUIRED( VERSION 2.6 )
                                             
project( jpeg )

SET( SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../../libjpeg )

#configure_file( ${SOURCE_DIR}/jconfig.vc ${SOURCE_DIR}/jconfig.h COPYONLY)
configure_file( ${CMAKE_CURRENT_SOURCE_DIR}/jmorecfg.h ${SOURCE_DIR}/jmorecfg.h COPYONLY)

configure_file(${SOURCE_DIR}/jconfig.vc ${SOURCE_DIR}/jconfig.h COPYONLY)

# OUR SOURCES
set( jpeg_sources
${SOURCE_DIR}/jaricom.c
${SOURCE_DIR}/jcapimin.c
${SOURCE_DIR}/jcapistd.c
${SOURCE_DIR}/jcarith.c
${SOURCE_DIR}/jccoefct.c
${SOURCE_DIR}/jccolor.c
${SOURCE_DIR}/jcdctmgr.c
${SOURCE_DIR}/jchuff.c
${SOURCE_DIR}/jcinit.c
${SOURCE_DIR}/jcmainct.c
${SOURCE_DIR}/jcmarker.c
${SOURCE_DIR}/jcmaster.c
${SOURCE_DIR}/jcomapi.c
${SOURCE_DIR}/jcparam.c
${SOURCE_DIR}/jcprepct.c
${SOURCE_DIR}/jcsample.c
${SOURCE_DIR}/jctrans.c
${SOURCE_DIR}/jdapimin.c
${SOURCE_DIR}/jdapistd.c
${SOURCE_DIR}/jdarith.c
${SOURCE_DIR}/jdatadst.c
${SOURCE_DIR}/jdatasrc.c
${SOURCE_DIR}/jdcoefct.c
${SOURCE_DIR}/jdcolor.c
${SOURCE_DIR}/jddctmgr.c
${SOURCE_DIR}/jdhuff.c
${SOURCE_DIR}/jdinput.c
${SOURCE_DIR}/jdmainct.c
${SOURCE_DIR}/jdmarker.c
${SOURCE_DIR}/jdmaster.c
${SOURCE_DIR}/jdmerge.c
${SOURCE_DIR}/jdpostct.c
${SOURCE_DIR}/jdsample.c
${SOURCE_DIR}/jdtrans.c
${SOURCE_DIR}/jerror.c
${SOURCE_DIR}/jfdctflt.c
${SOURCE_DIR}/jfdctfst.c
${SOURCE_DIR}/jfdctint.c
${SOURCE_DIR}/jidctflt.c
${SOURCE_DIR}/jidctfst.c
${SOURCE_DIR}/jidctint.c
${SOURCE_DIR}/jquant1.c
${SOURCE_DIR}/jquant2.c
${SOURCE_DIR}/jutils.c
${SOURCE_DIR}/jmemmgr.c
)

#choose memory manager
if(WIN32)
	set(jpeg_sources ${jpeg_sources} ${SOURCE_DIR}/jmemnobs.c)
else(WIN32)
	set(jpeg_sources ${jpeg_sources} ${SOURCE_DIR}/jmemnobs.c)
endif(WIN32)

include_directories(${SOURCE_DIR})
add_library( ${PROJECT_NAME}_static STATIC ${jpeg_sources} )
