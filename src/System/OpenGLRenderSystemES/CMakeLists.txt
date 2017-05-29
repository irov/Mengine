set( MY_LIB_NAME OpenGLRenderSystemES )

include_directories( ${Mengine_SOURCE_DIR}/Utils)

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

ADD_LIBRARY( ${MY_LIB_NAME} ${SRC_FILES} )

SET( APPLICATION_LIBS
 ${APPLICATION_LIBS}
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