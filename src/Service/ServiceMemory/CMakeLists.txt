SET( MY_LIB_NAME MemoryService )

ADD_FILTER(
src
	MemoryService.cpp
	MemoryService.h
    
    MemoryBuffer.h
    MemoryBuffer.cpp
    
    MemoryProxy.h
    MemoryProxy.cpp
	
	MemoryInput.h
	MemoryInput.cpp
        
	MemoryProxyInput.h
	MemoryProxyInput.cpp

    MemoryCacheInput.h
    MemoryCacheInput.cpp
    
    MemoryCacheBuffer.h
    MemoryCacheBuffer.cpp    
)

ADD_MENGINE_LIBRARY()

set_target_properties (${MY_LIB_NAME} PROPERTIES
    FOLDER Services
)

SET( APPLICATION_DEPENDENCIES
 ${APPLICATION_DEPENDENCIES}
 ${MY_LIB_NAME}
)


SET( APPLICATION_DEPENDENCIES
 ${APPLICATION_DEPENDENCIES}
 PARENT_SCOPE
)
