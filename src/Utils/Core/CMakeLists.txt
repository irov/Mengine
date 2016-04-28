
set( MY_LIB_NAME Core )

ADD_FILTER(
src
    CRC32.h
    CRC32.cpp
    
    Date.h
    Date.cpp
    
    Exception.h
    Visitor.h
    
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
    
    Hash.h
    Hash.cpp
    
    FilePath.h
    FilePath.cpp
    
    FileLogger.cpp
    FileLogger.h
    
    PixelFormat.h
    PixelFormat.cpp
    
    MemoryAllocator.h
    
    MemoryHelper.h
    MemoryHelper.cpp
	
    ValueInterpolator.h
    ValueFollower.h
    
    String.h
	String.cpp
    
    StringFormat.h
    
    Params.h
    
	Rect.h
	Rect.cpp
    
    Stream.h
    Stream.cpp
    
    Pointer.h
	
    ConstString.h
        
    RenderUtil.h
    
    OutputStreamWriter.h   
    
    IntrusiveTree.h
    IntrusiveSplayTree.h
    IntrusiveDuplexTree.h
    
    PluginBase.h
    PluginBase.cpp
    
    ModuleBase.h
    ModuleBase.cpp
    
    ModuleFactory.h
   
    Magic.h    
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
