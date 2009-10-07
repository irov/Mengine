
set( MY_LIB_NAME OGLRenderSystem )

if( MENGE_USE_MD )
	set( MY_LIB_NAME ${MY_LIB_NAME}_md )
endif( MENGE_USE_MD )

ADD_FILTER(
src
	OGLBatching.cpp
	OGLRenderTexture.cpp
	OGLRenderTexture.h
	OGLRenderTextureCopy.cpp
	OGLRenderTextureCopy.h
	OGLRenderTextureFBO.cpp
	OGLRenderTextureFBO.h
	OGLTexture.cpp
	OGLTexture.h
	OGLUtils.cpp
	OGLUtils.h
	OGLRenderSystem.cpp
	OGLRenderSystem.h
	OGLRenderSystem.def
)

ADD_LIBRARY( ${MY_LIB_NAME} ${SRC_FILES} )

SET( APPLICATION_DEPENDENCIES
 ${APPLICATION_DEPENDENCIES}
 ${MY_LIB_NAME}
)

SET( APPLICATION_DEPENDENCIES
 ${APPLICATION_DEPENDENCIES}
 PARENT_SCOPE
)