set( MY_LIB_NAME DX9RenderSystem )

ADD_FILTER(
src
	DX9RenderSystem.h
	DX9RenderSystem.cpp
	DX9Texture.h
	DX9Texture.cpp
	DX9RenderTexture.h
	DX9RenderTexture.cpp
    DX9RenderShader.h
    DX9RenderShader.cpp
    DX9RenderProgram.h
    DX9RenderProgram.cpp
    DX9ErrorHelper.h
    DX9ErrorHelper.cpp
)

add_definitions(-DUNICODE)

INCLUDE_DIRECTORIES( 
    ${Mengine_SOURCE_DIR}/Utils
    )

IF(MSVC)
INCLUDE_DIRECTORIES( 
    ${THIRDPARTY_DIR}/DirectX/DX9/Include 
    )
ENDIF(MSVC)

ADD_LIBRARY( ${MY_LIB_NAME} ${SRC_FILES} )

SET( APPLICATION_DEPENDENCIES
 ${APPLICATION_DEPENDENCIES}
 ${MY_LIB_NAME}
)

SET( APPLICATION_DEPENDENCIES
 ${APPLICATION_DEPENDENCIES}
 PARENT_SCOPE
)

SET( APPLICATION_LIB_PATHS
 ${APPLICATION_LIB_PATHS}
 PARENT_SCOPE
)

IF(MSVC)
SET( APPLICATION_LIBS
 ${APPLICATION_LIBS}  
 ${THIRDPARTY_DIR}/DirectX/dx9/lib/DxErr.lib
)
ENDIF(MSVC)

IF(MINGW)
SET( APPLICATION_LIBS
    ${APPLICATION_LIBS} 
    libdxerr9.a
)
ENDIF(MINGW)

SET( APPLICATION_LIBS
 ${APPLICATION_LIBS}  
 PARENT_SCOPE
)
