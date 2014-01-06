# project menge

set( MY_LIB_NAME Menge )

ADD_FILTER(
Amplifier
	AmplifierService.cpp
	AmplifierService.h
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
	CameraTarget2D.cpp
	CameraTarget2D.h
	Camera3D.cpp
	Camera3D.h
	Frustum.cpp
	Frustum.h
	Camera.h
	Camera.cpp
    RenderViewport.h
    RenderViewport.cpp
)

SOURCE_GROUP( Engine )

ADD_FILTER(
Service\\\\Notification
	NotificationService.cpp
	NotificationService.h
)

ADD_FILTER(
Service\\\\Watchdog
	Watchdog.cpp
	Watchdog.h
)

ADD_FILTER(
Engine\\\\Input
	InputEngine.cpp
	InputEngine.h
)

ADD_FILTER(
Engine\\\\Script
	ScriptClassWrapper.h
)

ADD_FILTER(
Arrow
	Arrow.cpp
	Arrow.h
)

ADD_FILTER(
Game
	ResourcePak.cpp	
	ResourcePak.h
	Game.cpp
	Game.h
)

ADD_FILTER(
Consts
	Consts.h
	Consts.cpp
)

ADD_FILTER(
MousePickerTrap
	MousePickerSystem.cpp
	MousePickerSystem.h
	MousePickerAdapter.cpp
	MousePickerAdapter.h
)

SOURCE_GROUP( Object2D )

ADD_FILTER(
Object2D\\\\Animation
	Animation.cpp
	Animation.h
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
    HotspotMousePickerAdapter.h
    HotspotMousePickerAdapter.cpp
    HotspotMousePickerVisitor.h
    HotspotMousePickerVisitor.cpp
)

ADD_FILTER(
Object2D\\\\Point
	Point.cpp
	Point.h
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
Object2D\\\\Landscape2D
	Landscape2D.cpp
	Landscape2D.h
)

ADD_FILTER(
Object2D\\\\Mesh2D
	Mesh2D.cpp
	Mesh2D.h
)

ADD_FILTER(
Object2D\\\\Model3D
	Model3D.cpp
	Model3D.h
)

ADD_FILTER(
Object2D\\\\Movie
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
Object2D\\\\TextField
	TextField.cpp
	TextField.h
	TextLine.cpp
	TextLine.h
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
Object2D\\\\HotSpotShape
	HotSpotShape.cpp
	HotSpotShape.h
)

ADD_FILTER(
Prefetcher
    ThreadWorkerImageResourceCompile.h
    ThreadWorkerImageResourceCompile.cpp
)

ADD_FILTER(
Player
	Player.cpp
	Player.h
	GlobalHandleSystem.cpp
	GlobalHandleSystem.h
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
Resource\\\\EmitterContainer
	ResourceEmitterContainer.cpp
	ResourceEmitterContainer.h
	ResourceEmitter.cpp
	ResourceEmitter.h
)

ADD_FILTER(
Resource\\\\Image
	ResourceImage.cpp
	ResourceImage.h
	ResourceImageDefault.cpp
	ResourceImageDefault.h
	ResourceImageSubstract.cpp
	ResourceImageSubstract.h
	ResourceImageCombineRGBAndAlpha.cpp
	ResourceImageCombineRGBAndAlpha.h
	ResourceImageMultiplyRGBAndAlpha.cpp
	ResourceImageMultiplyRGBAndAlpha.h
	ResourceImageSolid.h
    ResourceImageSolid.cpp
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
Manager\\\\Text
	TextManager.cpp
	TextManager.h
    TextEntry.cpp
    TextEntry.h
    TextFont.cpp
    TextFont.h
    TextGlyph.cpp
    TextGlyph.h
    TextGlyphChar.cpp
    TextGlyphChar.h
    TextLocalePak.cpp
    TextLocalePak.h
)

ADD_FILTER(
Manager\\\\Schedule
	ScheduleManager.cpp
	ScheduleManager.h
)

ADD_FILTER(
Manager\\\\Timing
	TimingManager.cpp
	TimingManager.h
)

ADD_FILTER(
Manager\\\\ThreadTask
	ThreadTaskLoadResourceImage.h
	ThreadTaskLoadResourceImage.cpp
    ThreadTaskLoadImageResources.h
    ThreadTaskLoadImageResources.cpp
	ThreadTaskDeferredLoading.h
	ThreadTaskDeferredLoading.cpp
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
Scene\\\\Layer2D
	Layer2D.cpp
	Layer2D.h
	Layer2DParallax.cpp
	Layer2DParallax.h
)

ADD_FILTER(
ScriptWrapper
    ScriptWrapper.h
    EntityScriptWrapper.cpp
	HelperScriptWrapper.cpp
    ConstsScriptWrapper.cpp
    NodeScriptWrapper.cpp	
    MathScriptWrapper.cpp
    SoundScriptWrapper.cpp
)

ADD_FILTER(
ScriptWrapper2
    EntityScriptWrapper2.cpp
    HelperScriptWrapper2.cpp
    NodeScriptWrapper2.cpp	
    MathScriptWrapper2.cpp
	SoundScriptWrapper2.cpp
)


ADD_FILTER(
Utils
	VersionInfo.h
)

INCLUDE_DIRECTORIES( 
    ${Mengine_SOURCE_DIR}/Utils 
    ${THIRDPARTY_DIR}
    ${THIRDPARTY_DIR}/pybind/include
    ${THIRDPARTY_DIR}/xxbind/
    ${THIRDPARTY_DIR}/boost
    ${THIRDPARTY_DIR}/metabuf
    ${THIRDPARTY_DIR}/pugixml/src
    ${THIRDPARTY_DIR}/utf8/source
)


add_definitions(-D_UNICODE)

include_directories( ${Mengine_SOURCE_DIR}/Menge )

ADD_LIBRARY( ${MY_LIB_NAME} ${SRC_FILES} )

SET( APPLICATION_DEPENDENCIES
 ${APPLICATION_DEPENDENCIES}
 ${MY_LIB_NAME} 
 Kernel
 Metacode
)

SET( APPLICATION_DEPENDENCIES
 ${APPLICATION_DEPENDENCIES}
 PARENT_SCOPE
)

SET( APPLICATION_LIBS
    ${APPLICATION_LIBS}
    ${THIRDPARTY_DIR}/${MENGINE_LIB_DIR}/${CMAKE_BUILD_TYPE}/${MENGINE_LIB_PREFIX}pybind_static${MENGINE_LIB_SUFFIX}
    ${THIRDPARTY_DIR}/${MENGINE_LIB_DIR}/${CMAKE_BUILD_TYPE}/${MENGINE_LIB_PREFIX}python_static${MENGINE_LIB_SUFFIX}
    ${THIRDPARTY_DIR}/${MENGINE_LIB_DIR}/${CMAKE_BUILD_TYPE}/${MENGINE_LIB_PREFIX}zlib_static${MENGINE_LIB_SUFFIX}    
    ${THIRDPARTY_DIR}/${MENGINE_LIB_DIR}/${CMAKE_BUILD_TYPE}/${MENGINE_LIB_PREFIX}pugixml_static${MENGINE_LIB_SUFFIX}
    ${THIRDPARTY_DIR}/${MENGINE_LIB_DIR}/${CMAKE_BUILD_TYPE}/${MENGINE_LIB_PREFIX}stdex_static${MENGINE_LIB_SUFFIX}
    )

    
SET( APPLICATION_LIBS
 ${APPLICATION_LIBS}
 PARENT_SCOPE
)
