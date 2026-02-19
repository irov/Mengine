MENGINE_PROJECT(DX11RenderSystem)

ADD_FILTER(
src
    DX11RenderSystem.h
    DX11RenderSystem.cpp
    DX11RenderImage.h
    DX11RenderImage.cpp
    DX11RenderImageTarget.h
    DX11RenderImageTarget.cpp
    DX11RenderImageLocked.h
    DX11RenderImageLocked.cpp
    DX11RenderImageLockedFactoryStorage.h
    DX11RenderTargetTexture.h
    DX11RenderTargetTexture.cpp
    DX11RenderTargetOffscreen.h
    DX11RenderTargetOffscreen.cpp
    DX11RenderVertexBuffer.h
    DX11RenderVertexBuffer.cpp
    DX11RenderVertexAttribute.h
    DX11RenderVertexAttribute.cpp
    DX11RenderIndexBuffer.h
    DX11RenderIndexBuffer.cpp
    DX11RenderFragmentShader.h
    DX11RenderFragmentShader.cpp
    DX11RenderVertexShader.h
    DX11RenderVertexShader.cpp
    DX11RenderProgram.h
    DX11RenderProgram.cpp
    DX11RenderEnum.h
    DX11RenderErrorHelper.h
    DX11RenderErrorHelper.cpp
    DX11RenderBuffer.h
    DX11RenderBuffer.cpp
    DX11RenderProgramVariable.h
    DX11RenderProgramVariable.cpp
    DX11RenderMaterialStageCache.h
    DX11RenderMaterialStageCache.cpp
    DX11RenderResourceHandler.h
    DX11RenderResourceHandler.cpp
)

IF(MENGINE_PLATFORM_SDL2)
    INCLUDE_DIRECTORIES(${THIRDPARTY_CONFIG_DIR}/SDL2/include/include-config-$<LOWER_CASE:$<CONFIG>>)
    INCLUDE_DIRECTORIES(${THIRDPARTY_CONFIG_DIR}/SDL2/include)
    INCLUDE_DIRECTORIES(${THIRDPARTY_DIR}/SDL2/include)
ENDIF()

IF(MENGINE_PLATFORM_SDL3)
    INCLUDE_DIRECTORIES(${THIRDPARTY_CONFIG_DIR}/SDL3/include/include-config-$<LOWER_CASE:$<CONFIG>>)
    INCLUDE_DIRECTORIES(${THIRDPARTY_CONFIG_DIR}/SDL3/include)
    INCLUDE_DIRECTORIES(${THIRDPARTY_DIR}/SDL3/include)
ENDIF()

ADD_MENGINE_LIBRARY(Systems)

if(MENGINE_PLATFORM_SDL2)
    TARGET_LINK_LIBRARIES(${PROJECT_NAME} SDL2)
endif()

if(MENGINE_PLATFORM_SDL3)
    TARGET_LINK_LIBRARIES(${PROJECT_NAME} SDL3)
endif()

TARGET_LINK_LIBRARIES(${PROJECT_NAME} d3d11 dxgi)