# project menge

set( MY_LIB_NAME Menge )

if( MENGE_USE_MT )
	set( MY_LIB_NAME ${MY_LIB_NAME}_mt )
endif( MENGE_USE_MT )

ADD_FILTER(
Interface
	../Interface/ApplicationInterface.h
	../Interface/CodecInterface.h
	../Interface/ImageCodecInterface.h
	../Interface/SoundCodecInterface.h
	../Interface/VideoCodecInterface.h
	../Interface/FileSystemInterface.h
	../Interface/InputSystemInterface.h
	../Interface/LogSystemInterface.h
	../Interface/ParticleSystemInterface.h
	../Interface/PhysicSystem2DInterface.h
	../Interface/PhysicSystemInterface.h
	../Interface/RenderSystemInterface.h
	../Interface/SoundSystemInterface.h
	../Interface/ThreadSystemInterface.h
)

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
)

ADD_FILTER(
Camera
	Camera2D.cpp
	Camera2D.h
	Camera3D.cpp
	Camera3D.h
	Frustum.cpp
	Frustum.h
	Camera.h
	Camera.cpp
)

SOURCE_GROUP( Engine )

ADD_FILTER(
"Engine\\\\File"
	FileEngine.cpp
	FileEngine.h
	MemoryFileInput.h
	MemoryFileInput.cpp
	MengeStdioWrapper.cpp
	BufferedFileInput.h
	BufferedFileInput.cpp
	FileSystem.h
	FileSystemDirectory.h
	FileSystemDirectory.cpp
	FileSystemMemoryMapped.h
	FileSystemMemoryMapped.cpp
	FileSystemZip.h
	FileSystemZip.cpp
	SimpleFileOutput.h
	SimpleFileOutput.cpp
	FileOutput.h
	FileOutput.cpp
	FileInput.h
	FileInput.cpp
)

ADD_FILTER(
"Engine\\\\Xml"
	XmlElement.cpp
	XmlElement.h
	XmlElementListener.cpp
	XmlElementListener.h
	XmlElementValueListener.h
	XmlExpatParser.cpp
	XmlExpatParser.h
	XmlParser.cpp
	XmlParser.h
	XmlParserCast.cpp
	XmlParserCast.h
	XmlParserCheckMethod.h
	XmlParserElement.cpp
	XmlParserElement.h
)

ADD_FILTER(
Engine\\\\Codec
	CodecEngine.cpp
	CodecEngine.h
)


ADD_FILTER(
Engine\\\\Input
	InputEngine.cpp
	InputEngine.h
	InputHandler.cpp
	InputHandler.h
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
	RenderEngine.cpp
	RenderEngine.h
	Material.h
	Texture.h
	Texture.cpp
	Vertex.h
	Vertex.cpp
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
	XmlEngine.cpp
	XmlEngine.h
)

ADD_FILTER(
Entity
	Entity.cpp
	Entity.h
)

ADD_FILTER(
Service
	ServiceProvider.cpp
	ServiceProvider.h
)

ADD_FILTER(
Game
	Account.cpp
	Account.h
	ResourcePak.cpp	
	ResourcePak.h
	Game.cpp
	Game.h
)

ADD_FILTER(
Kernel
	MengeExport.h
	ValueInterpolator.h
)


ADD_FILTER(
Consts
	Consts.h
	Consts.cpp
)


ADD_FILTER(
Kernel\\\\Eventable
	Eventable.cpp
	Eventable.h
	EventEnum.cpp
	EventEnum.h
)

ADD_FILTER(
Kernel\\\\MousePickerTrap
	MousePickerSystem.cpp
	MousePickerSystem.h
	MousePickerTrap.cpp
	MousePickerTrap.h
	MousePickerAdapter.cpp
	MousePickerAdapter.h
)

ADD_FILTER(
Kernel\\\\Affector
	Affector.h
	Affector.cpp
)

ADD_FILTER(
Kernel\\\\Node
	Node.cpp
	Node.h
)

ADD_FILTER(
Kernel\\\\Node\\\\BoundingBox
	BoundingBox.cpp
	BoundingBox.h
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
Kernel\\\\Node\\\\Renderable
	Renderable.cpp
	Renderable.h
)

ADD_FILTER(
Kernel\\\\Node\\\\Transformation2D
	Transformation2D.cpp
	Transformation2D.h
)

