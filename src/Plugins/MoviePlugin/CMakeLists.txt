MENGINE_PROJECT(MoviePlugin)

ADD_FILTER(
src
    Movie2Interface.h

    MoviePlugin.h
    MoviePlugin.cpp
    MoviePlugin.def

    Movie2.cpp
    Movie2.h
    Movie2Slot.cpp
    Movie2Slot.h
    Movie2Scissor.cpp
    Movie2Scissor.h
    Movie2SubComposition.cpp
    Movie2SubComposition.h
    Movie2DataInterface.h

    DataflowAEZ.cpp
    DataflowAEZ.h
    Movie2Data.cpp
    Movie2Data.h
    ResourceMovie2.cpp
    ResourceMovie2.h

    Movie2DebugRender.cpp
    Movie2DebugRender.h
    ResourceMovie2Validator.cpp
    ResourceMovie2Validator.h
    Movie2DebuggerBoundingBox.h
    Movie2DebuggerBoundingBox.cpp
)

if(MENGINE_PLUGIN_METABUF)
    ADD_FILTER(
    Metabuf
        MetabufLoaderResourceMovie2.cpp
        MetabufLoaderResourceMovie2.h
    )

    add_definitions(-DMETABUF_EXTERNAL_CONFIG)

    INCLUDE_DIRECTORIES(${THIRDPARTY_DIR}/metabuf/include)
endif()

if(MENGINE_PLUGIN_JSON)
    ADD_FILTER(
    JSONLoader
        JSONLoaderResourceMovie2.cpp
        JSONLoaderResourceMovie2.h
    )

    INCLUDE_DIRECTORIES(${THIRDPARTY_DIR}/jpp/include)
endif()

if(MENGINE_BUILD_MENGINE_SCRIPT_EMBEDDED)
    ADD_FILTER(
    embedding
        Movie2ScriptEmbedding.h
        Movie2ScriptEmbedding.cpp
        )

    INCLUDE_DIRECTORIES(${THIRDPARTY_DIR}/pybind/include)
endif()

INCLUDE_DIRECTORIES(${THIRDPARTY_DIR}/libmovie/include)

ADD_MENGINE_PLUGIN(MENGINE_PLUGIN_MOVIE)

if(MENGINE_BUILD_MENGINE_SCRIPT_EMBEDDED)
    TARGET_LINK_LIBRARIES(${PROJECT_NAME} Python)
endif()

TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${THIRDPARTY_LIB_DIR}/${MENGINE_LIB_PREFIX}math${MENGINE_LIB_SUFFIX})
TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${THIRDPARTY_LIB_DIR}/${MENGINE_LIB_PREFIX}movie${MENGINE_LIB_SUFFIX})

TARGET_LINK_LIBRARIES(${PROJECT_NAME} Kernel)
TARGET_LINK_LIBRARIES(${PROJECT_NAME} Engine)

if(MENGINE_PLUGIN_METABUF)
    TARGET_LINK_LIBRARIES(${PROJECT_NAME} Metacode)
    TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${THIRDPARTY_LIB_DIR}/${MENGINE_LIB_PREFIX}metabuf${MENGINE_LIB_SUFFIX})
endif()
