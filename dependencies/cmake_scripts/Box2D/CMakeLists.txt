
CMAKE_MINIMUM_REQUIRED( VERSION 2.6 )
                                             
project(box2d)

set(TRUE_DIR ../../box2d/Source)
set(BOX2D_SOURCE_DIR ../../box2d)

INCLUDE( ../menge_macro.cmake )

MENGE_SET_PROJECT_DEFAULTS( box2d )

# OUR SOURCES
set( box2d_sources
	${TRUE_DIR}/Dynamics/b2Body.cpp
	${TRUE_DIR}/Dynamics/b2Island.cpp
	${TRUE_DIR}/Dynamics/b2World.cpp
	${TRUE_DIR}/Dynamics/b2ContactManager.cpp
	${TRUE_DIR}/Dynamics/Contacts/b2Contact.cpp
	${TRUE_DIR}/Dynamics/Contacts/b2PolyContact.cpp
	${TRUE_DIR}/Dynamics/Contacts/b2CircleContact.cpp
	${TRUE_DIR}/Dynamics/Contacts/b2PolyAndCircleContact.cpp
	${TRUE_DIR}/Dynamics/Contacts/b2EdgeAndCircleContact.cpp
	${TRUE_DIR}/Dynamics/Contacts/b2PolyAndEdgeContact.cpp
	${TRUE_DIR}/Dynamics/Contacts/b2ContactSolver.cpp
	${TRUE_DIR}/Dynamics/b2WorldCallbacks.cpp
	${TRUE_DIR}/Dynamics/Joints/b2MouseJoint.cpp
	${TRUE_DIR}/Dynamics/Joints/b2PulleyJoint.cpp
	${TRUE_DIR}/Dynamics/Joints/b2Joint.cpp
	${TRUE_DIR}/Dynamics/Joints/b2RevoluteJoint.cpp
	${TRUE_DIR}/Dynamics/Joints/b2PrismaticJoint.cpp
	${TRUE_DIR}/Dynamics/Joints/b2DistanceJoint.cpp
	${TRUE_DIR}/Dynamics/Joints/b2GearJoint.cpp
	${TRUE_DIR}/Dynamics/Joints/b2LineJoint.cpp
	${TRUE_DIR}/Dynamics/Controllers/b2Controller.cpp
	${TRUE_DIR}/Dynamics/Controllers/b2BuoyancyController.cpp
	${TRUE_DIR}/Dynamics/Controllers/b2GravityController.cpp
	${TRUE_DIR}/Dynamics/Controllers/b2TensorDampingController.cpp
	${TRUE_DIR}/Dynamics/Controllers/b2ConstantForceController.cpp
	${TRUE_DIR}/Dynamics/Controllers/b2ConstantAccelController.cpp
	${TRUE_DIR}/Common/b2StackAllocator.cpp
	${TRUE_DIR}/Common/b2Math.cpp
	${TRUE_DIR}/Common/b2BlockAllocator.cpp
	${TRUE_DIR}/Common/b2Settings.cpp
	${TRUE_DIR}/Collision/b2Collision.cpp
	${TRUE_DIR}/Collision/b2Distance.cpp
	${TRUE_DIR}/Collision/Shapes/b2Shape.cpp
	${TRUE_DIR}/Collision/Shapes/b2CircleShape.cpp
	${TRUE_DIR}/Collision/Shapes/b2PolygonShape.cpp
	${TRUE_DIR}/Collision/Shapes/b2EdgeShape.cpp
	${TRUE_DIR}/Collision/b2TimeOfImpact.cpp
	${TRUE_DIR}/Collision/b2PairManager.cpp
	${TRUE_DIR}/Collision/b2CollidePoly.cpp
	${TRUE_DIR}/Collision/b2CollideCircle.cpp
	${TRUE_DIR}/Collision/b2BroadPhase.cpp
)

include_directories(${box2d_SOURCE_DIR}/include)
add_library( ${box2d_LIB_NAME} STATIC ${box2d_sources} )
