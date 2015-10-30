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

ADD_LIBRARY( ${MY_LIB_NAME} ${SRC_FILES} )

INCLUDE_DIRECTORIES( 
    ${Mengine_SOURCE_DIR} 
    ${Mengine_SOURCE_DIR}/Utils
    ) 
    
SET( MENGE_DEPENDENCIES
 ${MENGE_DEPENDENCIES}
 ${MY_LIB_NAME}
)
 
SET( MENGE_DEPENDENCIES
 ${MENGE_DEPENDENCIES}
 PARENT_SCOPE
)

SET( APPLICATION_DEPENDENCIES
 ${APPLICATION_DEPENDENCIES}
 ${MY_LIB_NAME}
)


SET( APPLICATION_DEPENDENCIES
 ${APPLICATION_DEPENDENCIES}
 PARENT_SCOPE
)