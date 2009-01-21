
CMAKE_MINIMUM_REQUIRED( VERSION 2.6 )
                                             
project(OpenAL)

SET(TRUE_DIR ../../OpenAL)
SET(OpenAL_SOURCE_DIR ${TRUE_DIR} )

INCLUDE( ../menge_macro.cmake )

MENGE_SET_PROJECT_DEFAULTS()

# SET LIBNAME
# msvc does not append 'lib' - do it here to have consistent name
if(MSVC)
 set(OpenAL_LIB_NAME lib)
endif(MSVC)
set( OpenAL_LIB_NAME ${OpenAL_LIB_NAME}openal_static )

if(WIN32)
	include_directories( ../../DirectX/DX8/include )
endif(WIN32)

include_directories(${OpenAL_SOURCE_DIR}/include)
 
# OUR SOURCES
set(libopenal_sources
)

if(WIN32)
	set( libopenal_sources ${libopenal_sources}
	${TRUE_DIR}/OpenAL-Windows/OpenAL32/include/alBuffer.h
	${TRUE_DIR}/OpenAL-Windows/OpenAL32/include/alEax.h
	${TRUE_DIR}/OpenAL-Windows/OpenAL32/aleax2.h
	${TRUE_DIR}/OpenAL-Windows/OpenAL32/include/alEaxPresets.h
	${TRUE_DIR}/OpenAL-Windows/OpenAL32/include/alError.h
	${TRUE_DIR}/OpenAL-Windows/OpenAL32/include/alExtension.h
	${TRUE_DIR}/OpenAL-Windows/OpenAL32/include/alListener.h
	${TRUE_DIR}/OpenAL-Windows/OpenAL32/include/alMain.h
	${TRUE_DIR}/OpenAL-Windows/OpenAL32/include/alSource.h
	${TRUE_DIR}/OpenAL-Windows/OpenAL32/include/alState.h
	${TRUE_DIR}/OpenAL-Windows/OpenAL32/alBuffer.c
	${TRUE_DIR}/OpenAL-Windows/OpenAL32/alEax.c
	${TRUE_DIR}/OpenAL-Windows/OpenAL32/aleax2.c
	${TRUE_DIR}/OpenAL-Windows/OpenAL32/alError.c
	${TRUE_DIR}/OpenAL-Windows/OpenAL32/alExtension.c
	${TRUE_DIR}/OpenAL-Windows/OpenAL32/alListener.c
	${TRUE_DIR}/OpenAL-Windows/OpenAL32/alSource.c
	${TRUE_DIR}/OpenAL-Windows/OpenAL32/alState.c
	${TRUE_DIR}/OpenAL-Windows/OpenAL32/alThunk.cpp
	${TRUE_DIR}/OpenAL-Windows/Alc/ALc.c
	${TRUE_DIR}/OpenAL-Windows/Alu/ALu.c
	)

	include_directories(${OpenAL_SOURCE_DIR}/OpenAl-Windows/OpenAL32/Include)

endif(WIN32)

add_definitions( -DAL_STATIC_LIB )


add_library( ${OpenAL_LIB_NAME} STATIC ${libopenal_sources} )
