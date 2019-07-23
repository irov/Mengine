set(MY_LIB_NAME OpenGLRenderSystem)

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

set_target_properties (${MY_LIB_NAME} PROPERTIES
    FOLDER Systems
)

IF(MENGINE_TARGET_WINDOWS OR MENGINE_TARGET_MINGW)
    TARGET_LINK_LIBRARIES(${MY_LIB_NAME} OpenGL32.lib)
ENDIF