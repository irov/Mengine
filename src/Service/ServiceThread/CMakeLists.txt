SET( MY_LIB_NAME ServiceThread )

ADD_FILTER(
src
	ThreadEngine.h
	ThreadEngine.cpp
    ThreadQueue.h
    ThreadQueue.cpp    
    ThreadMutexDummy.h
)

ADD_LIBRARY( ${MY_LIB_NAME} ${SRC_FILES} )

INCLUDE_DIRECTORIES( 
    ${Mengine_SOURCE_DIR} 
    ${Mengine_SOURCE_DIR}/Utils 
    ) 

SET( APPLICATION_DEPENDENCIES
 ${APPLICATION_DEPENDENCIES}
 ${MY_LIB_NAME}
)


SET( APPLICATION_DEPENDENCIES
 ${APPLICATION_DEPENDENCIES}
 PARENT_SCOPE
)
