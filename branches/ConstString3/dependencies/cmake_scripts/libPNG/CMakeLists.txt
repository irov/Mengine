
CMAKE_MINIMUM_REQUIRED( VERSION 2.6 )
                                             
project( png )

SET( CMAKE_CURRENT_SOURCE_DIR ${CMAKE_SOURCE_DIR}/libPNG )

INCLUDE( ../menge_macro.cmake )

MENGE_SET_PROJECT_DEFAULTS( png )

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

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/scripts/pnglibconf.h.prebuilt ${CMAKE_CURRENT_SOURCE_DIR}/pnglibconf.h COPYONLY)

 # OUR SOURCES
set(png_sources
 ${CMAKE_CURRENT_SOURCE_DIR}/png.h
 ${CMAKE_CURRENT_SOURCE_DIR}/pngconf.h
 ${CMAKE_CURRENT_SOURCE_DIR}/pngpriv.h
 ${CMAKE_CURRENT_SOURCE_DIR}/png.c
 ${CMAKE_CURRENT_SOURCE_DIR}/pngerror.c
 ${CMAKE_CURRENT_SOURCE_DIR}/pngget.c
 ${CMAKE_CURRENT_SOURCE_DIR}/pngmem.c
 ${CMAKE_CURRENT_SOURCE_DIR}/pngpread.c
 ${CMAKE_CURRENT_SOURCE_DIR}/pngread.c
 ${CMAKE_CURRENT_SOURCE_DIR}/pngrio.c
 ${CMAKE_CURRENT_SOURCE_DIR}/pngrtran.c
 ${CMAKE_CURRENT_SOURCE_DIR}/pngrutil.c
 ${CMAKE_CURRENT_SOURCE_DIR}/pngset.c
 ${CMAKE_CURRENT_SOURCE_DIR}/pngtrans.c
 ${CMAKE_CURRENT_SOURCE_DIR}/pngwio.c
 ${CMAKE_CURRENT_SOURCE_DIR}/pngwrite.c
 ${CMAKE_CURRENT_SOURCE_DIR}/pngwtran.c
 ${CMAKE_CURRENT_SOURCE_DIR}/pngwutil.c
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
include_directories(${CMAKE_CURRENT_SOURCE_DIR} ${ZLIB_INCLUDE_DIR})

add_library( ${png_LIB_NAME} STATIC ${png_sources} )

