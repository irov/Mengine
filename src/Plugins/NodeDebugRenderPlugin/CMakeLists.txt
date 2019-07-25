set(MY_LIB_NAME NodeDebugRenderPlugin)

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

INCLUDE_DIRECTORIES(${THIRDPARTY_DIR}/stdex/include)
INCLUDE_DIRECTORIES(${THIRDPARTY_DIR}/libmath/include)

ADD_MENGINE_LIBRARY(Plugins)

TARGET_LINK_LIBRARIES(${MY_LIB_NAME} Kernel)
TARGET_LINK_LIBRARIES(${MY_LIB_NAME} Engine)