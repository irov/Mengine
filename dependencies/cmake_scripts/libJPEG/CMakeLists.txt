
CMAKE_MINIMUM_REQUIRED( VERSION 2.6 )
                                             
project(JPEG)

SET(TRUE_DIR ../../libJPEG)
SET(JPGE_SOURCE_DIR ${TRUE_DIR})

INCLUDE( ../menge_macro.cmake )

MENGE_SET_PROJECT_DEFAULTS()

# SET LIBNAME
# msvc does not append 'lib' - do it here to have consistent name
if(MSVC)
 set(JPEG_LIB_NAME lib)
endif(MSVC)
set( JPEG_LIB_NAME ${JPEG_LIB_NAME}jpeg_static )

# OUR SOURCES
set(libjpeg_sources
${TRUE_DIR}/jcapimin.c 
${TRUE_DIR}/jcapistd.c
${TRUE_DIR}/jccoefct.c 
${TRUE_DIR}/jccolor.c 
${TRUE_DIR}/jcdctmgr.c 
${TRUE_DIR}/jchuff.c
${TRUE_DIR}/jcinit.c 
${TRUE_DIR}/jcmainct.c 
${TRUE_DIR}/jcmarker.c 
${TRUE_DIR}/jcmaster.c 
${TRUE_DIR}/jcomapi.c 
${TRUE_DIR}/jcparam.c
${TRUE_DIR}/jcphuff.c 
${TRUE_DIR}/jcprepct.c 
${TRUE_DIR}/jcsample.c 
${TRUE_DIR}/jctrans.c 
${TRUE_DIR}/jdapimin.c 
${TRUE_DIR}/jdapistd.c
${TRUE_DIR}/jdatadst.c 
${TRUE_DIR}/jdatasrc.c 
${TRUE_DIR}/jdcoefct.c 
${TRUE_DIR}/jdcolor.c 
${TRUE_DIR}/jddctmgr.c 
${TRUE_DIR}/jdhuff.c
${TRUE_DIR}/jdinput.c 
${TRUE_DIR}/jdmainct.c 
${TRUE_DIR}/jdmarker.c 
${TRUE_DIR}/jdmaster.c 
${TRUE_DIR}/jdmerge.c 
${TRUE_DIR}/jdphuff.c
${TRUE_DIR}/jdpostct.c 
${TRUE_DIR}/jdsample.c 
${TRUE_DIR}/jdtrans.c 
${TRUE_DIR}/jerror.c 
${TRUE_DIR}/jfdctflt.c 
${TRUE_DIR}/jfdctfst.c
${TRUE_DIR}/jfdctint.c 
${TRUE_DIR}/jidctflt.c 
${TRUE_DIR}/jidctfst.c 
${TRUE_DIR}/jidctint.c 
${TRUE_DIR}/jidctred.c 
${TRUE_DIR}/jquant1.c
${TRUE_DIR}/jquant2.c 
${TRUE_DIR}/jutils.c 
${TRUE_DIR}/jmemmgr.c
)

#choose memory manager
if(WIN32)
	set(libjpeg_sources ${libjpeg_sources} ${TRUE_DIR}/jmemnobs.c)
endif(WIN32)

include_directories(${JPEG_SOURCE_DIR})
add_library( ${JPEG_LIB_NAME} STATIC ${libjpeg_sources} )
