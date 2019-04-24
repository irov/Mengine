SET( MY_LIB_NAME Win32Platform )

ADD_FILTER(
src
    Win32Platform.h
    Win32Platform.cpp
    Win32DynamicLibrary.h
    Win32DynamicLibrary.cpp
    Win32MouseEvent.h
    Win32MouseEvent.cpp
    Win32FPSMonitor.h
    Win32FPSMonitor.cpp
    Win32AlreadyRunningMonitor.h
    Win32AlreadyRunningMonitor.cpp
    Win32AntifreezeMonitor.h
    Win32AntifreezeMonitor.cpp
)

ADD_MENGINE_LIBRARY()

INCLUDE_DIRECTORIES(${THIRDPARTY_DIR}/libmath/include)
INCLUDE_DIRECTORIES(${THIRDPARTY_DIR}/pybind/include)

SET( APPLICATION_DEPENDENCIES
    ${APPLICATION_DEPENDENCIES}
    ${MY_LIB_NAME}    	
)

SET( APPLICATION_DEPENDENCIES
 ${APPLICATION_DEPENDENCIES}
 PARENT_SCOPE
)

SET( APPLICATION_LIBS
    ${APPLICATION_LIBS}
)

IF(MENGINE_TARGET_WINDOWS OR MENGINE_TARGET_MINGW)
    SET( APPLICATION_LIBS
        ${APPLICATION_LIBS}
        Shlwapi.lib
    )
ENDIF()

SET( APPLICATION_LIBS
    ${APPLICATION_LIBS}
    PARENT_SCOPE
)