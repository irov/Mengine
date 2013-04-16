
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
	Holder.h
	Pool.h
    
    File.h
    File.cpp
    
    FilePath.h
    FilePath.cpp
	
	MemoryProxyInput.h
	MemoryProxyInput.cpp
	
    ValueInterpolator.h
    
    String.h
	String.cpp
    UnicodeFormat.h
    
	PixelFormat.h
	PixelFormat.cpp
	Bitwise.h
    
    Stream.h
	Rect.h
	Rect.cpp
	
    ConstString.h
	ConstString.cpp
    
    ConstStringHolder.h
    ConstStringHolder.cpp
    
	IntrusivePtr.h
	IntrusiveLinked.h
	IntrusiveList.h
	IntrusiveSlug.h
	IntrusiveAlgorithm.h    
    
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
