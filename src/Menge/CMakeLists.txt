# project menge

#SET( SRC_FILES )

ADD_FILTER(
Amplifier
	Amplifier.cpp
	Amplifier.h
	Playlist.cpp
	Playlist.h
)

ADD_FILTER(
Application
	Application.cpp
	Application.h
	Resolution.cpp
	Resolution.h
)

ADD_FILTER(
Camera
	Camera2D.cpp
	Camera2D.h
	Camera3D.cpp
	Camera3D.h
	Frustum.cpp
	Frustum.h
	Viewport.cpp
	Viewport.h
)

ADD_FILTER(
Codec
	Codec.h
	VideoDecoderOGGTheora.cpp
	VideoDecoderOGGTheora.h
	ImageDecoderJPEG.h
	ImageDecoderJPEG.cpp
	#ImageCodecMNE.cpp
	#ImageCodecMNE.h
	ImageDecoderPNG.cpp
	ImageDecoderPNG.h
	SoundDecoderOGGVorbis.h
	SoundDecoderOGGVorbis.cpp
)

SOURCE_GROUP( Engine )

ADD_FILTER(
"Engine\\\\File"
	FileEngine.cpp
	FileEngine.h
)

ADD_FILTER(
Engine\\\\Input
	InputEngine.cpp
	InputEngine.h
	InputHandler.cpp
	InputHandler.h
)

ADD_FILTER(
Engine\\\\Log
	LogEngine.cpp
	LogEngine.h
)

ADD_FILTER(
Engine\\\\Particle
	ParticleEngine.cpp
	ParticleEngine.h
)

ADD_FILTER(
Engine\\\\Physic
	PhysicEngine.cpp
	PhysicEngine.h
	PhysicEngine2D.cpp
	PhysicEngine2D.h
)

ADD_FILTER(
Engine\\\\Render
	Bitwise.h
	ColourValue.cpp
	ColourValue.h
	PixelFormat.cpp
	PixelFormat.h
	RenderEngine.cpp
	RenderEngine.h
)

ADD_FILTER(
Engine\\\\Script
	ScriptClassDeclaration.cpp
	ScriptClassDeclaration.h
	ScriptClassWrapper.cpp
	ScriptClassWrapper.h
	ScriptClassWrapperDefine.h
	ScriptDeclarationDefine.h
	ScriptEngine.cpp
	ScriptEngine.h
	ScriptModuleDeclaration.cpp
	ScriptModuleDeclaration.h
)

ADD_FILTER(
Engine\\\\Sound
	SoundEngine.cpp
	SoundEngine.h
)

ADD_FILTER(
Engine\\\\Xml
	XmlCompile.cpp
	XmlCompile.h
	XmlEngine.cpp
	XmlEngine.h
	XmlEnum.h
	XmlFormat.cpp
	XmlFormat.h
	XmlTag.cpp
	XmlTag.h
)

ADD_FILTER(
Entity
	Entity.cpp
	Entity.h
)

ADD_FILTER(
Game
	Account.cpp
	Account.h
	Game.cpp
	Game.h
)

ADD_FILTER(
Kernel
	Factory.h
	Holder.h
	MengeExport.h
	ObjectDeclare.h
	ObjectImplement.h
	ValueInterpolator.h
)

ADD_FILTER(
Kernel\\\\Eventable
	Eventable.cpp
	Eventable.h
	EventEnum.h
)

ADD_FILTER(
Kernel\\\\MousePickerTrap
	MousePickerSystem.cpp
	MousePickerSystem.h
	MousePickerTrap.cpp
	MousePickerTrap.h
)

ADD_FILTER(
Kernel\\\\Node
	Node.cpp
	Node.h
	Node2D.cpp
	Node2D.h
)

ADD_FILTER(
Kernel\\\\Node\\\\BoundingBox
	BoundingBox.cpp
	BoundingBox.h
)

ADD_FILTER(
Kernel\\\\Node\\\\Factory
	NodeFactory.h
)

ADD_FILTER(
Kernel\\\\Node\\\\Identity
	Identity.cpp
	Identity.h
)

ADD_FILTER(
Kernel\\\\Node\\\\Scriptable
	Scriptable.cpp
	Scriptable.h
)

