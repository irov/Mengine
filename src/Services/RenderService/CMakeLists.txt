MENGINE_PROJECT(RenderService)

ADD_FILTER(
src
    RenderBatch.h
    RenderBatch.cpp
    RenderService.h
    RenderService.cpp
    RenderTexture.h
    RenderTexture.cpp
    RenderTextureService.h
    RenderTextureService.cpp
    DecoderRenderImageProvider.h
    DecoderRenderImageProvider.cpp
    DecoderRenderImageLoader.h
    DecoderRenderImageLoader.cpp
    RenderMaterial.h
    RenderMaterial.cpp
    RenderMaterialService.h
    RenderMaterialService.cpp
    BatchRenderPipeline.h
    BatchRenderPipeline.cpp
)

ADD_MENGINE_LIBRARY(Services)