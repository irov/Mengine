CMAKE_MINIMUM_REQUIRED( VERSION 2.6 )

PROJECT( MengeDependencies )

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

#ADD_SUBDIRECTORY( astralax )
#ADD_SUBDIRECTORY( dxerr )
#ADD_SUBDIRECTORY( cal3d )

#ADD_SUBDIRECTORY( libvpx )
#ADD_SUBDIRECTORY( libwebm )

#ADD_SUBDIRECTORY( iniparser )
#ADD_SUBDIRECTORY( expat )
#ADD_SUBDIRECTORY( ois )
if( MENGINE_PLATFORM_WIN32 )	
    ADD_SUBDIRECTORY( Python3 )
    ADD_SUBDIRECTORY( Python3_decimal )
    ADD_SUBDIRECTORY( pybind3 )
    
    ADD_SUBDIRECTORY( xml2metabuf )
endif()

ADD_SUBDIRECTORY( spine )
ADD_SUBDIRECTORY( libmovie )
ADD_SUBDIRECTORY( GOAP )

if( MENGINE_PLUGIN_TTF )
    set(CMAKE_SOURCE_DIR2 ${CMAKE_SOURCE_DIR})
    set(CMAKE_SOURCE_DIR ${THIRDPARTY_DIR}/freetype)    
    set(SKIP_INSTALL_ALL ON CACHE BOOL "SKIP_INSTALL_ALL" FORCE)

    ADD_SUBDIRECTORY( ${CMAKE_SOURCE_DIR} ${CMAKE_TEMP_DIR}/freetype )
    set(CMAKE_SOURCE_DIR ${CMAKE_SOURCE_DIR2})    
endif()

if( MENGINE_PLUGIN_CURL )
    set(CMAKE_SOURCE_DIR2 ${CMAKE_SOURCE_DIR})
    set(CMAKE_SOURCE_DIR ${THIRDPARTY_DIR}/libcurl) 
	
    set(BUILD_CURL_EXE OFF CACHE BOOL "Set to ON to build curl executable." FORCE)
    set(BUILD_TESTING OFF CACHE BOOL "Set to ON to build curl executable." FORCE)
    set(ENABLE_MANUAL OFF CACHE BOOL "to provide the built-in manual" FORCE)
    set(CURL_STATICLIB ON CACHE BOOL "Set to ON to build libcurl with static linking." FORCE)
    set(HTTP_ONLY ON CACHE BOOL "disables all protocols except HTTP (This overrides all CURL_DISABLE_* options)" FORCE)
    set(CMAKE_USE_OPENSSL OFF CACHE BOOL "Use OpenSSL code. Experimental" FORCE)	
	
    ADD_SUBDIRECTORY( ${CMAKE_SOURCE_DIR} ${CMAKE_TEMP_DIR}/libcurl )
	
    configure_file(${CURL_BINARY_DIR}/include/curl/curlbuild.h ${CURL_SOURCE_DIR}/include/curl/curlbuild.h COPYONLY)
		
    set(CMAKE_SOURCE_DIR ${CMAKE_SOURCE_DIR2})
endif()


if( MENGINE_TARGET_SDL )
    set(CMAKE_SOURCE_DIR2 ${CMAKE_SOURCE_DIR})
    set(CMAKE_SOURCE_DIR ${THIRDPARTY_DIR}/SDL2)    
    set(FORCE_STATIC_VCRT ON CACHE BOOL "Force /MT for static VC runtimes" FORCE)
    set(SDL_SHARED OFF CACHE BOOL "Build a static version of the library" FORCE)
    set(SDL_STATIC ON CACHE BOOL "Build a static version of the library" FORCE)

    ADD_SUBDIRECTORY( ${CMAKE_SOURCE_DIR} ${CMAKE_TEMP_DIR}/SDL2 )
    set(CMAKE_SOURCE_DIR ${CMAKE_SOURCE_DIR2})
    
    ADD_SUBDIRECTORY( sdl2_net )
    ADD_SUBDIRECTORY( sdl2_mixer )
endif()

