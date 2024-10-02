cmake_minimum_required(VERSION 2.4.5 FATAL_ERROR)

project(python3_decimal)

SET( SOURCE_DIR ${THIRDPARTY_DIR}/Python3 )

if(WIN32)
  set(PY_POSIX_NAME nt)
else(WIN32)
  set(PY_POSIX_NAME posix)
endif(WIN32)

set( HAVE_CHROOT 0 )
set( HAVE_FSYNC 0 )
set( HAVE_WAIT3 0 )
set( HAVE_WAIT4 0 )
set( HAVE_DEVICE_MACROS 0 )
                 
set(MODULE_SRCS 
                ${SOURCE_DIR}/Modules/_decimal/_decimal.c
                ${SOURCE_DIR}/Modules/_decimal/libmpdec/basearith.c
                ${SOURCE_DIR}/Modules/_decimal/libmpdec/constants.c
                ${SOURCE_DIR}/Modules/_decimal/libmpdec/context.c
                ${SOURCE_DIR}/Modules/_decimal/libmpdec/convolute.c
                ${SOURCE_DIR}/Modules/_decimal/libmpdec/crt.c
                ${SOURCE_DIR}/Modules/_decimal/libmpdec/difradix2.c
                ${SOURCE_DIR}/Modules/_decimal/libmpdec/fnt.c
                ${SOURCE_DIR}/Modules/_decimal/libmpdec/fourstep.c
                ${SOURCE_DIR}/Modules/_decimal/libmpdec/io.c
                ${SOURCE_DIR}/Modules/_decimal/libmpdec/mpdecimal.c
                ${SOURCE_DIR}/Modules/_decimal/libmpdec/numbertheory.c
                ${SOURCE_DIR}/Modules/_decimal/libmpdec/sixstep.c
                ${SOURCE_DIR}/Modules/_decimal/libmpdec/transpose.c
                )
				
include_directories(${SOURCE_DIR})
include_directories(${SOURCE_DIR}/Include)
include_directories(${SOURCE_DIR}/Modules/_decimal/libmpdec)

if(MENGINE_ARCHITECTURE_X86)
    add_definitions(-DCONFIG_32)
endif()

if(MENGINE_ARCHITECTURE_X64)
    add_definitions(-DCONFIG_64)
endif()

add_definitions(-DANSI)

if(WIN32)
  include_directories(${SOURCE_DIR}/PC ${SOURCE_DIR}/Python)
  if(MSVC)
	  add_definitions(-DWIN32 -D_WINDOWS)
  else(MSVC)
	  add_definitions(-D_WINDOWS)
  endif(MSVC)
endif(WIN32)

add_definitions(-DPYEXPAT_EXPORTS)
add_definitions(-DHAVE_EXPAT_H)
add_definitions(-DXML_NS)
add_definitions(-DXML_DTD)
add_definitions(-DBYTEORDER=1234)
add_definitions(-DXML_CONTEXT_BYTES=1024)
add_definitions(-DXML_STATIC)
add_definitions(-DHAVE_MEMMOVE)

add_definitions(-DMS_NO_COREDLL)
add_definitions(-DPy_NO_ENABLE_SHARED)
add_definitions(-DPy_BUILD_CORE)
	  
add_library( ${PROJECT_NAME} STATIC			  
              ${MODULE_SRCS}
              )
            
target_link_libraries(${PROJECT_NAME} ${extraLibs})