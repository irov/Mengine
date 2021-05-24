project(imgui)

SET(SOURCE_DIR ${THIRDPARTY_DIR}/imgui)

set(SRC_FILES
    ${SOURCE_DIR}/imgui.cpp
    ${SOURCE_DIR}/imgui_draw.cpp
    ${SOURCE_DIR}/imgui_widgets.cpp
    ${SOURCE_DIR}/imgui_tables.cpp
)

SET(LIB_TYPE STATIC)

add_definitions(-DIMGUI_DISABLE_DEMO_WINDOWS)

add_library(${PROJECT_NAME} ${LIB_TYPE} ${SRC_FILES})