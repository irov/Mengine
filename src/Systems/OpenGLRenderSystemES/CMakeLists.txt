set(MY_LIB_NAME OpenGLRenderSystemES)

ADD_FILTER(
OpenGLRender
    OpenGLRenderEnumES.h
    OpenGLRenderErrorES.cpp
    OpenGLRenderErrorES.h
    OpenGLRenderFragmentShaderES.cpp
    OpenGLRenderFragmentShaderES.h
    OpenGLRenderHeaderES.h
    OpenGLRenderImageES.cpp
    OpenGLRenderImageES.h
    OpenGLRenderIndexBufferES.cpp
    OpenGLRenderIndexBufferES.h
    OpenGLRenderProgramES.cpp
    OpenGLRenderProgramES.h
    OpenGLRenderShaderES.h
    OpenGLRenderSystemES.cpp
    OpenGLRenderSystemES.h
    OpenGLRenderVertexBufferES.cpp
    OpenGLRenderVertexBufferES.h
    OpenGLRenderVertexShaderES.cpp
    OpenGLRenderVertexShaderES.h
)

ADD_MENGINE_LIBRARY()

set_target_properties (${MY_LIB_NAME} PROPERTIES
    FOLDER Systems
)