ADD_FILTER(
Kernel\\\\Node\\\\Colorable
	Colorable.cpp
	Colorable.h
)

ADD_FILTER(
Kernel\\\\Node\\\\Affectorable
	Affectorable.cpp
	Affectorable.h
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
Kernel\\\\Vertices
	FixedVertices.h
	VectorVertices.cpp
	VectorVertices.h
)

ADD_FILTER(
Kernel\\\\Visitor
	Visitor.h
	VisitorAdapter.h
)

ADD_FILTER(
Kernel\\\\Loadable
	Loadable.h
)

ADD_FILTER(
Kernel\\\\BinParser
	BinParser.cpp
	BinParser.h
	BinProtocol.h
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
Object2D\\\\ParticleEmitter
	ParticleEmitter.cpp
	ParticleEmitter.h
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
Object2D\\\\TextField2
	TextField2.cpp
	TextField2.h
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
Object2D\\\\HotSpotImage
	HotSpotImage.cpp
	HotSpotImage.h
)

ADD_FILTER(
Object2D\\\\Mesh_40_30
	Mesh_40_30.cpp
	Mesh_40_30.h
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
	GlobalHandleAdapter.cpp
	GlobalHandleAdapter.h
	GlobalHandleSystem.cpp
	GlobalHandleSystem.h
)

ADD_FILTER(
Resource
	ResourceDeclare.h
	ResourceImplement.h
	ResourceReference.cpp
	ResourceReference.h
	ResourceVisitor.h
)

ADD_FILTER(
Resource\\\\Animation
	ResourceAnimation.cpp
	ResourceAnimation.h
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
Resource\\\\HotspotImage
	ResourceHotspotImage.cpp
	ResourceHotspotImage.h
)

ADD_FILTER(
Manager\\\\Resource
	ResourceManager.cpp
	ResourceManager.h
)

ADD_FILTER(
Manager\\\\AlphaChannel
	AlphaChannelManager.cpp
	AlphaChannelManager.h	
)

ADD_FILTER(
Manager\\\\Text
	TextManager.cpp
	TextManager.h
)

ADD_FILTER(
Manager\\\\Schedule
	ScheduleManager.cpp
	ScheduleManager.h
)

ADD_FILTER(
Manager\\\\Task
	TaskManager.h
	TaskManager.cpp
	Task.h
	Task.cpp
	TaskDeferredLoading.h
	TaskDeferredLoading.cpp
	TaskSoundBufferUpdate.h
	TaskSoundBufferUpdate.cpp
	TaskLoadPak.h
	TaskLoadPak.cpp
)

ADD_FILTER(
Manager\\\\Thread
	ThreadManager.h
	ThreadManager.cpp
)

ADD_FILTER(
Manager\\\\Const
	ConstManager.cpp
	ConstManager.h
)

ADD_FILTER(
Manager\\\\Node
	NodeManager.cpp
	NodeManager.h
)

ADD_FILTER(
Manager\\\\Scene

	SceneManager.cpp	
	SceneManager.h
)

ADD_FILTER(
Manager\\\\Arrow

	ArrowManager.cpp	
	ArrowManager.h
)

ADD_FILTER(
Manager\\\\Account

	AccountManager.cpp	
	AccountManager.h
)

ADD_FILTER(
Manager\\\\Factory
	Factorable.cpp
	Factorable.h
	Factory.cpp
	Factory.h
	FactoryDefault.h
	FactoryManager.cpp
	FactoryManager.h
	FactoryPool.h
)

ADD_FILTER(
Scene
	Allocator3D.cpp
	Allocator3D.h
	Scene.cpp
	Scene.h
	SceneNode3D.cpp
	SceneNode3D.h
)

ADD_FILTER(
Scene\\\\Layer
	Layer.cpp
	Layer.h
)

ADD_FILTER(
Scene\\\\Layer2D
	Layer2D.cpp
	Layer2D.h
	Layer2DAccumulator.cpp
	Layer2DAccumulator.h
	Layer2DLoop.cpp
	Layer2DLoop.h
	Layer2DTexture.cpp
	Layer2DTexture.h
)


ADD_FILTER(
Scene\\\\Layer3D
	Layer3D.cpp
	Layer3D.h
)

