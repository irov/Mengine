SET( MY_LIB_NAME ServiceMemory )

ADD_FILTER(
src
	MemoryManager.cpp
	MemoryManager.h
    
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
    FOLDER Service
)

SET( APPLICATION_DEPENDENCIES
 ${APPLICATION_DEPENDENCIES}
 ${MY_LIB_NAME}
)


SET( APPLICATION_DEPENDENCIES
 ${APPLICATION_DEPENDENCIES}
 PARENT_SCOPE
)
