
CMAKE_MINIMUM_REQUIRED( VERSION 2.6 )
                                             
project(box2d)

set( CMAKE_CURRENT_SOURCE_DIR ${CMAKE_SOURCE_DIR}/../Box2D/Source )

INCLUDE( ../menge_macro.cmake )

MENGE_SET_PROJECT_DEFAULTS( box2d )

# OUR SOURCES
set( box2d_sources
	${CMAKE_CURRENT_SOURCE_DIR}/Dynamics/b2Body.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/Dynamics/b2Island.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/Dynamics/b2World.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/Dynamics/b2ContactManager.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/Dynamics/b2WorldCallbacks.cpp
	
	${CMAKE_CURRENT_SOURCE_DIR}/Dynamics/Contacts/b2Contact.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/Dynamics/Contacts/b2ContactSolver.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/Dynamics/Contacts/b2CircleContact.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/Dynamics/Contacts/b2PolyContact.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/Dynamics/Contacts/b2PolyAndCircleContact.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/Dynamics/Contacts/b2PolyAndEdgeContact.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/Dynamics/Contacts/b2EdgeAndCircleContact.cpp
	
	
	${CMAKE_CURRENT_SOURCE_DIR}/Dynamics/Joints/b2MouseJoint.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/Dynamics/Joints/b2PulleyJoint.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/Dynamics/Joints/b2Joint.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/Dynamics/Joints/b2RevoluteJoint.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/Dynamics/Joints/b2PrismaticJoint.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/Dynamics/Joints/b2DistanceJoint.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/Dynamics/Joints/b2GearJoint.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/Dynamics/Joints/b2LineJoint.cpp
	
	${CMAKE_CURRENT_SOURCE_DIR}/Dynamics/Controllers/b2BuoyancyController.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/Dynamics/Controllers/b2ConstantAccelController.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/Dynamics/Controllers/b2ConstantForceController.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/Dynamics/Controllers/b2Controller.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/Dynamics/Controllers/b2GravityController.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/Dynamics/Controllers/b2TensorDampingController.cpp	
	
	${CMAKE_CURRENT_SOURCE_DIR}/Common/b2BlockAllocator.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/Common/b2StackAllocator.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/Common/b2Math.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/Common/b2BlockAllocator.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/Common/b2Settings.cpp
	
	${CMAKE_CURRENT_SOURCE_DIR}/Collision/b2Collision.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/Collision/b2Distance.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/Collision/b2TimeOfImpact.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/Collision/b2PairManager.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/Collision/b2CollidePoly.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/Collision/b2CollideCircle.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/Collision/b2BroadPhase.cpp
	
	${CMAKE_CURRENT_SOURCE_DIR}/Collision/Shapes/b2Shape.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/Collision/Shapes/b2CircleShape.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/Collision/Shapes/b2PolygonShape.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/Collision/Shapes/b2EdgeShape.cpp
	
	
)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

add_library( ${box2d_LIB_NAME} STATIC ${box2d_sources} )