ADD_FILTER(
Scene\\\\LayerScene
	LayerScene.cpp
	LayerScene.h
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
Utils
	ConfigFile.cpp
	ConfigFile.h
	VersionInfo.h
)


ADD_FILTER(
FileLogger
	FileLogger.h
	FileLogger.cpp
)

INCLUDE_DIRECTORIES( ${Mengine_SOURCE_DIR}/Utils 
					 ${THIRDPARTY_DIR}
					 ${THIRDPARTY_DIR}/pybind/include
					 ${THIRDPARTY_DIR}/python/include
)

if( WIN32 )
	INCLUDE_DIRECTORIES( ${THIRDPARTY_DIR}/python/PC )
endif( WIN32 )

if( WIN32 )
	include_directories( ${THIRDPARTY_DIR}/python/PC )

endif( WIN32 )

ADD_DEFINITIONS( -DXML_STATIC )
add_definitions(-DPy_BUILD_CORE)

include_directories( ${Mengine_SOURCE_DIR}/Menge )

INCLUDE_DIRECTORIES( ${THIRDPARTY_DIR}/expat/lib )

if(APPLE)
	include_directories( ${MENGINE_REPOSITORY}/dependencies/Python )
endif(APPLE)
ADD_LIBRARY( ${MY_LIB_NAME} ${SRC_FILES} )

SET( APPLICATION_DEPENDENCIES
 ${APPLICATION_DEPENDENCIES}
 ${MY_LIB_NAME} 
)

SET( APPLICATION_DEPENDENCIES
 ${APPLICATION_DEPENDENCIES}
 PARENT_SCOPE
)

SET( APPLICATION_LIBS
 ${APPLICATION_LIBS}
debug ${MENGINE_LIB_PREFIX}pybind_static${MENGINE_LIB_SUFFIXD}
optimized ${MENGINE_LIB_PREFIX}pybind_static${MENGINE_LIB_SUFFIX}
debug ${MENGINE_LIB_PREFIX}python_static${MENGINE_LIB_SUFFIXD}
optimized ${MENGINE_LIB_PREFIX}python_static${MENGINE_LIB_SUFFIX}
debug ${MENGINE_LIB_PREFIX}Menge${MENGINE_LIB_SUFFIXD}		# Menge dependency
optimized ${MENGINE_LIB_PREFIX}Menge${MENGINE_LIB_SUFFIX}
debug ${MENGINE_LIB_PREFIX}png_static${MENGINE_LIB_SUFFIXD}
optimized ${MENGINE_LIB_PREFIX}png_static${MENGINE_LIB_SUFFIX}
debug ${MENGINE_LIB_PREFIX}theora_static${MENGINE_LIB_SUFFIXD}
optimized ${MENGINE_LIB_PREFIX}theora_static${MENGINE_LIB_SUFFIX}
debug ${MENGINE_LIB_PREFIX}vorbisfile_static${MENGINE_LIB_SUFFIXD}
optimized ${MENGINE_LIB_PREFIX}vorbisfile_static${MENGINE_LIB_SUFFIX}
debug ${MENGINE_LIB_PREFIX}vorbis_static${MENGINE_LIB_SUFFIXD}
optimized ${MENGINE_LIB_PREFIX}vorbis_static${MENGINE_LIB_SUFFIX}
debug ${MENGINE_LIB_PREFIX}ogg_static${MENGINE_LIB_SUFFIXD}
optimized ${MENGINE_LIB_PREFIX}ogg_static${MENGINE_LIB_SUFFIX}
debug ${MENGINE_LIB_PREFIX}zlib_static${MENGINE_LIB_SUFFIXD}
optimized ${MENGINE_LIB_PREFIX}zlib_static${MENGINE_LIB_SUFFIX}
debug ${MENGINE_LIB_PREFIX}jpeg_static${MENGINE_LIB_SUFFIXD}
optimized ${MENGINE_LIB_PREFIX}jpeg_static${MENGINE_LIB_SUFFIX}
debug ${MENGINE_LIB_PREFIX}expat_static${MENGINE_LIB_SUFFIXD}
optimized ${MENGINE_LIB_PREFIX}expat_static${MENGINE_LIB_SUFFIX}
)

SET( APPLICATION_LIBS
 ${APPLICATION_LIBS}
 PARENT_SCOPE
)
