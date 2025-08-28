MENGINE_PROJECT(MetalRenderSystem)

ADD_FILTER(
    src
    MetalRenderSystem.h
    MetalRenderSystem.mm
    MetalRenderTypes.h
    MetalRenderVertexBuffer.h
    MetalRenderVertexBuffer.mm
    MetalRenderIndexBuffer.h
    MetalRenderIndexBuffer.mm
    MetalRenderVertexAttribute.h
    MetalRenderVertexAttribute.mm
    MetalRenderFragmentShader.h
    MetalRenderFragmentShader.mm
    MetalRenderVertexShader.h
    MetalRenderVertexShader.mm
    MetalRenderProgram.h
    MetalRenderProgram.mm
    MetalRenderProgramVariable.h
    MetalRenderProgramVariable.mm
    MetalRenderImage.h
    MetalRenderImage.mm
    MetalRenderImageLocked.h
    MetalRenderImageLocked.mm
    MetalRenderTargetTexture.h
    MetalRenderTargetTexture.mm
    MetalRenderMaterialStageCache.h
    MetalRenderMaterialStageCache.mm
)

ADD_MENGINE_LIBRARY(Systems)

