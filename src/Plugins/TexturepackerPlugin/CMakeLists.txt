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
)

if(MENGINE_PLUGIN_METABUF)
    ADD_FILTER(
    Metabuf
        MetabufLoaderResourceTexturepacker.cpp
        MetabufLoaderResourceTexturepacker.h
    )

    add_definitions(-DMETABUF_EXTERNAL_CONFIG)

    INCLUDE_DIRECTORIES(${THIRDPARTY_DIR}/metabuf/include)
endif()

if(MENGINE_PLUGIN_JSON)
    ADD_FILTER(
    JSONLoader
        JSONLoaderResourceTexturepacker.cpp
        JSONLoaderResourceTexturepacker.h
    )

    INCLUDE_DIRECTORIES(${THIRDPARTY_DIR}/jpp/include)
endif()

if(MENGINE_BUILD_MENGINE_SCRIPT_EMBEDDED)
    ADD_FILTER(
    embedding
        TexturepackerScriptEmbedding.cpp
        TexturepackerScriptEmbedding.h
        )

    INCLUDE_DIRECTORIES(${THIRDPARTY_DIR}/pybind/include)
endif()

INCLUDE_DIRECTORIES(${THIRDPARTY_DIR}/jansson/include)
INCLUDE_DIRECTORIES(${THIRDPARTY_DIR}/jpp/include)

ADD_MENGINE_PLUGIN(MENGINE_PLUGIN_TEXTUREPACKER)

if(MENGINE_BUILD_MENGINE_SCRIPT_EMBEDDED)
    TARGET_LINK_LIBRARIES(${PROJECT_NAME} Python)
endif()

TARGET_LINK_LIBRARIES(${PROJECT_NAME} Kernel)

if(MENGINE_PLUGIN_METABUF)
    TARGET_LINK_LIBRARIES(${PROJECT_NAME} Metacode)
    TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${THIRDPARTY_LIB_DIR}/${MENGINE_LIB_PREFIX}metabuf${MENGINE_LIB_SUFFIX})
endif()
