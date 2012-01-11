
CMAKE_MINIMUM_REQUIRED( VERSION 2.6 )
                                             
project(OpenAL)

SET( SOURCE_DIR ${CMAKE_SOURCE_DIR}/OpenAL )

if(MSVC)
	include_directories( ${CMAKE_SOURCE_DIR}/DirectX/DX8/include )
endif(MSVC)

include_directories(${SOURCE_DIR}/include)
INCLUDE_DIRECTORIES(OpenAL32/Include include "${CMAKE_BINARY_DIR}")
 
#patch
#if( WIN32 )
#	configure_file( ${CMAKE_SOURCE_DIR}/OpenAL/win_patch/al.h ${SOURCE_DIR}/Include/AL/al.h COPYONLY)
#	configure_file( ${CMAKE_SOURCE_DIR}/OpenAL/win_patch/alc.h ${SOURCE_DIR}/Include/AL/alc.h COPYONLY)
#	configure_file( ${CMAKE_SOURCE_DIR}/OpenAL/win_patch/ALc.c ${SOURCE_DIR}/OpenAL-Windows/Alc/ALc.c COPYONLY)
#endif( WIN32 )

# OUR SOURCES
#set(libopenal_sources)
CONFIGURE_FILE(
    "${SOURCE_DIR}/config.h.in"
    "${CMAKE_BINARY_DIR}/config.h")

SET(OPENAL_OBJS  ${SOURCE_DIR}/OpenAL32/alAuxEffectSlot.c
                 ${SOURCE_DIR}/OpenAL32/alBuffer.c
                 ${SOURCE_DIR}/OpenAL32/alDatabuffer.c
                 ${SOURCE_DIR}/OpenAL32/alEffect.c
                 ${SOURCE_DIR}/OpenAL32/alError.c
                 ${SOURCE_DIR}/OpenAL32/alExtension.c
                 ${SOURCE_DIR}/OpenAL32/alFilter.c
                 ${SOURCE_DIR}/OpenAL32/alListener.c
                 ${SOURCE_DIR}/OpenAL32/alSource.c
                 ${SOURCE_DIR}/OpenAL32/alState.c
                 ${SOURCE_DIR}/OpenAL32/alThunk.c
)

SET(ALC_OBJS  ${SOURCE_DIR}/Alc/ALc.c
              ${SOURCE_DIR}/Alc/ALu.c
              ${SOURCE_DIR}/Alc/alcConfig.c
              ${SOURCE_DIR}/Alc/alcEcho.c
              ${SOURCE_DIR}/Alc/alcModulator.c
              ${SOURCE_DIR}/Alc/alcReverb.c
              ${SOURCE_DIR}/Alc/alcRing.c
              ${SOURCE_DIR}/Alc/alcThread.c
              ${SOURCE_DIR}/Alc/bs2b.c
              ${SOURCE_DIR}/Alc/mixer.c
              ${SOURCE_DIR}/Alc/panning.c
              ${SOURCE_DIR}/Alc/null.c
)

include_directories(${SOURCE_DIR}/OpenAL32/Include)

ADD_DEFINITIONS(-DAL_LIBTYPE_STATIC)
SET(PKG_CONFIG_CFLAGS -DAL_LIBTYPE_STATIC ${PKG_CONFIG_CFLAGS})

add_library( ${PROJECT_NAME}_static STATIC ${OPENAL_OBJS} ${ALC_OBJS} )
