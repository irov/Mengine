set( MY_LIB_NAME Kernel )

ADD_FILTER(
Servant
    Servant.h
    Servant.cpp
)

ADD_FILTER(
Eventable
	Eventable.cpp
	Eventable.h
	EventReceiver.h
)

ADD_FILTER(
Affector
	Affector.h
	Affector.cpp
    AffectorHelper.h
    AffectorHelper.cpp
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
)

ADD_FILTER(
Surface
	Surface.cpp
	Surface.h
)

ADD_FILTER(
Entity
	Entity.cpp
	Entity.h
)

ADD_FILTER(
Scene
	Scene.cpp
	Scene.h
)

ADD_FILTER(
Arrow
	Arrow.cpp
	Arrow.h
)

ADD_FILTER(
Layer
	Layer.cpp
	Layer.h
)

ADD_FILTER(
PrototypeGenerator
    BasePrototypeGenerator.cpp
    BasePrototypeGenerator.h    
    DefaultPrototypeGenerator.h
    NodePrototypeGenerator.h
    ScriptablePrototypeGenerator.h
    ResourcePrototypeGenerator.h
    SurfacePrototypeGenerator.h    
)

ADD_FILTER(
BoundingBox
	BoundingBox.cpp
	BoundingBox.h
)

ADD_FILTER(
Identity
	Identity.cpp
	Identity.h
)

ADD_FILTER(
Scriptable
	Scriptable.cpp
	Scriptable.h
)

ADD_FILTER(
Animatable
	Animatable.h
)

ADD_FILTER(
Soundable
	Soundable.cpp
	Soundable.h
)

ADD_FILTER(
Comable
    Comable.h
)

ADD_FILTER(
Updatable
	Updatable.cpp
	Updatable.h
)

ADD_FILTER(
Renderable
	Renderable.cpp
	Renderable.h
)

ADD_FILTER(
Transformation3D
	Transformation3D.cpp
	Transformation3D.h
)

ADD_FILTER(
Colorable
	Colorable.cpp
	Colorable.h
)

ADD_FILTER(
Affectorable
	Affectorable.cpp
	Affectorable.h
)

ADD_FILTER(
Materialable
	Materialable.cpp
	Materialable.h
)

ADD_FILTER(
Animation
	BaseAnimation.h
    BaseAnimation.cpp
)


ADD_FILTER(
Parallax
	Parallax.cpp
	Parallax.h
)

ADD_FILTER(
MatrixProxy
	MatrixProxy.cpp
	MatrixProxy.h
)

ADD_FILTER(
Isometric
	Isometric.cpp
	Isometric.h
)

ADD_FILTER(
RenderCamera
	RenderCamera.cpp
	RenderCamera.h
    RenderCameraHelper.cpp
    RenderCameraHelper.h
    RenderCameraProxy.cpp
    RenderCameraProxy.h
    RenderCameraOrthogonal.cpp
    RenderCameraOrthogonal.h
    RenderCameraProjection.cpp
    RenderCameraProjection.h
	RenderCameraOrthogonalTarget.cpp
	RenderCameraOrthogonalTarget.h
    RenderViewport.h
    RenderViewport.cpp
    RenderScissor.h
    RenderScissor.cpp
)

ADD_FILTER(
Reference
	Reference.cpp
	Reference.h
)

ADD_FILTER(
Resource
	Compilable.cpp
	Compilable.h
    ResourceCacher.h
    ResourceHolder.h        
    Resource.cpp
    Resource.h
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
ScheduleManager
	ScheduleManager.cpp
	ScheduleManager.h
)

ADD_FILTER(
ScriptWrapper
	ScriptWrapper.h
	ScriptEventReceiver.h	
    AnimationEventReceiver.h
)

ADD_FILTER(
Randomizer
	MT19937Randomizer.cpp
	MT19937Randomizer.h	
)

ADD_FILTER(
Observable
	Observable.h
)
   
   
INCLUDE_DIRECTORIES(
    ${SOURCE_DIR}
    ${SOURCE_DIR}/Utils
    ${THIRDPARTY_DIR}/metabuf
    ${THIRDPARTY_DIR}/libmath/include
    ${THIRDPARTY_DIR}/pybind/include
    )     

ADD_MENGINE_LIBRARY()

SET( APPLICATION_DEPENDENCIES
 ${APPLICATION_DEPENDENCIES}
 ${MY_LIB_NAME}
)


SET( APPLICATION_DEPENDENCIES
 ${APPLICATION_DEPENDENCIES}
 PARENT_SCOPE
)
