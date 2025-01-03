MENGINE_PROJECT(GraphicsPlugin)

ADD_FILTER(
src
    GraphicsInterface.h

    Graphics.h
    Graphics.cpp
    
    GraphicsPlugin.h
    GraphicsPlugin.cpp
    GraphicsPlugin.def
)

if(MENGINE_USE_SCRIPT_SERVICE)
    ADD_FILTER(
    embedding
        GraphicsScriptEmbedding.h
        GraphicsScriptEmbedding.cpp
        )
        
    INCLUDE_DIRECTORIES(${THIRDPARTY_DIR}/pybind/include)
endif()

INCLUDE_DIRECTORIES(${THIRDPARTY_CONFIG_DIR}/graphics)
INCLUDE_DIRECTORIES(${THIRDPARTY_DIR}/graphics/include)

ADD_MENGINE_PLUGIN(MENGINE_PLUGIN_GRAPHICS)

if(MENGINE_USE_SCRIPT_SERVICE)
    TARGET_LINK_LIBRARIES(${PROJECT_NAME} Python)
endif()

TARGET_LINK_LIBRARIES(${PROJECT_NAME} Kernel)

TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${THIRDPARTY_LIB_DIR}/${MENGINE_LIB_PREFIX}stdex${MENGINE_LIB_SUFFIX})
TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${THIRDPARTY_LIB_DIR}/${MENGINE_LIB_PREFIX}graphics${MENGINE_LIB_SUFFIX})
