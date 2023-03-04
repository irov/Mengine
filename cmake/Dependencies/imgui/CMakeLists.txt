project(imgui)

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/imconfig.h.cmakein ${THIRDPARTY_CONFIG_DIR}/imgui/imconfig.h @ONLY)

SET(SOURCE_DIR ${THIRDPARTY_DIR}/imgui)

set(SRC_FILES
    ${SOURCE_DIR}/imgui.cpp
    ${SOURCE_DIR}/imgui_draw.cpp
    ${SOURCE_DIR}/imgui_widgets.cpp
    ${SOURCE_DIR}/imgui_tables.cpp
)

SET(LIB_TYPE STATIC)

include_directories(${THIRDPARTY_CONFIG_DIR}/imgui)

add_library(${PROJECT_NAME} ${LIB_TYPE} ${SRC_FILES})