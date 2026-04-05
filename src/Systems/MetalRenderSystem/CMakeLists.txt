MENGINE_PROJECT(MetalRenderSystem)

ADD_FILTER(
src
    MetalRenderSystem.mm
    MetalRenderSystem.h
    MetalRenderEnum.mm
    MetalRenderEnum.h
    MetalRenderResourceHandler.cpp
    MetalRenderResourceHandler.h
    MetalRenderFragmentShader.mm
    MetalRenderFragmentShader.h
    MetalRenderIndexBuffer.mm
    MetalRenderIndexBuffer.h
    MetalRenderProgram.mm
    MetalRenderProgram.h
    MetalRenderProgramVariable.mm
    MetalRenderProgramVariable.h
    MetalRenderVertexAttribute.cpp
    MetalRenderVertexAttribute.h
    MetalRenderVertexBuffer.mm
    MetalRenderVertexBuffer.h
    MetalRenderVertexShader.mm
    MetalRenderVertexShader.h
    MetalRenderImage.mm
    MetalRenderImage.h
    MetalRenderImageTarget.mm
    MetalRenderImageTarget.h
    MetalRenderImageLocked.cpp
    MetalRenderImageLocked.h
    MetalRenderImageLockedFactoryStorage.h
    MetalRenderTargetTexture.mm
    MetalRenderTargetTexture.h
)

ADD_MENGINE_LIBRARY(Systems)

IF(MENGINE_TARGET_MACOS OR MENGINE_TARGET_IOS OR MENGINE_TARGET_IOS_SIMULATOR)
    find_library(METAL_FRAMEWORK Metal)
    find_library(METALKIT_FRAMEWORK MetalKit)
    find_library(QUARTZCORE_FRAMEWORK QuartzCore)

    TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${METAL_FRAMEWORK})
    TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${QUARTZCORE_FRAMEWORK})

    if(METALKIT_FRAMEWORK)
        TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${METALKIT_FRAMEWORK})
    endif()
ENDIF()
