SET( MY_LIB_NAME ServiceRender )

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

ADD_MENGINE_LIBRARY()

set_target_properties (${MY_LIB_NAME} PROPERTIES
    FOLDER Service
)

INCLUDE_DIRECTORIES( 
    ${THIRDPARTY_DIR}/pybind/include
    ${THIRDPARTY_DIR}/metabuf
    ${THIRDPARTY_DIR}/tinyini/include
    ${THIRDPARTY_DIR}/libmath/include
    ) 

SET( APPLICATION_DEPENDENCIES
 ${APPLICATION_DEPENDENCIES}
 ${MY_LIB_NAME}
)


SET( APPLICATION_DEPENDENCIES
 ${APPLICATION_DEPENDENCIES}
 PARENT_SCOPE
)
