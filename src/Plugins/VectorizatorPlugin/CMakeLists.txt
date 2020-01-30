set(MY_LIB_NAME VectorizatorPlugin)

ADD_FILTER(
src
    VectorizatorInterface.h

    Vectorizator.h
    Vectorizator.cpp
    
    VectorizatorPlugin.h
    VectorizatorPlugin.cpp
    VectorizatorPlugin.def
)

if(MENGINE_USE_SCRIPT_SERVICE)
    ADD_FILTER(
    embedding
        VectorizatorScriptEmbedding.h
        VectorizatorScriptEmbedding.cpp
        )
        
    INCLUDE_DIRECTORIES(${THIRDPARTY_DIR}/pybind/include)
endif()

INCLUDE_DIRECTORIES(${THIRDPARTY_DIR}/graphics/include)

ADD_MENGINE_PLUGIN(MENGINE_PLUGIN_VECTORIZATOR)

if(MENGINE_USE_SCRIPT_SERVICE)
    TARGET_LINK_LIBRARIES(${MY_LIB_NAME} ${THIRDPARTY_LIB_DIR}/${MENGINE_LIB_PREFIX}pybind${MENGINE_LIB_SUFFIX})
endif()

TARGET_LINK_LIBRARIES(${MY_LIB_NAME} Kernel)

TARGET_LINK_LIBRARIES(${MY_LIB_NAME} ${THIRDPARTY_LIB_DIR}/${MENGINE_LIB_PREFIX}stdex${MENGINE_LIB_SUFFIX})
TARGET_LINK_LIBRARIES(${MY_LIB_NAME} ${THIRDPARTY_LIB_DIR}/${MENGINE_LIB_PREFIX}graphics${MENGINE_LIB_SUFFIX})
