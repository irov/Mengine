
CMAKE_MINIMUM_REQUIRED( VERSION 2.6 )
                                             
project(OpenAL)

SET( CMAKE_CURRENT_SOURCE_DIR ${CMAKE_SOURCE_DIR}/OpenAL )

INCLUDE( ../menge_macro.cmake )

MENGE_SET_PROJECT_DEFAULTS( OpenAL )

if(MSVC)
	include_directories( ${CMAKE_SOURCE_DIR}/DirectX/DX8/include )
endif(MSVC)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)
INCLUDE_DIRECTORIES(OpenAL32/Include include "${CMAKE_BINARY_DIR}")
 
#patch
#if( WIN32 )
#	configure_file( ${CMAKE_SOURCE_DIR}/OpenAL/win_patch/al.h ${CMAKE_CURRENT_SOURCE_DIR}/Include/AL/al.h COPYONLY)
#	configure_file( ${CMAKE_SOURCE_DIR}/OpenAL/win_patch/alc.h ${CMAKE_CURRENT_SOURCE_DIR}/Include/AL/alc.h COPYONLY)
#	configure_file( ${CMAKE_SOURCE_DIR}/OpenAL/win_patch/ALc.c ${CMAKE_CURRENT_SOURCE_DIR}/OpenAL-Windows/Alc/ALc.c COPYONLY)
#endif( WIN32 )

# OUR SOURCES
#set(libopenal_sources)
CONFIGURE_FILE(
    "${CMAKE_CURRENT_SOURCE_DIR}/config.h.in"
    "${CMAKE_BINARY_DIR}/config.h")

SET(OPENAL_OBJS  ${CMAKE_CURRENT_SOURCE_DIR}/OpenAL32/alAuxEffectSlot.c
                 ${CMAKE_CURRENT_SOURCE_DIR}/OpenAL32/alBuffer.c
                 ${CMAKE_CURRENT_SOURCE_DIR}/OpenAL32/alDatabuffer.c
                 ${CMAKE_CURRENT_SOURCE_DIR}/OpenAL32/alEffect.c
                 ${CMAKE_CURRENT_SOURCE_DIR}/OpenAL32/alError.c
                 ${CMAKE_CURRENT_SOURCE_DIR}/OpenAL32/alExtension.c
                 ${CMAKE_CURRENT_SOURCE_DIR}/OpenAL32/alFilter.c
                 ${CMAKE_CURRENT_SOURCE_DIR}/OpenAL32/alListener.c
                 ${CMAKE_CURRENT_SOURCE_DIR}/OpenAL32/alSource.c
                 ${CMAKE_CURRENT_SOURCE_DIR}/OpenAL32/alState.c
                 ${CMAKE_CURRENT_SOURCE_DIR}/OpenAL32/alThunk.c
)

SET(ALC_OBJS  ${CMAKE_CURRENT_SOURCE_DIR}/Alc/ALc.c
              ${CMAKE_CURRENT_SOURCE_DIR}/Alc/ALu.c
              ${CMAKE_CURRENT_SOURCE_DIR}/Alc/alcConfig.c
              ${CMAKE_CURRENT_SOURCE_DIR}/Alc/alcEcho.c
              ${CMAKE_CURRENT_SOURCE_DIR}/Alc/alcModulator.c
              ${CMAKE_CURRENT_SOURCE_DIR}/Alc/alcReverb.c
              ${CMAKE_CURRENT_SOURCE_DIR}/Alc/alcRing.c
              ${CMAKE_CURRENT_SOURCE_DIR}/Alc/alcThread.c
              ${CMAKE_CURRENT_SOURCE_DIR}/Alc/bs2b.c
              ${CMAKE_CURRENT_SOURCE_DIR}/Alc/mixer.c
              ${CMAKE_CURRENT_SOURCE_DIR}/Alc/panning.c
              ${CMAKE_CURRENT_SOURCE_DIR}/Alc/null.c
)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/OpenAL32/Include)

ADD_DEFINITIONS(-DAL_LIBTYPE_STATIC)
SET(PKG_CONFIG_CFLAGS -DAL_LIBTYPE_STATIC ${PKG_CONFIG_CFLAGS})

add_library( ${OpenAL_LIB_NAME} STATIC ${OPENAL_OBJS} ${ALC_OBJS} )
