MENGINE_PROJECT(TexturepackerPlugin)

ADD_FILTER(
src
    TexturepackerInterface.h
    
    TexturepackerPlugin.h
    TexturepackerPlugin.cpp
    TexturepackerPlugin.def
    
    ResourceTexturepacker.cpp
    ResourceTexturepacker.h
    ResourceMultiTexturepacker.cpp
    ResourceMultiTexturepacker.h

    LoaderResourceTexturepacker.cpp
    LoaderResourceTexturepacker.h
)

if(MENGINE_USE_SCRIPT_SERVICE)
    ADD_FILTER(
    embedding    
        TexturepackerScriptEmbedding.cpp
        TexturepackerScriptEmbedding.h
        )
        
    INCLUDE_DIRECTORIES(${THIRDPARTY_DIR}/pybind/include)    
endif()

add_definitions(-DMETABUF_EXTERNAL_CONFIG)

INCLUDE_DIRECTORIES(${THIRDPARTY_DIR}/jansson/include)
INCLUDE_DIRECTORIES(${THIRDPARTY_DIR}/jpp/include)
INCLUDE_DIRECTORIES(${THIRDPARTY_DIR}/metabuf/include)

ADD_MENGINE_PLUGIN(MENGINE_PLUGIN_TEXTUREPACKER)

if(MENGINE_USE_SCRIPT_SERVICE)
    TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${THIRDPARTY_LIB_DIR}/${MENGINE_LIB_PREFIX}pybind${MENGINE_LIB_SUFFIX})
endif()

TARGET_LINK_LIBRARIES(${PROJECT_NAME} Kernel)
TARGET_LINK_LIBRARIES(${PROJECT_NAME} Metacode)

TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${THIRDPARTY_LIB_DIR}/${MENGINE_LIB_PREFIX}stdex${MENGINE_LIB_SUFFIX})
TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${THIRDPARTY_LIB_DIR}/${MENGINE_LIB_PREFIX}math${MENGINE_LIB_SUFFIX})
TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${THIRDPARTY_LIB_DIR}/${MENGINE_LIB_PREFIX}metabuf${MENGINE_LIB_SUFFIX})