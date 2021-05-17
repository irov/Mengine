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
    DX11ErrorHelper.h
    DX11ErrorHelper.cpp
    DX11RenderBuffer.h
    DX11RenderBuffer.cpp
    DX11RenderProgramVariable.h
    DX11RenderProgramVariable.cpp
    DX11RenderResourceHandler.h
    DX11RenderResourceHandler.cpp
)

ADD_MENGINE_LIBRARY(Systems)

TARGET_LINK_LIBRARIES(${PROJECT_NAME} d3d11 dxgi)