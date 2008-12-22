
ADD_FILTER(
src
	Box2DPhysicBody.cpp
	Box2DPhysicBody.h
	Box2DPhysicJoint.cpp
	Box2DPhysicJoint.h
	Box2DPhysicSystem.cpp
	Box2DPhysicSystem.h
	Box2DPhysicSystem.def
)

ADD_LIBRARY( Box2DPhysicSystem ${SRC_FILES} )

SET( APPLICATION_DEPENDENCIES
 ${APPLICATION_DEPENDENCIES}
 Box2DPhysicSystem
)

SET( APPLICATION_DEPENDENCIES
 ${APPLICATION_DEPENDENCIES}
 PARENT_SCOPE
)