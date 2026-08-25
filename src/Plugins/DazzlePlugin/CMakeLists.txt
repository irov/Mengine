MENGINE_PROJECT(DazzlePlugin)

ADD_FILTER(
src
    DazzleInterface.h
    
    DazzleIncluder.h

    DazzlePlugin.cpp
    DazzlePlugin.h
    DazzlePlugin.def

    DazzleEffect.cpp
    DazzleEffect.h
    DazzleEffectPrototypeGenerator.cpp
    DazzleEffectPrototypeGenerator.h
    ResourceDazzleEffect.cpp
    ResourceDazzleEffect.h
    ResourceDazzleEffectCustom.cpp
    ResourceDazzleEffectCustom.h
    DataflowDZZ.cpp
    DataflowDZZ.h
    DataflowDazzleJSON.cpp
    DataflowDazzleJSON.h
    
    DazzleDataInterface.h
    
    DazzleData.cpp
    DazzleData.h
)

ADD_FILTER(
evict
    ${THIRDPARTY_DIR}/dazzle/tools/evict/src/evict.cpp
    ${THIRDPARTY_DIR}/dazzle/tools/evict/include/evict/evict.hpp
)

if(MENGINE_BUILD_MENGINE_SCRIPT_EMBEDDED)
    ADD_FILTER(
    embedding
        DazzleScriptEmbedding.cpp
        DazzleScriptEmbedding.h
    )

    INCLUDE_DIRECTORIES(${THIRDPARTY_DIR}/pybind/include)
endif()

INCLUDE_DIRECTORIES(${THIRDPARTY_DIR}/dazzle/include)
INCLUDE_DIRECTORIES(${THIRDPARTY_DIR}/dazzle/tools/evict/include)
INCLUDE_DIRECTORIES(${THIRDPARTY_DIR}/jpp/include)
INCLUDE_DIRECTORIES(${THIRDPARTY_CONFIG_DIR}/jansson/include)

ADD_MENGINE_PLUGIN(MENGINE_PLUGIN_DAZZLE)

TARGET_LINK_LIBRARIES(${PROJECT_NAME} Kernel)

if(MENGINE_BUILD_MENGINE_SCRIPT_EMBEDDED)
    TARGET_LINK_LIBRARIES(${PROJECT_NAME} Python)
endif()

TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${THIRDPARTY_LIB_DIR}/${MENGINE_LIB_PREFIX}dazzle_aux${MENGINE_LIB_SUFFIX})
TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${THIRDPARTY_LIB_DIR}/${MENGINE_LIB_PREFIX}dazzle_read${MENGINE_LIB_SUFFIX})
TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${THIRDPARTY_LIB_DIR}/${MENGINE_LIB_PREFIX}dazzle${MENGINE_LIB_SUFFIX})
TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${THIRDPARTY_LIB_DIR}/${MENGINE_LIB_PREFIX}jpp${MENGINE_LIB_SUFFIX})
TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${THIRDPARTY_LIB_DIR}/${MENGINE_LIB_PREFIX}jansson${MENGINE_LIB_SUFFIX})
