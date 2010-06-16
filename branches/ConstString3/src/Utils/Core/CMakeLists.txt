
set( MY_LIB_NAME Core )

if( MENGE_USE_MT )
	set( MY_LIB_NAME ${MY_LIB_NAME}_mt )
endif( MENGE_USE_MT )

ADD_FILTER(
src
	Viewport.cpp
	
	Viewport.h
	Resolution.h
	Resolution.cpp	
	ColourValue.h
	ColourValue.cpp
	Holder.h
	Pool.h
	File.h
	File.cpp
	String.h
	String.cpp
	PixelFormat.h
	PixelFormat.cpp
	Bitwise.h
	ConstString.h
	ConstString.cpp

)

ADD_LIBRARY( ${MY_LIB_NAME} ${SRC_FILES} )

INCLUDE_DIRECTORIES( ${Mengine_SOURCE_DIR}/Utils 
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
