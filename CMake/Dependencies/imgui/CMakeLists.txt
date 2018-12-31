CMAKE_MINIMUM_REQUIRED(VERSION 3.0)

SET( SOURCE_DIR ${THIRDPARTY_DIR}/imgui )

project(imgui)

SET (LIB_TYPE STATIC)

add_library(${PROJECT_NAME} ${LIB_TYPE} ${SOURCE_DIR}/imgui.cpp ${SOURCE_DIR}/imgui_draw.cpp ${SOURCE_DIR}/imgui_widgets.cpp)