set(CMAKE_SOURCE_DIR2 ${CMAKE_SOURCE_DIR})
set(CMAKE_SOURCE_DIR ${THIRDPARTY_DIR}/Box2D)
SET(BOX2D_INSTALL OFF CACHE BOOL "Install Box2D libs, includes, and CMake scripts" FORCE)
SET(BOX2D_INSTALL_DOC OFF CACHE BOOL "Install Box2D documentation" FORCE)
SET(BOX2D_BUILD_SHARED OFF CACHE BOOL "Build Box2D shared libraries" FORCE)
SET(BOX2D_BUILD_STATIC ON CACHE BOOL "Build Box2D static libraries" FORCE)
SET(BOX2D_BUILD_EXAMPLES OFF CACHE BOOL "Build Box2D examples" FORCE)
ADD_SUBDIRECTORY( ${CMAKE_SOURCE_DIR} ${CMAKE_TEMP_DIR}/Box2D )
set(CMAKE_SOURCE_DIR ${CMAKE_SOURCE_DIR2})

set(CMAKE_SOURCE_DIR2 ${CMAKE_SOURCE_DIR})
set(CMAKE_SOURCE_DIR ${THIRDPARTY_DIR}/libwebp)
ADD_SUBDIRECTORY( ${CMAKE_SOURCE_DIR} ${CMAKE_TEMP_DIR}/libwebp )
set(CMAKE_SOURCE_DIR ${CMAKE_SOURCE_DIR2})

if( MENGINE_TARGET_MSVC OR MENGINE_TARGET_MINGW OR MENGINE_TARGET_SDL)
    set(CMAKE_SOURCE_DIR2 ${CMAKE_SOURCE_DIR})
    set(CMAKE_SOURCE_DIR ${THIRDPARTY_DIR}/openal-soft)
    SET(LIBTYPE STATIC)
    SET(FORCE_STATIC_VCRT ON CACHE BOOL "Force /MT for static VC runtimes" FORCE)
    SET(ALSOFT_NO_UID_DEFS OFF CACHE BOOL "Do not define GUIDs, IIDs, CLSIDs, or PropertyKeys" FORCE)
    SET(ALSOFT_DLOPEN ON CACHE BOOL "Check for the dlopen API for loading optional libs" FORCE)
    SET(ALSOFT_WERROR OFF CACHE BOOL "Treat compile warnings as errors" FORCE)
    SET(ALSOFT_UTILS OFF CACHE BOOL "Build and install utility programs" FORCE)
    SET(ALSOFT_NO_CONFIG_UTIL ON CACHE BOOL "Disable building the alsoft-config utility" FORCE)
    SET(ALSOFT_EXAMPLES OFF CACHE BOOL "Build and install example programs" FORCE)
    SET(ALSOFT_TESTS OFF CACHE BOOL "Build and install test programs" FORCE)
    SET(ALSOFT_CONFIG OFF CACHE BOOL "Install alsoft.conf sample configuration file" FORCE)
    SET(ALSOFT_HRTF_DEFS OFF CACHE BOOL "Install HRTF definition files" FORCE)
    SET(ALSOFT_INSTALL OFF CACHE BOOL "Install headers and libraries" FORCE)    
    
    ADD_SUBDIRECTORY( ${CMAKE_SOURCE_DIR} ${CMAKE_TEMP_DIR}/openal-soft )
    set(CMAKE_SOURCE_DIR ${CMAKE_SOURCE_DIR2})
endif()

if( MENGINE_TARGET_AEPLUGIN )
    ADD_SUBDIRECTORY( ae )
endif()    


ADD_SUBDIRECTORY( lz4 )
ADD_SUBDIRECTORY( libogg )
ADD_SUBDIRECTORY( libvorbis )
ADD_SUBDIRECTORY( libvorbisfile )
ADD_SUBDIRECTORY( libtheora )

#ADD_SUBDIRECTORY( AtlasAllocator )

#ADD_SUBDIRECTORY( simpleini )

ADD_SUBDIRECTORY( stdex )
ADD_SUBDIRECTORY( zlib )
ADD_SUBDIRECTORY( poly2tri )
ADD_SUBDIRECTORY( polyclipping )
#ADD_SUBDIRECTORY( icu )

#ADD_SUBDIRECTORY( Box2D )

ADD_SUBDIRECTORY( libjpeg )
ADD_SUBDIRECTORY( libpng )
#ADD_SUBDIRECTORY( libwebp )

ADD_SUBDIRECTORY( pybind )
ADD_SUBDIRECTORY( Python )

ADD_SUBDIRECTORY( metabuf )
ADD_SUBDIRECTORY( pugixml )

