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

if(MENGINE_BUILD_MENGINE_SCRIPT_EMBEDDED)
    ADD_FILTER(
    embedding
        Box2DScriptEmbedding.h
        Box2DScriptEmbedding.cpp
    )

    INCLUDE_DIRECTORIES(${THIRDPARTY_DIR}/pybind/include)
endif()

INCLUDE_DIRECTORIES(${THIRDPARTY_DIR}/box2d/include)

ADD_MENGINE_PLUGIN(MENGINE_PLUGIN_BOX2D)

TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${THIRDPARTY_LIB_DIR}/${MENGINE_LIB_PREFIX}box2d${MENGINE_DEPENDENCIES_DEBUG_SUFFIX}${MENGINE_LIB_SUFFIX})
TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${THIRDPARTY_LIB_DIR}/${MENGINE_LIB_PREFIX}math${MENGINE_LIB_SUFFIX})
TARGET_LINK_LIBRARIES(${PROJECT_NAME} Kernel)

if(MENGINE_BUILD_MENGINE_SCRIPT_EMBEDDED)
    TARGET_LINK_LIBRARIES(${PROJECT_NAME} Python)
endif()
