set( MY_LIB_NAME OpenGLRenderSystem )

ADD_FILTER(
src
    OpenGLRenderHeader.h
    OpenGLRenderExtension.cpp
    OpenGLRenderExtension.h
    OpenGLRenderSystem.cpp
    OpenGLRenderSystem.h
    OpenGLRenderError.cpp
    OpenGLRenderError.h
    OpenGLRenderEnum.cpp
    OpenGLRenderEnum.h
    OpenGLRenderFragmentShader.cpp
    OpenGLRenderFragmentShader.h
    OpenGLRenderIndexBuffer.cpp
    OpenGLRenderIndexBuffer.h
    OpenGLRenderProgram.cpp
    OpenGLRenderProgram.h
    OpenGLRenderProgramVariable.cpp
    OpenGLRenderProgramVariable.h
    OpenGLRenderVertexAttribute.cpp
    OpenGLRenderVertexAttribute.h
    OpenGLRenderVertexBuffer.cpp
    OpenGLRenderVertexBuffer.h
    OpenGLRenderVertexShader.cpp
    OpenGLRenderVertexShader.h
    OpenGLRenderImage.cpp
    OpenGLRenderImage.h
)

if(MENGINE_TARGET_IOS)
    add_definitions(-DGLES_SILENCE_DEPRECATION)
endif()

include_directories( 
    ${THIRDPARTY_DIR}/SDL2/include
    ${THIRDPARTY_DIR}/libmath/include
    )

ADD_MENGINE_LIBRARY()

ADD_FRAMEWORK(${MY_LIB_NAME} OpenGLES)

set_target_properties (${MY_LIB_NAME} PROPERTIES
    FOLDER Systems
)

SET( APPLICATION_DEPENDENCIES
 ${APPLICATION_DEPENDENCIES}
 ${MY_LIB_NAME} 
)

SET( APPLICATION_DEPENDENCIES
 ${APPLICATION_DEPENDENCIES}
 PARENT_SCOPE
)

IF(MENGINE_TARGET_WINDOWS OR MENGINE_TARGET_MINGW)
    SET( APPLICATION_LIBS
        ${APPLICATION_LIBS}
        OpenGL32.lib
    )
ENDIF()

SET( APPLICATION_LIBS
    ${APPLICATION_LIBS}
    PARENT_SCOPE
)