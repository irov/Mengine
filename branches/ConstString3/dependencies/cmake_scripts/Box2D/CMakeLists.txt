
CMAKE_MINIMUM_REQUIRED( VERSION 2.6 )
                                             
project(box2d)

set( CMAKE_CURRENT_SOURCE_DIR ${CMAKE_SOURCE_DIR}/Box2D/Box2D )
include_directories( ${CMAKE_SOURCE_DIR}/Box2D )
INCLUDE( ../menge_macro.cmake )

MENGE_SET_PROJECT_DEFAULTS( box2d )

# OUR SOURCES
set(BOX2D_Collision_SRCS
	${CMAKE_CURRENT_SOURCE_DIR}/Collision/b2BroadPhase.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/Collision/b2CollideCircle.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/Collision/b2CollideEdge.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/Collision/b2CollidePolygon.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/Collision/b2Collision.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/Collision/b2Distance.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/Collision/b2DynamicTree.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/Collision/b2TimeOfImpact.cpp
)
set(BOX2D_Collision_HDRS
	${CMAKE_CURRENT_SOURCE_DIR}/Collision/b2BroadPhase.h
	${CMAKE_CURRENT_SOURCE_DIR}/Collision/b2Collision.h
	${CMAKE_CURRENT_SOURCE_DIR}/Collision/b2Distance.h
	${CMAKE_CURRENT_SOURCE_DIR}/Collision/b2DynamicTree.h
	${CMAKE_CURRENT_SOURCE_DIR}/Collision/b2TimeOfImpact.h
)
set(BOX2D_Shapes_SRCS
	${CMAKE_CURRENT_SOURCE_DIR}/Collision/Shapes/b2CircleShape.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/Collision/Shapes/b2EdgeShape.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/Collision/Shapes/b2ChainShape.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/Collision/Shapes/b2PolygonShape.cpp
)
set(BOX2D_Shapes_HDRS
	${CMAKE_CURRENT_SOURCE_DIR}/Collision/Shapes/b2CircleShape.h
	${CMAKE_CURRENT_SOURCE_DIR}/Collision/Shapes/b2EdgeShape.h
	${CMAKE_CURRENT_SOURCE_DIR}/Collision/Shapes/b2ChainShape.h
	${CMAKE_CURRENT_SOURCE_DIR}/Collision/Shapes/b2PolygonShape.h
	${CMAKE_CURRENT_SOURCE_DIR}/Collision/Shapes/b2Shape.h
)
set(BOX2D_Common_SRCS
	${CMAKE_CURRENT_SOURCE_DIR}/Common/b2BlockAllocator.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/Common/b2Draw.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/Common/b2Math.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/Common/b2Settings.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/Common/b2StackAllocator.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/Common/b2Timer.cpp
)
set(BOX2D_Common_HDRS
	${CMAKE_CURRENT_SOURCE_DIR}/Common/b2BlockAllocator.h
	${CMAKE_CURRENT_SOURCE_DIR}/Common/b2Draw.h
	${CMAKE_CURRENT_SOURCE_DIR}/Common/b2GrowableStack.h
	${CMAKE_CURRENT_SOURCE_DIR}/Common/b2Math.h
	${CMAKE_CURRENT_SOURCE_DIR}/Common/b2Settings.h
	${CMAKE_CURRENT_SOURCE_DIR}/Common/b2StackAllocator.h
	${CMAKE_CURRENT_SOURCE_DIR}/Common/b2Timer.h
)
set(BOX2D_Dynamics_SRCS
	${CMAKE_CURRENT_SOURCE_DIR}/Dynamics/b2Body.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/Dynamics/b2ContactManager.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/Dynamics/b2Fixture.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/Dynamics/b2Island.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/Dynamics/b2World.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/Dynamics/b2WorldCallbacks.cpp
)
set(BOX2D_Dynamics_HDRS
	${CMAKE_CURRENT_SOURCE_DIR}/Dynamics/b2Body.h
	${CMAKE_CURRENT_SOURCE_DIR}/Dynamics/b2ContactManager.h
	${CMAKE_CURRENT_SOURCE_DIR}/Dynamics/b2Fixture.h
	${CMAKE_CURRENT_SOURCE_DIR}/Dynamics/b2Island.h
	${CMAKE_CURRENT_SOURCE_DIR}/Dynamics/b2TimeStep.h
	${CMAKE_CURRENT_SOURCE_DIR}/Dynamics/b2World.h
	${CMAKE_CURRENT_SOURCE_DIR}/Dynamics/b2WorldCallbacks.h
)
set(BOX2D_Contacts_SRCS
	${CMAKE_CURRENT_SOURCE_DIR}/Dynamics/Contacts/b2CircleContact.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/Dynamics/Contacts/b2Contact.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/Dynamics/Contacts/b2ContactSolver.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/Dynamics/Contacts/b2PolygonAndCircleContact.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/Dynamics/Contacts/b2EdgeAndCircleContact.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/Dynamics/Contacts/b2EdgeAndPolygonContact.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/Dynamics/Contacts/b2ChainAndCircleContact.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/Dynamics/Contacts/b2ChainAndPolygonContact.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/Dynamics/Contacts/b2PolygonContact.cpp
)
set(BOX2D_Contacts_HDRS
	${CMAKE_CURRENT_SOURCE_DIR}/Dynamics/Contacts/b2CircleContact.h
	${CMAKE_CURRENT_SOURCE_DIR}/Dynamics/Contacts/b2Contact.h
	${CMAKE_CURRENT_SOURCE_DIR}/Dynamics/Contacts/b2ContactSolver.h
	${CMAKE_CURRENT_SOURCE_DIR}/Dynamics/Contacts/b2PolygonAndCircleContact.h
	${CMAKE_CURRENT_SOURCE_DIR}/Dynamics/Contacts/b2EdgeAndCircleContact.h
	${CMAKE_CURRENT_SOURCE_DIR}/Dynamics/Contacts/b2EdgeAndPolygonContact.h
	${CMAKE_CURRENT_SOURCE_DIR}/Dynamics/Contacts/b2ChainAndCircleContact.h
	${CMAKE_CURRENT_SOURCE_DIR}/Dynamics/Contacts/b2ChainAndPolygonContact.h
	${CMAKE_CURRENT_SOURCE_DIR}/Dynamics/Contacts/b2PolygonContact.h
)
set(BOX2D_Joints_SRCS
	${CMAKE_CURRENT_SOURCE_DIR}/Dynamics/Joints/b2DistanceJoint.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/Dynamics/Joints/b2FrictionJoint.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/Dynamics/Joints/b2GearJoint.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/Dynamics/Joints/b2Joint.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/Dynamics/Joints/b2MouseJoint.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/Dynamics/Joints/b2PrismaticJoint.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/Dynamics/Joints/b2PulleyJoint.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/Dynamics/Joints/b2RevoluteJoint.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/Dynamics/Joints/b2RopeJoint.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/Dynamics/Joints/b2WeldJoint.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/Dynamics/Joints/b2WheelJoint.cpp
)
set(BOX2D_Joints_HDRS
	${CMAKE_CURRENT_SOURCE_DIR}/Dynamics/Joints/b2DistanceJoint.h
	${CMAKE_CURRENT_SOURCE_DIR}/Dynamics/Joints/b2FrictionJoint.h
	${CMAKE_CURRENT_SOURCE_DIR}/Dynamics/Joints/b2GearJoint.h
	${CMAKE_CURRENT_SOURCE_DIR}/Dynamics/Joints/b2Joint.h
	${CMAKE_CURRENT_SOURCE_DIR}/Dynamics/Joints/b2MouseJoint.h
	${CMAKE_CURRENT_SOURCE_DIR}/Dynamics/Joints/b2PrismaticJoint.h
	${CMAKE_CURRENT_SOURCE_DIR}/Dynamics/Joints/b2PulleyJoint.h
	${CMAKE_CURRENT_SOURCE_DIR}/Dynamics/Joints/b2RevoluteJoint.h
	${CMAKE_CURRENT_SOURCE_DIR}/Dynamics/Joints/b2RopeJoint.h
	${CMAKE_CURRENT_SOURCE_DIR}/Dynamics/Joints/b2WeldJoint.h
	${CMAKE_CURRENT_SOURCE_DIR}/Dynamics/Joints/b2WheelJoint.h
)
set(BOX2D_Rope_SRCS
	${CMAKE_CURRENT_SOURCE_DIR}/Rope/b2Rope.cpp
)
set(BOX2D_Rope_HDRS
	${CMAKE_CURRENT_SOURCE_DIR}/Rope/b2Rope.h
)
set(BOX2D_General_HDRS
	${CMAKE_CURRENT_SOURCE_DIR}/Box2D.h
)
set( box2d_sources
	${BOX2D_General_HDRS}
    ${BOX2D_Joints_SRCS}
    ${BOX2D_Joints_HDRS}
    ${BOX2D_Contacts_SRCS}
    ${BOX2D_Contacts_HDRS}
    ${BOX2D_Dynamics_SRCS}
    ${BOX2D_Dynamics_HDRS}
    ${BOX2D_Common_SRCS}
    ${BOX2D_Common_HDRS}
    ${BOX2D_Shapes_SRCS}
    ${BOX2D_Shapes_HDRS}
    ${BOX2D_Collision_SRCS}
    ${BOX2D_Collision_HDRS}
    ${BOX2D_Rope_SRCS}
    ${BOX2D_Rope_HDRS}
)




include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

add_library( ${box2d_LIB_NAME} STATIC ${box2d_sources} )

