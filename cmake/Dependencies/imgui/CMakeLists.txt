project(imgui)

SET(SOURCE_DIR ${THIRDPARTY_DIR}/imgui)

SET(LIB_TYPE STATIC)

add_library(${PROJECT_NAME} ${LIB_TYPE} ${SOURCE_DIR}/imgui.cpp ${SOURCE_DIR}/imgui_draw.cpp ${SOURCE_DIR}/imgui_widgets.cpp)