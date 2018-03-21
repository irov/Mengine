set( MY_LIB_NAME PosixFileGroupPlugin )

ADD_FILTER(
src
	PosixFileGroupPlugin.h
	PosixFileGroupPlugin.cpp
    PosixFileGroupDirectory.cpp
    PosixFileGroupDirectory.h        
	PosixFileInputStream.h
	PosixFileInputStream.cpp
	PosixFileOutputStream.cpp
	PosixFileOutputStream.h
    PosixFileMapped.h
    PosixFileMapped.cpp
)

INCLUDE_DIRECTORIES( 
        ${Mengine_SOURCE_DIR}/Utils 
)

add_definitions(-DUNICODE)
add_definitions(-D_UNICODE)

ADD_LIBRARY( ${MY_LIB_NAME} ${SRC_FILES} )

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