ADD_FILTER(
Kernel\\\\Reference
	Reference.cpp
	Reference.h
)

ADD_FILTER(
Kernel\\\\Resource
	Resource.cpp
	Resource.h
)

ADD_FILTER(
Kernel\\\\Visitor
	Visitor.h
	VisitorAdapter.h
)

SOURCE_GROUP( Object2D )

ADD_FILTER(
Object2D\\\\Animation
	Animation.cpp
	Animation.h
)

ADD_FILTER(
Object2D\\\\Arrow
	Arrow.cpp
	Arrow.h
)

ADD_FILTER(
Object2D\\\\Emitter
	Emitter.cpp
	Emitter.h
)

ADD_FILTER(
Object2D\\\\HotSpot
	HotSpot.cpp
	HotSpot.h
)

ADD_FILTER(
Object2D\\\\Point
	Point.cpp
	Point.h
)

ADD_FILTER(
Object2D\\\\RigidBody2D
	RigidBody2D.cpp
	RigidBody2D.h
)

ADD_FILTER(
Object2D\\\\Shadows
	Light2D.cpp
	Light2D.h
	ShadowCaster2D.cpp
	ShadowCaster2D.h
)

ADD_FILTER(
Object2D\\\\Shadows\\\\LightSystem
	LightSystem.cpp
	LightSystem.h
)

ADD_FILTER(
Object2D\\\\SoundEmitter
	SoundEmitter.cpp
	SoundEmitter.h
)

ADD_FILTER(
Object2D\\\\Sprite
	Sprite.cpp
	Sprite.h
)

ADD_FILTER(
Object2D\\\\TextField
	TextField.cpp
	TextField.h
	TextLine.cpp
	TextLine.h
)

ADD_FILTER(
Object2D\\\\TileMap
	TileMap.cpp
	TileMap.h
)

ADD_FILTER(
Object2D\\\\TilePolygon
	TilePolygon.cpp
	TilePolygon.h
)

ADD_FILTER(
Object2D\\\\Track
	Track.cpp
	Track.h
	TrackChain.cpp
	TrackChain.h
)

ADD_FILTER(
Object2D\\\\Video
	Video.cpp
	Video.h
)

ADD_FILTER(
Object2D\\\\Window
	Window.cpp
	Window.h
)

ADD_FILTER(
Object3D
	RenderMesh.cpp
	RenderMesh.h
)

ADD_FILTER(
Player
	Player.cpp
	Player.h
)

ADD_FILTER(
Resource
	ResourceDeclare.h
	ResourceImplement.h
	ResourceManager.cpp
	ResourceManager.h
	ResourceReference.cpp
	ResourceReference.h
)

ADD_FILTER(
Resource\\\\Animation
	ResourceAnimation.cpp
	ResourceAnimation.h
)

ADD_FILTER(
Resource\\\\Controller
	ResourceCapsuleController.cpp
	ResourceCapsuleController.h
)

ADD_FILTER(
Resource\\\\EmitterContainer
	ResourceEmitterContainer.cpp
	ResourceEmitterContainer.h
)

ADD_FILTER(
Resource\\\\Font
	ResourceFont.cpp
	ResourceFont.h
)

ADD_FILTER(
Resource\\\\Image
	ResourceImage.cpp
	ResourceImage.h
	ResourceImageAtlas.cpp
	ResourceImageAtlas.h
	ResourceImageCell.cpp
	ResourceImageCell.h
	ResourceImageDefault.cpp
	ResourceImageDefault.h
	ResourceImageDynamic.cpp
	ResourceImageDynamic.h
	ResourceImageSet.cpp
	ResourceImageSet.h
)

ADD_FILTER(
Resource\\\\Material
	ResourceMaterial.cpp
	ResourceMaterial.h
)

ADD_FILTER(
Resource\\\\Mesh
	ResourceMesh.cpp
	ResourceMesh.h
	ResourceMeshMD3.cpp
	ResourceMeshMD3.h
	ResourceMeshMS3D.cpp
	ResourceMeshMS3D.h
	ResourceMeshNoise.cpp
	ResourceMeshNoise.h
	ResourceSkeleton.cpp
	ResourceSkeleton.h
)


