MENGINE_PROJECT(Box2DFramework)

ADD_FILTER(
Box2DFramework
    Box2DFramework.cpp
    Box2DFramework.h
    Box2DFrameworkPlugin.cpp
    Box2DFrameworkPlugin.h
    Box2DEventReceiver.cpp
    Box2DEventReceiver.h
)

INCLUDE_DIRECTORIES(${MENGINE_CMAKE_DEPENDENCIES_DIR}/GOAP/Config)
INCLUDE_DIRECTORIES(${THIRDPARTY_DIR}/GOAP/include)

ADD_MENGINE_PLUGIN(MENGINE_PLUGIN_EXAMPLEFRAMEWORK Frameworks)

TARGET_LINK_LIBRARIES(${PROJECT_NAME} Kernel)
TARGET_LINK_LIBRARIES(${PROJECT_NAME} Engine)

TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${THIRDPARTY_LIB_DIR}/${MENGINE_LIB_PREFIX}stdex${MENGINE_LIB_SUFFIX})
TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${THIRDPARTY_LIB_DIR}/${MENGINE_LIB_PREFIX}math${MENGINE_LIB_SUFFIX})