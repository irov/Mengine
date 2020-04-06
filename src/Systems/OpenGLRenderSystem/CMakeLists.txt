MENGINE_PROJECT(OpenGLRenderSystem)

ADD_FILTER(
src
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

include_directories(${THIRDPARTY_DIR}/SDL2/include)

ADD_MENGINE_LIBRARY(Systems)

IF(MENGINE_TARGET_WINDOWS OR MENGINE_TARGET_MINGW)
    TARGET_LINK_LIBRARIES(${PROJECT_NAME} OpenGL32.lib)
ENDIF()

IF(MENGINE_TARGET_UNIX)
    TARGET_LINK_LIBRARIES(${PROJECT_NAME} GL)
ENDIF()