ADD_FILTER(
Resource\\\\PhysicGeometry
	ResourcePhysicBoxGeometry.cpp
	ResourcePhysicBoxGeometry.h
	ResourcePhysicConcaveGeometry.cpp
	ResourcePhysicConcaveGeometry.h
	ResourcePhysicConvexGeometry.cpp
	ResourcePhysicConvexGeometry.h
	ResourcePhysicGeometry.cpp
	ResourcePhysicGeometry.h
)

ADD_FILTER(
Resource\\\\Playlist
	ResourcePlaylist.cpp
	ResourcePlaylist.h
)


ADD_FILTER(
Resource\\\\Sound
	ResourceSound.cpp
	ResourceSound.h
)

ADD_FILTER(
Resource\\\\TileMap
	ResourceTileMap.cpp
	ResourceTileMap.h
	ResourceTileSet.cpp
	ResourceTileSet.h
)


ADD_FILTER(
Resource\\\\TilePolygon
	ResourceTilePolygon.cpp
	ResourceTilePolygon.h
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
SceneManager
	SceneManager.cpp
	SceneManager.h
)


ADD_FILTER(
SceneManager\\\\Scene
	Allocator2D.cpp
	Allocator2D.h
	Allocator3D.cpp
	Allocator3D.h
	Scene.cpp
	Scene.h
	SceneNode3D.cpp
	SceneNode3D.h
)

ADD_FILTER(
SceneManager\\\\Scene\\\\Layer
	Layer.cpp
	Layer.h
)

ADD_FILTER(
SceneManager\\\\Scene\\\\Layer2D
	Layer2D.cpp
	Layer2D.h
	Layer2DAccumulator.cpp
	Layer2DAccumulator.h
	Layer2DLoop.cpp
	Layer2DLoop.h
)


ADD_FILTER(
SceneManager\\\\Scene\\\\Layer3D
	Layer3D.cpp
	Layer3D.h
)

ADD_FILTER(
SceneManager\\\\Scene\\\\LayerScene
	LayerScene.cpp
	LayerScene.h
)

ADD_FILTER(
ScheduleManager
	ScheduleManager.cpp
	ScheduleManager.h
)

ADD_FILTER(
ScriptWrapper
	EntityScriptWrapper.cpp
	HelperScriptWrapper.cpp
	NodeScriptWrapper.cpp
	ScriptWrapper.h
	SoundScriptWrapper.cpp
)

ADD_FILTER(
ScriptWrapper\\\\C4AI
	C4AI.cpp
	C4AI.h
)

ADD_FILTER(
ScriptWrapper\\\\CornersAI
	CornersAI.cpp
	CornersAI.h
)

ADD_FILTER(
ScriptWrapper\\\\ReversiAI
	ReversiAI.cpp
	ReversiAI.h
)

ADD_FILTER(
TextManager
	TextManager.cpp
	TextManager.h
)

ADD_FILTER(
Utils
	ConfigFile.cpp
	ConfigFile.h
	Profiler.cpp
	Profiler.h
	Utils.cpp
	Utils.h
)

ADD_LIBRARY( Menge ${SRC_FILES} )

SET( APPLICATION_DEPENDENCIES
 ${APPLICATION_DEPENDENCIES}
 Menge 
)

SET( APPLICATION_DEPENDENCIES
 ${APPLICATION_DEPENDENCIES}
 PARENT_SCOPE
)

SET( APPLICATION_LIBS
 ${APPLICATION_LIBS}
debug pybind_d.lib
optimized pybind.lib
debug python25_d.lib
optimized python25.lib
debug libpngd.lib
optimized libPNG.lib
debug zlibd.lib
optimized zlib.lib
debug libjpegd.lib
optimized libJPEG.lib
debug libtheora_d.lib
optimized libtheora.lib
debug ogg_static_d.lib
optimized ogg_static.lib
debug vorbis_static_d.lib
optimized vorbis_static.lib
debug vorbisfile_static_d.lib
optimized vorbisfile_static.lib
)

SET( APPLICATION_LIBS
 ${APPLICATION_LIBS}
 PARENT_SCOPE
)

