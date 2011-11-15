
CMAKE_MINIMUM_REQUIRED( VERSION 2.6 )

project( ois )

SET( CMAKE_CURRENT_SOURCE_DIR ${CMAKE_SOURCE_DIR}/ois )

INCLUDE( ../menge_macro.cmake )

MENGE_SET_PROJECT_DEFAULTS( ois )

if(WIN32)
	include_directories( ${CMAKE_SOURCE_DIR}/../DirectX/DX8/include )
endif(WIN32)
# OUR SOURCES
set( ois_sources
${CMAKE_CURRENT_SOURCE_DIR}/src/OISEffect.cpp
${CMAKE_CURRENT_SOURCE_DIR}/src/OISException.cpp
${CMAKE_CURRENT_SOURCE_DIR}/src/OISForceFeedback.cpp
${CMAKE_CURRENT_SOURCE_DIR}/src/OISInputManager.cpp
${CMAKE_CURRENT_SOURCE_DIR}/src/OISJoyStick.cpp
${CMAKE_CURRENT_SOURCE_DIR}/src/OISKeyboard.cpp
${CMAKE_CURRENT_SOURCE_DIR}/src/OISObject.cpp
${CMAKE_CURRENT_SOURCE_DIR}/includes/OIS.h
${CMAKE_CURRENT_SOURCE_DIR}/includes/OISConfig.h
${CMAKE_CURRENT_SOURCE_DIR}/includes/OISEvents.h
${CMAKE_CURRENT_SOURCE_DIR}/includes/OISException.h
${CMAKE_CURRENT_SOURCE_DIR}/includes/OISFactoryCreator.h
${CMAKE_CURRENT_SOURCE_DIR}/includes/OISInputManager.h
${CMAKE_CURRENT_SOURCE_DIR}/includes/OISJoystick.h
${CMAKE_CURRENT_SOURCE_DIR}/includes/OISKeyboard.h
${CMAKE_CURRENT_SOURCE_DIR}/includes/OISMouse.h
${CMAKE_CURRENT_SOURCE_DIR}/includes/OISObject.h
${CMAKE_CURRENT_SOURCE_DIR}/includes/OISPrereqs.h
${CMAKE_CURRENT_SOURCE_DIR}/includes/OISEffect.h
${CMAKE_CURRENT_SOURCE_DIR}/includes/OISForceFeedback.h
${CMAKE_CURRENT_SOURCE_DIR}/includes/OISInterface.h
)

if(WIN32)
	set(ois_sources ${ois_sources}
		${CMAKE_CURRENT_SOURCE_DIR}/src/win32/Win32ForceFeedback.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/src/win32/Win32InputManager.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/src/win32/Win32JoyStick.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/src/win32/Win32KeyBoard.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/src/win32/Win32Mouse.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/includes/win32/Win32ForceFeedback.h
		${CMAKE_CURRENT_SOURCE_DIR}/includes/win32/Win32InputManager.h
		${CMAKE_CURRENT_SOURCE_DIR}/includes/win32/Win32JoyStick.h
		${CMAKE_CURRENT_SOURCE_DIR}/includes/win32/Win32KeyBoard.h
		${CMAKE_CURRENT_SOURCE_DIR}/includes/win32/Win32Mouse.h
		${CMAKE_CURRENT_SOURCE_DIR}/includes/win32/Win32Prereqs.h
		${CMAKE_CURRENT_SOURCE_DIR}/src/win32/extras/WiiMote/hiddevice.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/src/win32/extras/WiiMote/hiddevice.h
		${CMAKE_CURRENT_SOURCE_DIR}/src/win32/extras/WiiMote/OISWiiMote.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/src/win32/extras/WiiMote/OISWiiMote.h
		${CMAKE_CURRENT_SOURCE_DIR}/src/win32/extras/WiiMote/OISWiiMoteFactoryCreator.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/src/win32/extras/WiiMote/OISWiiMoteFactoryCreator.h
		${CMAKE_CURRENT_SOURCE_DIR}/src/win32/extras/WiiMote/OISWiiMoteForceFeedback.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/src/win32/extras/WiiMote/OISWiiMoteForceFeedback.h
		${CMAKE_CURRENT_SOURCE_DIR}/src/win32/extras/WiiMote/OISWiiMoteRingBuffer.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/src/win32/extras/WiiMote/OISWiiMoteRingBuffer.h
		${CMAKE_CURRENT_SOURCE_DIR}/src/win32/extras/WiiMote/wiimote.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/src/win32/extras/WiiMote/wiimote.h
		${CMAKE_CURRENT_SOURCE_DIR}/src/extras/LIRC/OISLIRC.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/src/extras/LIRC/OISLIRC.h
		${CMAKE_CURRENT_SOURCE_DIR}/src/extras/LIRC/OISLIRCFactoryCreator.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/src/extras/LIRC/OISLIRCFactoryCreator.h
		${CMAKE_CURRENT_SOURCE_DIR}/src/extras/LIRC/OISLIRCRingBuffer.h
	)

elseif(APPLE)
	set( ois_sources ${ois_sources}
		${CMAKE_CURRENT_SOURCE_DIR}/src/mac/MacHelpers.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/src/mac/MacInputManager.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/src/mac/MacKeyboard.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/src/mac/MacMouse.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/src/mac/MacHIDManager.cpp
	)
endif(WIN32)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/includes)
add_library( ${ois_LIB_NAME} STATIC ${ois_sources} )
