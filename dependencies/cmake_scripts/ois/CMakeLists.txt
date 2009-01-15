
CMAKE_MINIMUM_REQUIRED( VERSION 2.6 )

project(OIS)

SET(TRUE_DIR ../../ois)
SET(OIS_SOURCE_DIR ${TRUE_DIR})

INCLUDE( ../menge_macro.cmake )

MENGE_SET_PROJECT_DEFAULTS()

# SET LIBNAME
# msvc does not append 'lib' - do it here to have consistent name
if(MSVC)
 set(OIS_LIB_NAME lib)
endif(MSVC)
set( OIS_LIB_NAME ${OIS_LIB_NAME}ois_static )

# to distinguish between debug and release lib
set(CMAKE_DEBUG_POSTFIX "d")

if(WIN32)
	include_directories( ../../DirectX/DX8/include )
endif(WIN32)
# OUR SOURCES
set(libois_sources
${TRUE_DIR}/src/OISEffect.cpp
${TRUE_DIR}/src/OISException.cpp
${TRUE_DIR}/src/OISForceFeedback.cpp
${TRUE_DIR}/src/OISInputManager.cpp
${TRUE_DIR}/src/OISJoyStick.cpp
${TRUE_DIR}/src/OISKeyboard.cpp
${TRUE_DIR}/src/OISObject.cpp
${TRUE_DIR}/includes/OIS.h
${TRUE_DIR}/includes/OISConfig.h
${TRUE_DIR}/includes/OISEvents.h
${TRUE_DIR}/includes/OISException.h
${TRUE_DIR}/includes/OISFactoryCreator.h
${TRUE_DIR}/includes/OISInputManager.h
${TRUE_DIR}/includes/OISJoystick.h
${TRUE_DIR}/includes/OISKeyboard.h
${TRUE_DIR}/includes/OISMouse.h
${TRUE_DIR}/includes/OISObject.h
${TRUE_DIR}/includes/OISPrereqs.h
${TRUE_DIR}/includes/OISEffect.h
${TRUE_DIR}/includes/OISForceFeedback.h
${TRUE_DIR}/includes/OISInterface.h
${TRUE_DIR}/src/extras/LIRC/OISLIRC.cpp
${TRUE_DIR}/src/extras/LIRC/OISLIRC.h
${TRUE_DIR}/src/extras/LIRC/OISLIRCFactoryCreator.cpp
${TRUE_DIR}/src/extras/LIRC/OISLIRCFactoryCreator.h
${TRUE_DIR}/src/extras/LIRC/OISLIRCRingBuffer.h
)

if(WIN32)
	set(libois_sources ${libois_sources}
		${TRUE_DIR}/src/win32/Win32ForceFeedback.cpp
		${TRUE_DIR}/src/win32/Win32InputManager.cpp
		${TRUE_DIR}/src/win32/Win32JoyStick.cpp
		${TRUE_DIR}/src/win32/Win32KeyBoard.cpp
		${TRUE_DIR}/src/win32/Win32Mouse.cpp
		${TRUE_DIR}/includes/win32/Win32ForceFeedback.h
		${TRUE_DIR}/includes/win32/Win32InputManager.h
		${TRUE_DIR}/includes/win32/Win32JoyStick.h
		${TRUE_DIR}/includes/win32/Win32KeyBoard.h
		${TRUE_DIR}/includes/win32/Win32Mouse.h
		${TRUE_DIR}/includes/win32/Win32Prereqs.h
		${TRUE_DIR}/src/win32/extras/WiiMote/hiddevice.cpp
		${TRUE_DIR}/src/win32/extras/WiiMote/hiddevice.h
		${TRUE_DIR}/src/win32/extras/WiiMote/OISWiiMote.cpp
		${TRUE_DIR}/src/win32/extras/WiiMote/OISWiiMote.h
		${TRUE_DIR}/src/win32/extras/WiiMote/OISWiiMoteFactoryCreator.cpp
		${TRUE_DIR}/src/win32/extras/WiiMote/OISWiiMoteFactoryCreator.h
		${TRUE_DIR}/src/win32/extras/WiiMote/OISWiiMoteForceFeedback.cpp
		${TRUE_DIR}/src/win32/extras/WiiMote/OISWiiMoteForceFeedback.h
		${TRUE_DIR}/src/win32/extras/WiiMote/OISWiiMoteRingBuffer.cpp
		${TRUE_DIR}/src/win32/extras/WiiMote/OISWiiMoteRingBuffer.h
		${TRUE_DIR}/src/win32/extras/WiiMote/wiimote.cpp
		${TRUE_DIR}/src/win32/extras/WiiMote/wiimote.h
	)
endif(WIN32)

include_directories(${OIS_SOURCE_DIR}/includes)
add_library( ${OIS_LIB_NAME} STATIC ${libois_sources} )
