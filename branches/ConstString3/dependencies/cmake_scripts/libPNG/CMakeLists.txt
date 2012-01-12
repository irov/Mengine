
CMAKE_MINIMUM_REQUIRED( VERSION 2.6 )
                                             
project( png )

SET( SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../../libPNG )

# needed packages
#find_package(ZLIB REQUIRED)
SET( ZLIB_INCLUDE_DIR ${CMAKE_SOURCE_DIR}/zlib )
if(NOT WIN32)
 find_library(M_LIBRARY
     NAMES m
     PATHS /usr/lib /usr/local/lib
 )
 if(NOT M_LIBRARY)
   message(STATUS
     "math library 'libm' not found - floating point support disabled")
 endif(NOT M_LIBRARY)
else(NOT WIN32)
 # not needed on windows
 set(M_LIBRARY "")
endif(NOT WIN32)

configure_file(${SOURCE_DIR}/scripts/pnglibconf.h.prebuilt ${SOURCE_DIR}/pnglibconf.h COPYONLY)

 # OUR SOURCES
set(png_sources
 ${SOURCE_DIR}/png.h
 ${SOURCE_DIR}/pngconf.h
 ${SOURCE_DIR}/pngpriv.h
 ${SOURCE_DIR}/png.c
 ${SOURCE_DIR}/pngerror.c
 ${SOURCE_DIR}/pngget.c
 ${SOURCE_DIR}/pngmem.c
 ${SOURCE_DIR}/pngpread.c
 ${SOURCE_DIR}/pngread.c
 ${SOURCE_DIR}/pngrio.c
 ${SOURCE_DIR}/pngrtran.c
 ${SOURCE_DIR}/pngrutil.c
 ${SOURCE_DIR}/pngset.c
 ${SOURCE_DIR}/pngtrans.c
 ${SOURCE_DIR}/pngwio.c
 ${SOURCE_DIR}/pngwrite.c
 ${SOURCE_DIR}/pngwtran.c
 ${SOURCE_DIR}/pngwutil.c
)

# SOME NEEDED DEFINITIONS
if(MSVC)
  add_definitions(-DPNG_NO_MODULEDEF)
endif(MSVC)

#add_definitions(-DZLIB_DLL)

add_definitions(-DLIBPNG_NO_MMX)
add_definitions(-DPNG_NO_MMX_CODE)

if(NOT M_LIBRARY AND NOT WIN32)
 add_definitions(-DPNG_NO_FLOATING_POINT_SUPPORTED)
endif(NOT M_LIBRARY AND NOT WIN32)

# NOW BUILD OUR TARGET
include_directories(${SOURCE_DIR} ${ZLIB_INCLUDE_DIR})

add_library( ${PROJECT_NAME}_static STATIC ${png_sources} )

