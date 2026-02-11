MENGINE_PROJECT(CameraDebugGizmoPlugin)

ADD_FILTER(
src
    CameraDebugGizmoPlugin.h
    CameraDebugGizmoPlugin.def
    CameraDebugGizmoPlugin.cpp
    
    CameraDebugGizmoModule.cpp
    CameraDebugGizmoModule.h
)

ADD_MENGINE_PLUGIN(MENGINE_PLUGIN_CAMERADEBUGGIZMO)

TARGET_LINK_LIBRARIES(${PROJECT_NAME} Kernel)