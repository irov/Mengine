SET( MY_LIB_NAME Factory )

ADD_FILTER(
src
    Factorable.h
    Factorable.cpp  
    FactorableUnique.h
	Factory.h
	Factory.cpp
    FactoryStore.h
    FactoryManager.h
    FactoryManager.cpp
    FactoryDefault.h    
    FactoryPool.h
    FactorablePtr.h
    FactorablePtr.cpp
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
