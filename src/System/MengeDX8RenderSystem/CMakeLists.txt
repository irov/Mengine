
set( MY_LIB_NAME MengeDX8RenderSystem )

ADD_FILTER(
src
	DX8RenderSystem.h
	DX8RenderSystem.cpp
	DX8Texture.h
	DX8Texture.cpp
	DX8RenderTexture.h
	DX8RenderTexture.cpp
    DX8Error.h
    DX8Error.cpp
)

include_directories( 
${Mengine_SOURCE_DIR}/Utils
${THIRDPARTY_DIR}/DirectX/DX8/Include )

ADD_LIBRARY( ${MY_LIB_NAME} ${SRC_FILES} )

SET( APPLICATION_DEPENDENCIES
 ${APPLICATION_DEPENDENCIES}
 ${MY_LIB_NAME}
)

SET( APPLICATION_DEPENDENCIES
 ${APPLICATION_DEPENDENCIES}
 PARENT_SCOPE
)

SET( APPLICATION_LIBS
 ${APPLICATION_LIBS}
 PARENT_SCOPE
)

SET( APPLICATION_LIB_PATHS
 ${APPLICATION_LIB_PATHS}
 PARENT_SCOPE
)
