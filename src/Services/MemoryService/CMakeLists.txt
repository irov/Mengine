SET(MY_LIB_NAME MemoryService)

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

ADD_MENGINE_LIBRARY(Services)