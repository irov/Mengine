set( MY_LIB_NAME NodeDebugRenderPlugin )

ADD_FILTER(
src
    NodeDebugRenderInterface.h
	NodeDebugRenderServiceInterface.h

	NodeDebugRenderPlugin.h
    NodeDebugRenderPlugin.def
	NodeDebugRenderPlugin.cpp
    
    NodeDebugRenderService.h
    NodeDebugRenderService.cpp

    BaseDebugRender.h
    NodeDebugRenderHelper.cpp
    NodeDebugRenderHelper.h
    ArrowDebugRender.cpp
    ArrowDebugRender.h
    PointDebugRender.cpp
    PointDebugRender.h
    TextFieldDebugRender.cpp
    TextFieldDebugRender.h
    HotSpotImageDebugRender.cpp
    HotSpotImageDebugRender.h
    HotSpotPolygonDebugRender.cpp
    HotSpotPolygonDebugRender.h
    HotSpotCircleDebugRender.cpp
    HotSpotCircleDebugRender.h
)

INCLUDE_DIRECTORIES( ${THIRDPARTY_DIR}/stdex/include )
INCLUDE_DIRECTORIES( ${THIRDPARTY_DIR}/libmath/include )
INCLUDE_DIRECTORIES( ${THIRDPARTY_DIR}/pybind/include )

ADD_MENGINE_LIBRARY()

set_target_properties (${MY_LIB_NAME} PROPERTIES
    FOLDER Plugins
)

SET( APPLICATION_DEPENDENCIES
 ${APPLICATION_DEPENDENCIES}
 ${MY_LIB_NAME}
 Engine
)

SET( APPLICATION_DEPENDENCIES
 ${APPLICATION_DEPENDENCIES}
 PARENT_SCOPE
)

SET( APPLICATION_LIBS
    ${APPLICATION_LIBS}
    Kernel
    Engine
    )

SET( APPLICATION_LIBS
 ${APPLICATION_LIBS}
 PARENT_SCOPE
)