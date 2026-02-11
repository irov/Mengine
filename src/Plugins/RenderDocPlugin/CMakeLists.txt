MENGINE_PROJECT(RenderDocPlugin)

ADD_FILTER(
src
    RenderDocPlugin.cpp
    RenderDocPlugin.h
    RenderDocPlugin.def

	RenderDocInterface.h

    RenderDocService.cpp
    RenderDocService.h
)

include_directories(${THIRDPARTY_DIR}/renderdoc)

ADD_MENGINE_PLUGIN(MENGINE_PLUGIN_RENDERDOC)

TARGET_LINK_LIBRARIES(${PROJECT_NAME} Kernel)
TARGET_LINK_LIBRARIES(${PROJECT_NAME} Windows)