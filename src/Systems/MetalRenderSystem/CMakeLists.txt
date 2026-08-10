MENGINE_PROJECT(MetalRenderSystem)

ADD_FILTER(
src
    MetalRenderSystem.mm
    MetalRenderSystem.h
    MetalRenderEnum.mm
    MetalRenderEnum.h
    MetalRenderResourceHandler.mm
    MetalRenderResourceHandler.h
    MetalRenderFragmentShader.mm
    MetalRenderFragmentShader.h
    MetalRenderIndexBuffer.mm
    MetalRenderIndexBuffer.h
    MetalRenderProgram.mm
    MetalRenderProgram.h
    MetalRenderProgramVariableInterface.h
    MetalRenderProgramVariableStatic.mm
    MetalRenderProgramVariableStatic.h
    MetalRenderProgramVariableDynamic.mm
    MetalRenderProgramVariableDynamic.h
    MetalRenderVertexAttribute.mm
    MetalRenderVertexAttribute.h
    MetalRenderVertexBuffer.mm
    MetalRenderVertexBuffer.h
    MetalRenderVertexShader.mm
    MetalRenderVertexShader.h
    MetalRenderImage.mm
    MetalRenderImage.h
    MetalRenderImageTarget.mm
    MetalRenderImageTarget.h
    MetalRenderImageLocked.mm
    MetalRenderImageLocked.h
    MetalRenderImageLockedFactoryStorage.h
    MetalRenderTargetTexture.mm
    MetalRenderTargetTexture.h
)

ADD_MENGINE_LIBRARY(Systems)

IF(MENGINE_TARGET_MACOS OR MENGINE_TARGET_IOS OR MENGINE_TARGET_IOS_SIMULATOR)
    TARGET_LINK_LIBRARIES(${PROJECT_NAME}
        "-framework Metal"
        "-framework MetalKit"
        "-framework QuartzCore"
        )
ENDIF()
