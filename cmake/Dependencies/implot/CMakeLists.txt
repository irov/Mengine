project(implot)

SET(SOURCE_DIR ${THIRDPARTY_DIR}/implot)

set(SRC_FILES
    ${SOURCE_DIR}/implot.cpp
    ${SOURCE_DIR}/implot_items.cpp
)

SET(LIB_TYPE STATIC)

include_directories(${THIRDPARTY_CONFIG_DIR}/imgui)
include_directories(${THIRDPARTY_DIR}/imgui)

add_library(${PROJECT_NAME} ${LIB_TYPE} ${SRC_FILES})