set( MY_LIB_NAME Kernel )

ADD_FILTER(
Eventable
	Eventable.cpp
	Eventable.h
	EventEnum.cpp
	EventEnum.h
)

ADD_FILTER(
Affector
	Affector.h
	Affector.cpp
	AffectorType.h
    AffectorType.cpp
)
ADD_FILTER(
GlobalHandleAdapter
	GlobalHandleAdapter.cpp
	GlobalHandleAdapter.h
)

ADD_FILTER(
Node
	Node.cpp
	Node.h
)

ADD_FILTER(
Layer
	Layer.cpp
	Layer.h
)

ADD_FILTER(
Layer
	Scene.cpp
	Scene.h
)

ADD_FILTER(
Node\\\\Join
	Join.cpp
	Join.h
)

ADD_FILTER(
Node\\\\BoundingBox
	BoundingBox.cpp
	BoundingBox.h
)

ADD_FILTER(
Node\\\\Identity
	Identity.cpp
	Identity.h
)

ADD_FILTER(
Node\\\\Identity
	Entity.cpp
	Entity.h
)

ADD_FILTER(
Node\\\\Scriptable
	Scriptable.cpp
	Scriptable.h
)

ADD_FILTER(
Node\\\\Animatable
	Animatable.cpp
	Animatable.h
)

ADD_FILTER(
Node\\\\Soundable
	Soundable.cpp
	Soundable.h
)

ADD_FILTER(
Node\\\\Updatable
	Updatable.cpp
	Updatable.h
)

ADD_FILTER(
Node\\\\Renderable
	Renderable.cpp
	Renderable.h
)

ADD_FILTER(
Node\\\\Transformation3D
	Transformation3D.cpp
	Transformation3D.h
)

ADD_FILTER(
Node\\\\Colorable
	Colorable.cpp
	Colorable.h
)

ADD_FILTER(
Node\\\\Affectorable
	Affectorable.cpp
	Affectorable.h
)

ADD_FILTER(
Reference
	Reference.cpp
	Reference.h
)

ADD_FILTER(
Resource
	Resource.cpp
	Resource.h
    ResourceReference.cpp
    ResourceReference.h
	ResourceDeclare.h
	ResourceImplement.h
	ResourceVisitor.h
)

ADD_FILTER(
Vertices
	FixedVertices.h
	VectorVertices.cpp
	VectorVertices.h
)

ADD_FILTER(
src
	ThreadTask.h
	ThreadTask.cpp
	ThreadTaskPacket.h
	ThreadTaskPacket.cpp
)

ADD_FILTER(
Loadable
	Loadable.h
	Loadable.cpp
)

ADD_FILTER(
Visitor
	Visitor.h
	Visitable.h
)
   
INCLUDE_DIRECTORIES( 
    ${Mengine_SOURCE_DIR} 
    ${Mengine_SOURCE_DIR}/Utils 
    ${THIRDPARTY_DIR}/pybind/include
    ${THIRDPARTY_DIR}/python/include
    )     

ADD_LIBRARY( ${MY_LIB_NAME} ${SRC_FILES} )

SET( APPLICATION_DEPENDENCIES
 ${APPLICATION_DEPENDENCIES}
 ${MY_LIB_NAME}
)


SET( APPLICATION_DEPENDENCIES
 ${APPLICATION_DEPENDENCIES}
 PARENT_SCOPE
)
