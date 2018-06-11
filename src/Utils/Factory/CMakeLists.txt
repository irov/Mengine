SET( MY_LIB_NAME Factory )

ADD_FILTER(
src
    Factorable.h
    Factorable.cpp  
	FactorablePointer.h
    FactorableUnique.h
	Factory.h
	Factory.cpp
    FactoryWithListener.h
    FactoryWithListener.cpp
    FactoryWithMutex.h
    FactoryWithMutex.cpp	
    FactoryDefault.h    
    FactoryPool.h
    FactoryPoolWithListener.h
)

ADD_MENGINE_LIBRARY()

set_target_properties (${MY_LIB_NAME} PROPERTIES
    FOLDER Utils
)

INCLUDE_DIRECTORIES( ${Mengine_SOURCE_DIR}/Utils ) 

SET( APPLICATION_DEPENDENCIES
 ${APPLICATION_DEPENDENCIES}
 ${MY_LIB_NAME}
)

SET( APPLICATION_DEPENDENCIES
 ${APPLICATION_DEPENDENCIES}
 PARENT_SCOPE
)
