set( MY_LIB_NAME Engine )

ADD_FILTER(
Application
	Application.cpp
	Application.h
)

SOURCE_GROUP( Engine )

ADD_FILTER(
Service\\\\Watchdog
	Watchdog.cpp
	Watchdog.h
)

ADD_FILTER(
Layer2D
	Layer2D.cpp
	Layer2D.h
)

ADD_FILTER(
Motor\\\\Motor
	Motor.cpp
	Motor.h
)

ADD_FILTER(
MousePickerTrap
	MousePickerSystem.cpp
	MousePickerSystem.h
)

SOURCE_GROUP( Surface )

ADD_FILTER(
Surface\\\\SurfaceVideo
	SurfaceVideo.cpp
	SurfaceVideo.h
)

ADD_FILTER(
Surface\\\\SurfaceSound
	SurfaceSound.cpp
	SurfaceSound.h
)

ADD_FILTER(
Surface\\\\SurfaceSolidColor
	SurfaceSolidColor.cpp
	SurfaceSolidColor.h
)

ADD_FILTER(
Surface\\\\SurfaceImage
	SurfaceImage.cpp
	SurfaceImage.h
)

ADD_FILTER(
Surface\\\\SurfaceImageSequence
	SurfaceImageSequence.cpp
	SurfaceImageSequence.h
)

ADD_FILTER(
Surface\\\\SurfaceTrackMatte
	SurfaceTrackMatte.cpp
	SurfaceTrackMatte.h
)

SOURCE_GROUP( Object )

ADD_FILTER(
Shape\\\\ShapeQuad
	ShapeQuad.cpp
	ShapeQuad.h
	ShapeQuadFixed.cpp
	ShapeQuadFixed.h
	ShapeQuadFlex.cpp
	ShapeQuadFlex.h
)


SOURCE_GROUP( Object )

ADD_FILTER(
Object\\\\ParticleEmitter2
	ParticleEmitter2.cpp
	ParticleEmitter2.h
)

ADD_FILTER(
Object\\\\HotSpot
	HotSpot.cpp
	HotSpot.h
	HotSpotPolygon.cpp
	HotSpotPolygon.h
	HotSpotImage.cpp
	HotSpotImage.h
	HotSpotBubbles.cpp
	HotSpotBubbles.h
	HotSpotShape.cpp
	HotSpotShape.h
    HotSpotCircle.cpp
    HotSpotCircle.h
    
    HotspotMousePickerVisitor.h
    HotspotMousePickerVisitor.cpp
)

ADD_FILTER(
Object\\\\Point
	Point.cpp
	Point.h
)

ADD_FILTER(
Object\\\\Line
	Line.cpp
	Line.h
)


ADD_FILTER(
Object\\\\SoundEmitter
	SoundEmitter.cpp
	SoundEmitter.h
)

ADD_FILTER(
Object\\\\Landscape2D
	Landscape2D.cpp
	Landscape2D.h
)

ADD_FILTER(
Object\\\\Meshget
	Meshget.cpp
	Meshget.h
)


ADD_FILTER(
Object\\\\Grid2D
	Grid2D.cpp
	Grid2D.h
)

ADD_FILTER(
Object\\\\Mesh2D
	Mesh2D.cpp
	Mesh2D.h
)

ADD_FILTER(
Object\\\\Model3D
	Model3D.cpp
	Model3D.h
)

ADD_FILTER(
Object\\\\Gyroscope
	Gyroscope.cpp
	Gyroscope.h
)

ADD_FILTER(
Object\\\\Movie
	Movie.cpp
	Movie.h	
    MovieSlot.cpp
    MovieSlot.h
    MovieSceneEffect.cpp
    MovieSceneEffect.h
	MovieInternalObject.cpp
	MovieInternalObject.h
    MovieNodeExtra.cpp
    MovieNodeExtra.h
    MovieEvent.cpp
    MovieEvent.h
)

ADD_FILTER(
Object\\\\TextField
	TextField.cpp
	TextField.h
	TextLine.cpp
	TextLine.h
    TextChar.cpp
    TextChar.h
)

ADD_FILTER(
Object\\\\Window
	Window.cpp
	Window.h
)

ADD_FILTER(
Player
	Player.cpp
	Player.h
    GlobalAffectorable.cpp
    GlobalAffectorable.h    
	GlobalHandleSystem.cpp
	GlobalHandleSystem.h
)

ADD_FILTER(
Resource\\\\File
	ResourceFile.cpp
	ResourceFile.h
)

