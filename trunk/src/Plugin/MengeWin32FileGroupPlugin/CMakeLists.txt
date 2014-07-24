set( MY_LIB_NAME MengeWin32FileGroupPlugin )

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
    Win32FileMapped.h
    Win32FileMapped.cpp
)

INCLUDE_DIRECTORIES( 
        ${Mengine_SOURCE_DIR}/Utils 
)

add_definitions(-DUNICODE)
add_definitions(-D_UNICODE)

ADD_LIBRARY( ${MY_LIB_NAME} ${SRC_FILES} )

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
    Logger
    Core
    Codec
    Archive
    )

SET( APPLICATION_LIB_PATHS
 ${APPLICATION_LIB_PATHS}
 PARENT_SCOPE
)

SET( APPLICATION_LIBS
 ${APPLICATION_LIBS}
 PARENT_SCOPE
)


