set(MY_LIB_NAME MoviePlugin)

ADD_FILTER(
src
    Movie2Interface.h
    UnknownMovie2Interface.h
    
	MoviePlugin.h
	MoviePlugin.cpp
    
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
    LoaderResourceMovie2.cpp
    LoaderResourceMovie2.h
    
    Movie2DebugRender.cpp
    Movie2DebugRender.h
    ResourceMovie2Validator.cpp
    ResourceMovie2Validator.h
    Movie2DebuggerBoundingBox.h
    Movie2DebuggerBoundingBox.cpp
    
    Movie2ScriptEmbedding.h
    Movie2ScriptEmbedding.cpp
)

add_definitions(-DMETABUF_EXTERNAL_CONFIG)

INCLUDE_DIRECTORIES( ${THIRDPARTY_DIR}/stdex/include )
INCLUDE_DIRECTORIES( ${THIRDPARTY_DIR}/libmovie/include )
INCLUDE_DIRECTORIES( ${THIRDPARTY_DIR}/pybind/include )
INCLUDE_DIRECTORIES( ${THIRDPARTY_DIR}/libmath/include )
INCLUDE_DIRECTORIES( ${THIRDPARTY_DIR}/metabuf/include )

ADD_MENGINE_PLUGIN(MENGINE_PLUGIN_MOVIE)

set_target_properties (${MY_LIB_NAME} PROPERTIES
    FOLDER Plugins
)

TARGET_LINK_LIBRARIES(${MY_LIB_NAME} ${THIRDPARTY_LIB_DIR}/${MENGINE_LIB_PREFIX}stdex${MENGINE_LIB_SUFFIX})
TARGET_LINK_LIBRARIES(${MY_LIB_NAME} ${THIRDPARTY_LIB_DIR}/${MENGINE_LIB_PREFIX}math${MENGINE_LIB_SUFFIX})
TARGET_LINK_LIBRARIES(${MY_LIB_NAME} ${THIRDPARTY_LIB_DIR}/${MENGINE_LIB_PREFIX}pybind${MENGINE_LIB_SUFFIX})
TARGET_LINK_LIBRARIES(${MY_LIB_NAME} ${THIRDPARTY_LIB_DIR}/${MENGINE_LIB_PREFIX}metabuf${MENGINE_LIB_SUFFIX})
TARGET_LINK_LIBRARIES(${MY_LIB_NAME} ${THIRDPARTY_LIB_DIR}/${MENGINE_LIB_PREFIX}movie${MENGINE_LIB_SUFFIX})

TARGET_LINK_LIBRARIES(${MY_LIB_NAME} Kernel)
TARGET_LINK_LIBRARIES(${MY_LIB_NAME} Engine)	
TARGET_LINK_LIBRARIES(${MY_LIB_NAME} Metacode)