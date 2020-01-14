set(MY_LIB_NAME VectorizatorFramework)

ADD_FILTER(
VectorizatorFramework
    VectorizatorFramework.cpp
    VectorizatorFramework.h
    VectorizatorFrameworkPlugin.cpp
    VectorizatorFrameworkPlugin.h
    VectorizatorSceneEventReceiver.cpp
    VectorizatorSceneEventReceiver.h
)

INCLUDE_DIRECTORIES(${THIRDPARTY_DIR}/GOAP/include)
INCLUDE_DIRECTORIES(${MENGINE_CMAKE_DEPENDENCIES_DIR}/GOAP/Config)

ADD_MENGINE_PLUGIN(MENGINE_PLUGIN_VECTARIZATORFRAMEWORK)

set_target_properties(${MY_LIB_NAME} PROPERTIES
    FOLDER Frameworks
)

TARGET_LINK_LIBRARIES(${MY_LIB_NAME} Kernel)
TARGET_LINK_LIBRARIES(${MY_LIB_NAME} Engine)

TARGET_LINK_LIBRARIES(${MY_LIB_NAME} ${THIRDPARTY_LIB_DIR}/${MENGINE_LIB_PREFIX}stdex${MENGINE_LIB_SUFFIX})
TARGET_LINK_LIBRARIES(${MY_LIB_NAME} ${THIRDPARTY_LIB_DIR}/${MENGINE_LIB_PREFIX}math${MENGINE_LIB_SUFFIX})