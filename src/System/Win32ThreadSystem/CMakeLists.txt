set( MY_LIB_NAME Win32ThreadSystem )

ADD_FILTER(
src
	Win32ThreadSystem.cpp
	Win32ThreadSystem.h
	Win32ThreadIdentity.cpp
    Win32ThreadIdentity.h
    Win32ThreadMutex.h
    Win32ThreadMutex.cpp
)

ADD_LIBRARY( ${MY_LIB_NAME} ${SRC_FILES} )

set_target_properties (${MY_LIB_NAME} PROPERTIES
    FOLDER System
)

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
 PARENT_SCOPE
)
