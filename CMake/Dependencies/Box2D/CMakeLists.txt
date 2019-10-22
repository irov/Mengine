CMAKE_MINIMUM_REQUIRED(VERSION 3.0)
                                             
project(Box2D)

SET(SOURCE_DIR ${THIRDPARTY_DIR}/Box2D)

# OUR SOURCES
set( SOURCES
    ${SOURCE_DIR}/Box2D/Collision/Shapes/b2ChainShape.cpp
    ${SOURCE_DIR}/Box2D/Collision/Shapes/b2CircleShape.cpp
    ${SOURCE_DIR}/Box2D/Collision/Shapes/b2EdgeShape.cpp
    ${SOURCE_DIR}/Box2D/Collision/Shapes/b2PolygonShape.cpp
    ${SOURCE_DIR}/Box2D/Collision/b2BroadPhase.cpp
    ${SOURCE_DIR}/Box2D/Collision/b2CollideCircle.cpp
    ${SOURCE_DIR}/Box2D/Collision/b2CollideEdge.cpp
    ${SOURCE_DIR}/Box2D/Collision/b2CollidePolygon.cpp
    ${SOURCE_DIR}/Box2D/Collision/b2Collision.cpp
    ${SOURCE_DIR}/Box2D/Collision/b2Distance.cpp
    ${SOURCE_DIR}/Box2D/Collision/b2DynamicTree.cpp
    ${SOURCE_DIR}/Box2D/Collision/b2TimeOfImpact.cpp
    ${SOURCE_DIR}/Box2D/Common/b2BlockAllocator.cpp
    ${SOURCE_DIR}/Box2D/Common/b2Draw.cpp
    ${SOURCE_DIR}/Box2D/Common/b2Math.cpp
    ${SOURCE_DIR}/Box2D/Common/b2Settings.cpp
    ${SOURCE_DIR}/Box2D/Common/b2StackAllocator.cpp
    ${SOURCE_DIR}/Box2D/Common/b2Timer.cpp
    ${SOURCE_DIR}/Box2D/Dynamics/Contacts/b2ChainAndCircleContact.cpp
    ${SOURCE_DIR}/Box2D/Dynamics/Contacts/b2ChainAndPolygonContact.cpp
    ${SOURCE_DIR}/Box2D/Dynamics/Contacts/b2CircleContact.cpp
    ${SOURCE_DIR}/Box2D/Dynamics/Contacts/b2Contact.cpp
    ${SOURCE_DIR}/Box2D/Dynamics/Contacts/b2ContactSolver.cpp
    ${SOURCE_DIR}/Box2D/Dynamics/Contacts/b2EdgeAndCircleContact.cpp
    ${SOURCE_DIR}/Box2D/Dynamics/Contacts/b2EdgeAndPolygonContact.cpp
    ${SOURCE_DIR}/Box2D/Dynamics/Contacts/b2PolygonAndCircleContact.cpp
    ${SOURCE_DIR}/Box2D/Dynamics/Contacts/b2PolygonContact.cpp
    ${SOURCE_DIR}/Box2D/Dynamics/Joints/b2DistanceJoint.cpp
    ${SOURCE_DIR}/Box2D/Dynamics/Joints/b2FrictionJoint.cpp
    ${SOURCE_DIR}/Box2D/Dynamics/Joints/b2GearJoint.cpp
    ${SOURCE_DIR}/Box2D/Dynamics/Joints/b2Joint.cpp
    ${SOURCE_DIR}/Box2D/Dynamics/Joints/b2MotorJoint.cpp
    ${SOURCE_DIR}/Box2D/Dynamics/Joints/b2MouseJoint.cpp
    ${SOURCE_DIR}/Box2D/Dynamics/Joints/b2GearJoint.cpp
    ${SOURCE_DIR}/Box2D/Dynamics/Joints/b2PrismaticJoint.cpp
    ${SOURCE_DIR}/Box2D/Dynamics/Joints/b2PulleyJoint.cpp
    ${SOURCE_DIR}/Box2D/Dynamics/Joints/b2RevoluteJoint.cpp
    ${SOURCE_DIR}/Box2D/Dynamics/Joints/b2RopeJoint.cpp
    ${SOURCE_DIR}/Box2D/Dynamics/Joints/b2WeldJoint.cpp
    ${SOURCE_DIR}/Box2D/Dynamics/Joints/b2WheelJoint.cpp  
    ${SOURCE_DIR}/Box2D/Dynamics/b2Body.cpp
    ${SOURCE_DIR}/Box2D/Dynamics/b2ContactManager.cpp
    ${SOURCE_DIR}/Box2D/Dynamics/b2Fixture.cpp
    ${SOURCE_DIR}/Box2D/Dynamics/b2Island.cpp
    ${SOURCE_DIR}/Box2D/Dynamics/b2World.cpp
    ${SOURCE_DIR}/Box2D/Dynamics/b2WorldCallbacks.cpp
    ${SOURCE_DIR}/Box2D/Rope/b2Rope.cpp
)

include_directories(${SOURCE_DIR})

add_library(${PROJECT_NAME} STATIC ${SOURCES})