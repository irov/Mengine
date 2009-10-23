
CMAKE_MINIMUM_REQUIRED( VERSION 2.6 )
                                             
project(OpenAL)

SET( CMAKE_CURRENT_SOURCE_DIR ${CMAKE_SOURCE_DIR}/../OpenAL )

INCLUDE( ../menge_macro.cmake )

MENGE_SET_PROJECT_DEFAULTS( OpenAL )

if(MSVC)
	include_directories( ${CMAKE_SOURCE_DIR}/../DirectX/DX8/include )
endif(MSVC)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)
 
#patch
if( WIN32 )
	configure_file( ${CMAKE_SOURCE_DIR}/OpenAL/win_patch/al.h ${CMAKE_CURRENT_SOURCE_DIR}/Include/AL/al.h COPYONLY)
	configure_file( ${CMAKE_SOURCE_DIR}/OpenAL/win_patch/alc.h ${CMAKE_CURRENT_SOURCE_DIR}/Include/AL/alc.h COPYONLY)
	configure_file( ${CMAKE_SOURCE_DIR}/OpenAL/win_patch/ALc.c ${CMAKE_CURRENT_SOURCE_DIR}/OpenAL-Windows/Alc/ALc.c COPYONLY)
endif( WIN32 )

# OUR SOURCES
set(libopenal_sources
)

if(WIN32)
	set( openal_sources ${openal_sources}
	${CMAKE_CURRENT_SOURCE_DIR}/OpenAL-Windows/OpenAL32/include/alBuffer.h
	${CMAKE_CURRENT_SOURCE_DIR}/OpenAL-Windows/OpenAL32/include/alEax.h
	${CMAKE_CURRENT_SOURCE_DIR}/OpenAL-Windows/OpenAL32/aleax2.h
	${CMAKE_CURRENT_SOURCE_DIR}/OpenAL-Windows/OpenAL32/include/alEaxPresets.h
	${CMAKE_CURRENT_SOURCE_DIR}/OpenAL-Windows/OpenAL32/include/alError.h
	${CMAKE_CURRENT_SOURCE_DIR}/OpenAL-Windows/OpenAL32/include/alExtension.h
	${CMAKE_CURRENT_SOURCE_DIR}/OpenAL-Windows/OpenAL32/include/alListener.h
	${CMAKE_CURRENT_SOURCE_DIR}/OpenAL-Windows/OpenAL32/include/alMain.h
	${CMAKE_CURRENT_SOURCE_DIR}/OpenAL-Windows/OpenAL32/include/alSource.h
	${CMAKE_CURRENT_SOURCE_DIR}/OpenAL-Windows/OpenAL32/include/alState.h
	${CMAKE_CURRENT_SOURCE_DIR}/OpenAL-Windows/OpenAL32/alBuffer.c
	${CMAKE_CURRENT_SOURCE_DIR}/OpenAL-Windows/OpenAL32/alEax.c
	${CMAKE_CURRENT_SOURCE_DIR}/OpenAL-Windows/OpenAL32/aleax2.c
	${CMAKE_CURRENT_SOURCE_DIR}/OpenAL-Windows/OpenAL32/alError.c
	${CMAKE_CURRENT_SOURCE_DIR}/OpenAL-Windows/OpenAL32/alExtension.c
	${CMAKE_CURRENT_SOURCE_DIR}/OpenAL-Windows/OpenAL32/alListener.c
	${CMAKE_CURRENT_SOURCE_DIR}/OpenAL-Windows/OpenAL32/alSource.c
	${CMAKE_CURRENT_SOURCE_DIR}/OpenAL-Windows/OpenAL32/alState.c
	${CMAKE_CURRENT_SOURCE_DIR}/OpenAL-Windows/OpenAL32/alThunk.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/OpenAL-Windows/Alc/ALc.c
	${CMAKE_CURRENT_SOURCE_DIR}/OpenAL-Windows/Alu/ALu.c
	)

	include_directories(${CMAKE_CURRENT_SOURCE_DIR}/OpenAl-Windows/OpenAL32/Include)

elseif(APPLE)
	set( openal_sources ${libopenal_sources}
	${CMAKE_CURRENT_SOURCE_DIR}/OpenAL-MacOSX/oalContext.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/OpenAL-MacOSX/oalDevice.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/OpenAL-MacOSX/oalImp.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/OpenAL-MacOSX/oalOSX.cpp
	/Developer/Examples/CoreAudio/PublicUtility/CAGuard.cpp
	/Developer/Examples/CoreAudio/PublicUtility/CAHostTimeBase.cpp
	/Developer/Examples/CoreAudio/PublicUtility/CAMutex.cpp
	/Developer/Examples/CoreAudio/PublicUtility/CAStreamBasicDescription.cpp
	/Developer/Examples/CoreAudio/PublicUtility/CADebugger.cpp
	/Developer/Examples/CoreAudio/PublicUtility/CADebugMacros.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/OpenAL-MacOSX/oalSource.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/OpenAL-MacOSX/oalBuffer.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/OpenAL-MacOSX/oalCaptureDevice.cpp
	/Developer/Examples/CoreAudio/PublicUtility/CABufferList.cpp
	)

	include_directories(
	${CMAKE_CURRENT_SOURCE_DIR}/OpenAL-MacOSX 
	${CMAKE_CURRENT_SOURCE_DIR}/OpenAL-MacOSX/Support 
	${CMAKE_CURRENT_SOURCE_DIR}/include/al 
	/Developer/Examples/CoreAudio/PublicUtility
	)

endif(WIN32)

add_definitions( -DAL_STATIC_LIB )

add_library( ${OpenAL_LIB_NAME} STATIC ${openal_sources} )