ADD_FILTER(
Resource\\\\Movie
	ResourceMovie.cpp
	ResourceMovie.h
)

ADD_FILTER(
Resource\\\\Model3D
	ResourceModel3D.cpp
	ResourceModel3D.h
)

ADD_FILTER(
Resource\\\\InternalObject
	ResourceInternalObject.cpp
	ResourceInternalObject.h
)

ADD_FILTER(
Resource\\\\Animation
	ResourceAnimation.cpp
	ResourceAnimation.h
)

ADD_FILTER(
Resource\\\\Cursor
	ResourceCursor.h
	ResourceCursorICO.cpp
	ResourceCursorICO.h
    ResourceCursorSystem.cpp
    ResourceCursorSystem.h
)

ADD_FILTER(
Resource\\\\Particle
	ResourceParticle.cpp
	ResourceParticle.h
)

ADD_FILTER(
Resource\\\\Image
	ResourceImageSolid.h
    ResourceImageSolid.cpp
	ResourceImageDefault.cpp
	ResourceImageDefault.h
	ResourceImageSubstract.cpp
	ResourceImageSubstract.h
	ResourceImageSubstractRGBAndAlpha.cpp
	ResourceImageSubstractRGBAndAlpha.h
)

ADD_FILTER(
Resource\\\\Sound
	ResourceSound.cpp
	ResourceSound.h
)

ADD_FILTER(
Resource\\\\Shape
	ResourceShape.cpp
	ResourceShape.h
)

ADD_FILTER(
Resource\\\\Video
	ResourceVideo.cpp
	ResourceVideo.h
)

ADD_FILTER(
Resource\\\\Window
	ResourceWindow.cpp
	ResourceWindow.h
)

ADD_FILTER(
Resource\\\\TestPick
	ResourceTestPick.h
)

ADD_FILTER(
Resource\\\\HIT
	ResourceHIT.cpp
	ResourceHIT.h
)


ADD_FILTER(
Manager\\\\Resource
	ResourceManager.cpp
	ResourceManager.h
)

ADD_FILTER(
Manager\\\\Prototype
	PrototypeManager.cpp
	PrototypeManager.h
)

ADD_FILTER(
Manager\\\\Node
	NodeManager.cpp
	NodeManager.h
)

ADD_FILTER(
Manager\\\\Account
	AccountManager.cpp	
	AccountManager.h
  	Account.cpp
	Account.h
)

ADD_FILTER(
Utils
	VersionInfo.h
)

INCLUDE_DIRECTORIES( 
    ${Mengine_SOURCE_DIR}/Utils 
    ${THIRDPARTY_DIR}
    ${THIRDPARTY_DIR}/pybind/include
    ${THIRDPARTY_DIR}/boost
    ${THIRDPARTY_DIR}/metabuf
    ${THIRDPARTY_DIR}/stdex/include
    ${THIRDPARTY_DIR}/utf8/source
    ${THIRDPARTY_DIR}/tinyini/include
    ${THIRDPARTY_DIR}/libmath/include
)

ADD_LIBRARY( ${MY_LIB_NAME} ${SRC_FILES} )

SET( APPLICATION_DEPENDENCIES
 ${APPLICATION_DEPENDENCIES}
 ${MY_LIB_NAME}  
 Kernel
 Core
 Logger
 Codec
 Factory
 Archive
 Metacode 
 ServiceText
 ServiceGraveyard
 ServiceRender
 ServicePackage
 ServiceProfiler
 ServiceUserdata
 ServiceTimeline
 ServiceNotification
)

SET( APPLICATION_DEPENDENCIES
 ${APPLICATION_DEPENDENCIES}
 PARENT_SCOPE
)

SET( APPLICATION_LIBS
    ${APPLICATION_LIBS}
    ${THIRDPARTY_LIB_DIR}/${MENGINE_LIB_PREFIX}zlib${MENGINE_LIB_SUFFIX}
    ${THIRDPARTY_LIB_DIR}/${MENGINE_LIB_PREFIX}stdex${MENGINE_LIB_SUFFIX}
    ${THIRDPARTY_LIB_DIR}/${MENGINE_LIB_PREFIX}tinyini${MENGINE_LIB_SUFFIX}
    ${THIRDPARTY_LIB_DIR}/${MENGINE_LIB_PREFIX}math${MENGINE_LIB_SUFFIX}
    )
    
SET( APPLICATION_LIBS
 ${APPLICATION_LIBS}
 PARENT_SCOPE
)
