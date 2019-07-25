set(MY_LIB_NAME Box2DPlugin)

ADD_FILTER(
src
    Box2DScaler.h
    Box2DBody.cpp
    Box2DBody.h
    Box2DJoint.cpp
    Box2DJoint.h
    Box2DModule.cpp
    Box2DModule.h    
    Box2DWorld.cpp
    Box2DWorld.h
	Box2DPlugin.cpp
	Box2DPlugin.h	
    NodeBox2DBody.cpp    
    NodeBox2DBody.h
)

INCLUDE_DIRECTORIES(${THIRDPARTY_DIR}/box2d)
INCLUDE_DIRECTORIES(${THIRDPARTY_DIR}/libmath/include)

ADD_MENGINE_LIBRARY(Plugins)

TARGET_LINK_LIBRARIES(${MY_LIB_NAME} ${THIRDPARTY_LIB_DIR}/${MENGINE_LIB_PREFIX}Box2D${MENGINE_LIB_SUFFIX})
TARGET_LINK_LIBRARIES(${MY_LIB_NAME} Kernel)