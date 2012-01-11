
CMAKE_MINIMUM_REQUIRED( VERSION 2.6 )

project( ois )

SET( SOURCE_DIR ${CMAKE_SOURCE_DIR}/ois )

if(WIN32)
	include_directories( ${CMAKE_SOURCE_DIR}/../DirectX/DX8/include )
endif(WIN32)
# OUR SOURCES
set( ois_sources
${SOURCE_DIR}/src/OISEffect.cpp
${SOURCE_DIR}/src/OISException.cpp
${SOURCE_DIR}/src/OISForceFeedback.cpp
${SOURCE_DIR}/src/OISInputManager.cpp
${SOURCE_DIR}/src/OISJoyStick.cpp
${SOURCE_DIR}/src/OISKeyboard.cpp
${SOURCE_DIR}/src/OISObject.cpp
${SOURCE_DIR}/includes/OIS.h
${SOURCE_DIR}/includes/OISConfig.h
${SOURCE_DIR}/includes/OISEvents.h
${SOURCE_DIR}/includes/OISException.h
${SOURCE_DIR}/includes/OISFactoryCreator.h
${SOURCE_DIR}/includes/OISInputManager.h
${SOURCE_DIR}/includes/OISJoystick.h
${SOURCE_DIR}/includes/OISKeyboard.h
${SOURCE_DIR}/includes/OISMouse.h
${SOURCE_DIR}/includes/OISObject.h
${SOURCE_DIR}/includes/OISPrereqs.h
${SOURCE_DIR}/includes/OISEffect.h
${SOURCE_DIR}/includes/OISForceFeedback.h
${SOURCE_DIR}/includes/OISInterface.h
)

if(WIN32)
	set(ois_sources ${ois_sources}
		${SOURCE_DIR}/src/win32/Win32ForceFeedback.cpp
		${SOURCE_DIR}/src/win32/Win32InputManager.cpp
		${SOURCE_DIR}/src/win32/Win32JoyStick.cpp
		${SOURCE_DIR}/src/win32/Win32KeyBoard.cpp
		${SOURCE_DIR}/src/win32/Win32Mouse.cpp
		${SOURCE_DIR}/includes/win32/Win32ForceFeedback.h
		${SOURCE_DIR}/includes/win32/Win32InputManager.h
		${SOURCE_DIR}/includes/win32/Win32JoyStick.h
		${SOURCE_DIR}/includes/win32/Win32KeyBoard.h
		${SOURCE_DIR}/includes/win32/Win32Mouse.h
		${SOURCE_DIR}/includes/win32/Win32Prereqs.h
		${SOURCE_DIR}/src/win32/extras/WiiMote/hiddevice.cpp
		${SOURCE_DIR}/src/win32/extras/WiiMote/hiddevice.h
		${SOURCE_DIR}/src/win32/extras/WiiMote/OISWiiMote.cpp
		${SOURCE_DIR}/src/win32/extras/WiiMote/OISWiiMote.h
		${SOURCE_DIR}/src/win32/extras/WiiMote/OISWiiMoteFactoryCreator.cpp
		${SOURCE_DIR}/src/win32/extras/WiiMote/OISWiiMoteFactoryCreator.h
		${SOURCE_DIR}/src/win32/extras/WiiMote/OISWiiMoteForceFeedback.cpp
		${SOURCE_DIR}/src/win32/extras/WiiMote/OISWiiMoteForceFeedback.h
		${SOURCE_DIR}/src/win32/extras/WiiMote/OISWiiMoteRingBuffer.cpp
		${SOURCE_DIR}/src/win32/extras/WiiMote/OISWiiMoteRingBuffer.h
		${SOURCE_DIR}/src/win32/extras/WiiMote/wiimote.cpp
		${SOURCE_DIR}/src/win32/extras/WiiMote/wiimote.h
		${SOURCE_DIR}/src/extras/LIRC/OISLIRC.cpp
		${SOURCE_DIR}/src/extras/LIRC/OISLIRC.h
		${SOURCE_DIR}/src/extras/LIRC/OISLIRCFactoryCreator.cpp
		${SOURCE_DIR}/src/extras/LIRC/OISLIRCFactoryCreator.h
		${SOURCE_DIR}/src/extras/LIRC/OISLIRCRingBuffer.h
	)

elseif(APPLE)
	set( ois_sources ${ois_sources}
		${SOURCE_DIR}/src/mac/MacHelpers.cpp
		${SOURCE_DIR}/src/mac/MacInputManager.cpp
		${SOURCE_DIR}/src/mac/MacKeyboard.cpp
		${SOURCE_DIR}/src/mac/MacMouse.cpp
		${SOURCE_DIR}/src/mac/MacHIDManager.cpp
	)
endif(WIN32)

include_directories(${SOURCE_DIR}/includes)
add_library( ${PROJECT_NAME}_static STATIC ${ois_sources} )
