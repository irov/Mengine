SET( MY_LIB_NAME Factory )

ADD_FILTER(
src
    Factorable.h
    Factorable.cpp    
	Factory.h
	Factory.cpp
    FactoryManager.h
    FactoryManager.cpp
    FactoryDefault.h
    FactoryPool.h
)

ADD_LIBRARY( ${MY_LIB_NAME} ${SRC_FILES} )

INCLUDE_DIRECTORIES( ${Mengine_SOURCE_DIR}/Utils ) 

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
