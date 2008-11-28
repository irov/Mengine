
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

ADD_LIBRARY( OGLRenderSystem ${SRC_FILES} )