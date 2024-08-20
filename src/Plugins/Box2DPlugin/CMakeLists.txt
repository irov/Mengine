MENGINE_PROJECT(Box2DPlugin)

ADD_FILTER(
src
    Box2DInterface.h
    Box2DIncluder.h

    Box2DPlugin.cpp
    Box2DPlugin.h
    Box2DService.cpp
    Box2DService.h
    Box2DScaler.h
    Box2DBody.cpp
    Box2DBody.h
    Box2DJoint.cpp
    Box2DJoint.h
    Box2DWorld.cpp
    Box2DWorld.h

    PhysicalPlaceholder.cpp
    PhysicalPlaceholder.h
)

INCLUDE_DIRECTORIES(${THIRDPARTY_DIR}/box2d/include)

ADD_MENGINE_PLUGIN(MENGINE_PLUGIN_BOX2D)

TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${THIRDPARTY_LIB_DIR}/${MENGINE_LIB_PREFIX}box2d${MENGINE_LIB_SUFFIX})
TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${THIRDPARTY_LIB_DIR}/${MENGINE_LIB_PREFIX}math${MENGINE_LIB_SUFFIX})
TARGET_LINK_LIBRARIES(${PROJECT_NAME} Kernel)