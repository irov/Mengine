MENGINE_PROJECT(OpenGLRenderSystem)

ADD_FILTER(
src
    OpenGLRenderExtension.cpp
    OpenGLRenderExtension.h
    OpenGLRenderSystem.cpp
    OpenGLRenderSystem.h
    OpenGLRenderErrorHelper.cpp
    OpenGLRenderErrorHelper.h
    OpenGLRenderEnum.cpp
    OpenGLRenderEnum.h
    OpenGLRenderResourceHandler.h
    OpenGLRenderResourceHandler.cpp
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
    OpenGLRenderImageTarget.cpp
    OpenGLRenderImageTarget.h
    OpenGLRenderImageLocked.cpp
    OpenGLRenderImageLocked.h
    OpenGLRenderImageLockedFactoryStorage.h
    OpenGLRenderTargetTexture.cpp
    OpenGLRenderTargetTexture.h
)

if(MENGINE_TARGET_IOS)
    add_definitions(-DGLES_SILENCE_DEPRECATION)
endif()

if(MENGINE_TARGET_IOS_SIMULATOR)
    add_definitions(-DGLES_SILENCE_DEPRECATION)
endif()

IF(MENGINE_PLATFORM_SDL)
    INCLUDE_DIRECTORIES(${THIRDPARTY_DIR}/SDL/include/include-config-$<LOWER_CASE:$<CONFIG>>)
    INCLUDE_DIRECTORIES(${THIRDPARTY_DIR}/SDL/include)
    INCLUDE_DIRECTORIES(${THIRDPARTY_CONFIG_DIR}/SDL)
ENDIF()

ADD_MENGINE_LIBRARY(Systems)

IF(MENGINE_TARGET_WINDOWS)
    TARGET_LINK_LIBRARIES(${PROJECT_NAME} OpenGL32.lib)
ENDIF()

IF(MENGINE_TARGET_UNIX)
    TARGET_LINK_LIBRARIES(${PROJECT_NAME} GL)
ENDIF()

IF(MENGINE_PLATFORM_ANDROID)
    find_package(OpenGLES REQUIRED)
    find_library(GLESv2-lib GLESv2)

    target_link_libraries(${PROJECT_NAME} ${GLESv2-lib})
ENDIF()
