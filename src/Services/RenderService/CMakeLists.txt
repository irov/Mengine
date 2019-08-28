SET(MY_LIB_NAME RenderService)

ADD_FILTER(
src
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
)

add_definitions(-DMETABUF_EXTERNAL_CONFIG)

INCLUDE_DIRECTORIES(${THIRDPARTY_DIR}/metabuf/include)

ADD_MENGINE_LIBRARY(Services)