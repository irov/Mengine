set( MY_LIB_NAME Win32FileGroupPlugin )

ADD_FILTER(
src
	Win32FileGroupPlugin.h
	Win32FileGroupPlugin.cpp
    Win32FileGroupDirectory.cpp
    Win32FileGroupDirectory.h        
	Win32FileInputStream.h
	Win32FileInputStream.cpp
	Win32FileOutputStream.cpp
	Win32FileOutputStream.h
    Win32FileMappedStream.h
    Win32FileMappedStream.cpp
    Win32FileHelper.h
    Win32FileHelper.cpp
)

INCLUDE_DIRECTORIES( 
        ${THIRDPARTY_DIR}/libmath/include
)

add_definitions(-DUNICODE)
add_definitions(-D_UNICODE)

ADD_MENGINE_LIBRARY()

set_target_properties (${MY_LIB_NAME} PROPERTIES
    FOLDER Plugins
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
    Kernel
    )

SET( APPLICATION_LIB_PATHS
 ${APPLICATION_LIB_PATHS}
 PARENT_SCOPE
)

SET( APPLICATION_LIBS
 ${APPLICATION_LIBS}
 PARENT_SCOPE
)


