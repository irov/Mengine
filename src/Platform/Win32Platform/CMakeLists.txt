SET( MY_LIB_NAME Win32Platform )

ADD_FILTER(
src
    Win32Platform.h
    Win32Platform.cpp
    Win32MouseEvent.h
    Win32MouseEvent.cpp
    Win32FPSMonitor.h
    Win32FPSMonitor.cpp
    Win32AlreadyRunningMonitor.h
    Win32AlreadyRunningMonitor.cpp
)

add_definitions(-DUNICODE)

ADD_MENGINE_LIBRARY()

INCLUDE_DIRECTORIES( ${THIRDPARTY_DIR}/libmath/include ) 

SET( APPLICATION_DEPENDENCIES
 ${APPLICATION_DEPENDENCIES}
 ${MY_LIB_NAME}
 Shlwapi.lib
)


SET( APPLICATION_DEPENDENCIES
 ${APPLICATION_DEPENDENCIES}
 PARENT_SCOPE
)