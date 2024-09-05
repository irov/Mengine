MENGINE_PROJECT(NodeDebugRenderPlugin)

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
    PointDebugRender.cpp
    PointDebugRender.h
    TextFieldDebugRender.cpp
    TextFieldDebugRender.h
    HotSpotImageDebugRender.cpp
    HotSpotImageDebugRender.h
    HotSpotPolygonDebugRender.cpp
    HotSpotPolygonDebugRender.h
    HotSpotSurfaceDebugRender.cpp
    HotSpotSurfaceDebugRender.h
    HotSpotCircleDebugRender.cpp
    HotSpotCircleDebugRender.h
)

ADD_MENGINE_PLUGIN(MENGINE_PLUGIN_NODEDEBUGRENDER)

TARGET_LINK_LIBRARIES(${PROJECT_NAME} Kernel)
TARGET_LINK_LIBRARIES(${PROJECT_NAME} Engine)