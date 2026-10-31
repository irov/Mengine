MENGINE_PROJECT(MoviePlugin)

ADD_FILTER(
src
    Movie2Includer.h
    Movie2Interface.h
    Movie2EventReceiverInterface.h
    Movie2SubCompositionEventReceiver.h
    Movie2MarkerEventReceiverInterface.h

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

ADD_FILTER(
Metabuf
    MetabufLoaderResourceMovie2.cpp
    MetabufLoaderResourceMovie2.h
)

ADD_DEFINITIONS(-DMETABUF_EXTERNAL_ALLOCATOR)

INCLUDE_DIRECTORIES(${THIRDPARTY_DIR}/metabuf/include)

IF(MENGINE_BUILD_MENGINE_SCRIPT_EMBEDDED)
    ADD_FILTER(
    embedding
        Movie2ScriptEmbedding.h
        Movie2ScriptEmbedding.cpp

        PythonMovie2EventReceiver.h
        PythonMovie2EventReceiver.cpp
        PythonMovie2MarkerEventReceiver.h
        PythonMovie2MarkerEventReceiver.cpp
        PythonMovie2SubCompositionEventReceiver.h
        PythonMovie2SubCompositionEventReceiver.cpp
    )

    INCLUDE_DIRECTORIES(${THIRDPARTY_DIR}/pybind/include)
ENDIF()

INCLUDE_DIRECTORIES(${THIRDPARTY_DIR}/libmovie/include)

ADD_MENGINE_PLUGIN(MENGINE_PLUGIN_MOVIE)

IF(MENGINE_BUILD_MENGINE_SCRIPT_EMBEDDED)
    TARGET_LINK_LIBRARIES(${PROJECT_NAME} Python)
ENDIF()

TARGET_LINK_LIBRARIES(${PROJECT_NAME}
    ${THIRDPARTY_LIB_DIR}/${MENGINE_LIB_PREFIX}math${MENGINE_LIB_SUFFIX}
    ${THIRDPARTY_LIB_DIR}/${MENGINE_LIB_PREFIX}movie${MENGINE_LIB_SUFFIX}
    Kernel
    Engine
    Metacode
    ${THIRDPARTY_LIB_DIR}/${MENGINE_LIB_PREFIX}metabuf${MENGINE_LIB_SUFFIX}
)
