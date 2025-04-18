MENGINE_PROJECT(JSONPlugin)

ADD_FILTER(
src
    JSONInterface.h
    
    JSONPlugin.h
    JSONPlugin.cpp
    JSONPlugin.def
    
    JSONService.h
    JSONService.cpp
    
    JSONPackageLoader.h
    JSONPackageLoader.cpp
    
    ResourceJSON.cpp
    ResourceJSON.h

    MetabufLoaderResourceJSON.cpp
    MetabufLoaderResourceJSON.h
    
    JSONSetting.cpp
    JSONSetting.h
    JSONSettingPrototypeGenerator.cpp
    JSONSettingPrototypeGenerator.h
    
    JSONConfig.cpp
    JSONConfig.h
)

if(MENGINE_BUILD_MENGINE_SCRIPT_EMBEDDED)
    ADD_FILTER(
    embedding
        JSONScriptEmbedding.cpp
        JSONScriptEmbedding.h
        )
        
    INCLUDE_DIRECTORIES(${THIRDPARTY_DIR}/pybind/include)
endif()

add_definitions(-DMETABUF_EXTERNAL_CONFIG)

INCLUDE_DIRECTORIES(${THIRDPARTY_DIR}/jansson/include)
INCLUDE_DIRECTORIES(${THIRDPARTY_DIR}/jpp/include)
INCLUDE_DIRECTORIES(${THIRDPARTY_DIR}/metabuf/include)

ADD_MENGINE_PLUGIN(MENGINE_PLUGIN_JSON)

if(MENGINE_BUILD_MENGINE_SCRIPT_EMBEDDED)
    TARGET_LINK_LIBRARIES(${PROJECT_NAME} Python)
endif()

TARGET_LINK_LIBRARIES(${PROJECT_NAME} Kernel)
TARGET_LINK_LIBRARIES(${PROJECT_NAME} Metacode)

TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${THIRDPARTY_LIB_DIR}/${MENGINE_LIB_PREFIX}stdex${MENGINE_LIB_SUFFIX})
TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${THIRDPARTY_LIB_DIR}/${MENGINE_LIB_PREFIX}metabuf${MENGINE_LIB_SUFFIX})
