set( MY_LIB_NAME OpenGLRenderSystem )

include_directories( ${THIRDPARTY_DIR}/SDL2/include ${Mengine_SOURCE_DIR}/Utils)

IF(MENGINE_PLATFORM_WIN32)
ADD_FILTER(
OpenGLRenderHeader
    OpenGLRenderHeaderWin32.cpp
    OpenGLRenderHeaderWin32.h
)
ENDIF()

IF(MENGINE_PLATFORM_IOS)
ADD_FILTER(
OpenGLRenderHeader
    OpenGLRenderHeaderIOS.h
)
ENDIF()

IF(MENGINE_PLATFORM_MAC)
ADD_FILTER(
OpenGLRenderHeader
    OpenGLRenderHeaderMacOS.h
)
ENDIF()

ADD_FILTER(
OpenGLRender
    OpenGLRenderHeader.h
    OpenGLRenderSystem.cpp
    OpenGLRenderSystem.h
    OpenGLRenderError.cpp
    OpenGLRenderError.h
    OpenGLRenderEnum.h
    OpenGLRenderFragmentShader.cpp
    OpenGLRenderFragmentShader.h
    OpenGLRenderIndexBuffer.cpp
    OpenGLRenderIndexBuffer.h
    OpenGLRenderProgram.cpp
    OpenGLRenderProgram.h
    OpenGLRenderShader.h
    OpenGLRenderVertexBuffer.cpp
    OpenGLRenderVertexBuffer.h
    OpenGLRenderVertexShader.cpp
    OpenGLRenderVertexShader.h
    OpenGLRenderImage.cpp
    OpenGLRenderImage.h
)

ADD_LIBRARY( ${MY_LIB_NAME} ${SRC_FILES} )

TARGET_LINK_LIBRARIES( ${MY_LIB_NAME} ${THIRDPARTY_LIB_DIR}/${MENGINE_LIB_PREFIX}SDL2${MENGINE_LIB_SUFFIX})

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