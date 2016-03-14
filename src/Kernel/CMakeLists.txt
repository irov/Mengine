set( MY_LIB_NAME Kernel )

ADD_FILTER(
Eventable
	Eventable.cpp
	Eventable.h
)

ADD_FILTER(
Affector
	Affector.h
	Affector.cpp
	AffectorType.h
    AffectorType.cpp
)

ADD_FILTER(
Shape
	Shape.cpp
	Shape.h
)

ADD_FILTER(
Node
	Node.cpp
	Node.h    
    ScriptClassWrapper.h
)

ADD_FILTER(
PrototypeGenerator
    BasePrototypeGenerator.cpp
    BasePrototypeGenerator.h
    NodePrototypeGenerator.h
    ResourcePrototypeGenerator.h
    ScriptClassWrapper.h
)

ADD_FILTER(
Layer
	Layer.cpp
	Layer.h
)

ADD_FILTER(
Scene
	Scene.cpp
	Scene.h
)

ADD_FILTER(
Node\\\\ScriptHolder
	ScriptHolder.cpp
	ScriptHolder.h
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
Node\\\\Entity
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
Node\\\\Materialable
	Materialable.cpp
	Materialable.h
)

ADD_FILTER(
Parallax
	Parallax.cpp
	Parallax.h
)

ADD_FILTER(
Isometric
	Isometric.cpp
	Isometric.h
)


ADD_FILTER(
Camera
	Camera2D.cpp
	Camera2D.h
	CameraTarget2D.cpp
	CameraTarget2D.h
	Camera3D.cpp
	Camera3D.h
    RenderViewport.h
    RenderViewport.cpp
    RenderCamera.h
    RenderCamera.cpp
    RenderClipplane.h
    RenderClipplane.cpp
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
    ResourceCacher.h
    ResourceHolder.h        
    ResourceReference.cpp
    ResourceReference.h
)

ADD_FILTER(
Resource\\\\Image
	ResourceImage.cpp
	ResourceImage.h
	ResourceImageData.cpp
	ResourceImageData.h
)

ADD_FILTER(
Resource\\\\Music
    ResourceMusic.cpp
    ResourceMusic.h
)


ADD_FILTER(
Vertices
	FixedVertices.h
	VectorVertices.cpp
	VectorVertices.h
)

ADD_FILTER(
ThreadTask
	ThreadTask.h
	ThreadTask.cpp
    ThreadJob.h
    ThreadJob.cpp
	ThreadTaskPacket.h
	ThreadTaskPacket.cpp
)

ADD_FILTER(
Loadable
	Loadable.h
	Loadable.cpp
)

ADD_FILTER(
Visitable
	Visitable.h
)

ADD_FILTER(
Manager\\\\Schedule
	ScheduleManager.cpp
	ScheduleManager.h
)

ADD_FILTER(
Module
    ModuleFactory.h
)
   
INCLUDE_DIRECTORIES( 
    ${Mengine_SOURCE_DIR} 
    ${Mengine_SOURCE_DIR}/Utils 
    ${THIRDPARTY_DIR}/metabuf
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
