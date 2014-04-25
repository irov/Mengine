
set( MY_LIB_NAME Core )

ADD_FILTER(
src
    CRC32.h
    CRC32.cpp
    
    Polygon.h
    Polygon.cpp    
    
	Viewport.h
	Viewport.cpp
	
	Resolution.h
	Resolution.cpp	

	ColourValue.h
	ColourValue.cpp
        
    GlyphChar.h
    
    IniUtil.h
    IniUtil.cpp
    
    File.h
    File.cpp
    
    FilePath.h
    FilePath.cpp
    
    FileLogger.cpp
    FileLogger.h
	
	MemoryInput.h
	MemoryInput.cpp
        
	MemoryProxyInput.h
	MemoryProxyInput.cpp

    MemoryCacheInput.h
    MemoryCacheInput.cpp
    
    CacheMemoryBuffer.h
    CacheMemoryBuffer.cpp
	
    ValueInterpolator.h
    
    MutexGuard.h
    MutexGuard.cpp
    
    String.h
	String.cpp
    UnicodeFormat.h
    
    Stream.h
	Rect.h
	Rect.cpp
	
    ConstString.h
    
    BlobjectRead.h
    
    RenderUtil.h
    
    InputStreamBuffer.h
    
    OutputStreamWriter.h
)

ADD_LIBRARY( ${MY_LIB_NAME} ${SRC_FILES} )

INCLUDE_DIRECTORIES(    ${Mengine_SOURCE_DIR}/Utils 
                        ${THIRDPARTY_DIR}/boost
                        ) 

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
