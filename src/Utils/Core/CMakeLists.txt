
set( MY_LIB_NAME Core )

ADD_FILTER(
src
    Assertion.h
    Assertion.cpp

    CRC32.h
    CRC32.cpp
	
	ServantBase.h
	ServiceBase.h
	
	FontBase.h
	FontBase.cpp
    
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
    
    Tags.h
    Tags.cpp
    
    StringFormat.h
    
    Params.h
    
	Rect.h
	Rect.cpp
    
    Stream.h
    Stream.cpp
    
    Pointer.h
	
    ConstString.h
    ConstString.cpp
	ConstStringHolder.h
    ConstStringHolder.cpp
	ConstStringHolderLocalString.h
	ConstStringHolderLocalString.cpp
	ConstStringTypes.h
    
    UID.h
    UID.cpp        
        
    RenderUtils.h
    
    OutputStreamWriter.h   
    
    IntrusiveTree.h
    
    PluginBase.h
    PluginBase.cpp
    
    ModuleBase.h
    ModuleBase.cpp
    
    ModuleFactory.h
    
    Holder.h
    
    RenderVertex2D.h
    RenderIndex.h
   
    Magic.h    
    Mixin.h
)

ADD_LIBRARY( ${MY_LIB_NAME} ${SRC_FILES} )

set_target_properties (${MY_LIB_NAME} PROPERTIES
    FOLDER Utils
)

INCLUDE_DIRECTORIES(    ${Mengine_SOURCE_DIR}/Utils 
                        ${THIRDPARTY_DIR}/boost
						${THIRDPARTY_DIR}/utf8/source
                        ${THIRDPARTY_DIR}/libmath/include
                        ${THIRDPARTY_DIR}/tinyini/include
                        ) 

SET( APPLICATION_DEPENDENCIES
 ${APPLICATION_DEPENDENCIES}
 ${MY_LIB_NAME}
)


SET( APPLICATION_DEPENDENCIES
 ${APPLICATION_DEPENDENCIES}
 PARENT_SCOPE
)
