set( MY_LIB_NAME Kernel )

ADD_FILTER(
Servant
    Servant.h
    Servant.cpp
)

ADD_FILTER(
Mixin\\\\Unknowable
	Unknowable.h
)

ADD_FILTER(
Mixin\\\\Eventable
	Eventable.h
    BaseEventation.h
    BaseEventation.cpp
)

ADD_FILTER(
Mixin\\\\Scriptable
	Scriptable.cpp
	Scriptable.h
)

ADD_FILTER(
Mixin\\\\Animatable
	Animatable.h
)

ADD_FILTER(
Mixin\\\\Soundable
	Soundable.cpp
	Soundable.h
)

ADD_FILTER(
Mixin\\\\Updatable
	Updatable.h
)

ADD_FILTER(
Mixin\\\\Renderable
	Renderable.h
)

ADD_FILTER(
Mixin\\\\Colorable
	Colorable.cpp
	Colorable.h
)

ADD_FILTER(
Mixin\\\\Affectorable
	Affectorable.cpp
	Affectorable.h
)

ADD_FILTER(
Mixin\\\\Materialable
	Materialable.cpp
	Materialable.h
)

ADD_FILTER(
Mixin\\\\Observable
	Observable.h
)

ADD_FILTER(
BaseUpdation    
    BaseUpdation.cpp
    BaseUpdation.h
)

ADD_FILTER(
BaseRender
	BaseRender.cpp
	BaseRender.h
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
    NodeRenderHelper.cpp
    NodeRenderHelper.h
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
Interender
    Interender.cpp
    Interender.h
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
Transformation
	Transformation.cpp
	Transformation.h
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
Core
    Assertion.h
    Assertion.cpp

    CRC32.h
    CRC32.cpp
	
	ServantBase.h
	ServiceBase.h
	
	FontBase.h
	FontBase.cpp
    
    Date.h
    Date.cpp
    
    Exception.h
    Visitor.h
    
    RenderVisitor.h
    RenderVisitor.cpp
    
    Polygon.h
    Polygon.cpp
    PolygonHelper.h
    PolygonHelper.cpp
    
    Geolygon.h
    Geolygon.cpp
    
	Viewport.h
	Viewport.cpp
	
	Resolution.h
	Resolution.cpp	

	ColourValue.h
	ColourValue.cpp
        
    GlyphChar.h
    
    IniUtil.h
    IniUtil.cpp
    
    Hash.h
   
    FilePath.h
    FilePath.cpp
    
    FileLogger.cpp
    FileLogger.h
    
    PixelFormat.h
    PixelFormat.cpp
    
    MemoryAllocator.h
    
    MemoryHelper.h
    MemoryHelper.cpp
	
    ValueInterpolator.h
    ValueFollower.h
    
    String.h
	String.cpp
    
    Tags.h
    Tags.cpp
    
    StringFormat.h
	StringFormat.cpp
    
    Params.h
    
	Rect.h
	Rect.cpp
    
    Stream.h
    Stream.cpp
    
    Pointer.h
	
    ConstString.h
    ConstString.cpp
	ConstStringHolder.h
    ConstStringHolder.cpp
	ConstStringHolderLocalString.h
	ConstStringHolderLocalString.cpp
	ConstStringTypes.h
    
    UID.h
    UID.cpp        
        
    RenderUtils.h
	RenderUtils.cpp
    
    OutputStreamWriter.h   
    
    IntrusiveTree.h
    
    PluginBase.h
    PluginBase.cpp
    
    ModuleBase.h
    ModuleBase.cpp
    
    ModuleFactory.h
    
    Holder.h
    
    RenderVertex2D.h
    RenderIndex.h
   
    Magic.h    
    Mixin.h
	
	Callback.h
    
    MixerValue.h
    MixerValue.cpp
    
    Typename.h
    SHA1.h
    
    IntrusivePtr.h
    IntrusivePtrBase.h
    
    LoggerBase.cpp
    LoggerBase.h
    
    AspectRatioViewports.h
    
    Range.h
)

ADD_FILTER(
Unknown
    UnknownResourceImageDataInterface.h
)

ADD_FILTER(
Codec
    Decoder.h
    Encoder.h
    PickEncoder.cpp
    PickEncoder.h
    PickDecoder.cpp
    PickDecoder.h
    VideoDecoder.h
    VideoDecoder.cpp
    SoundDecoder.h
    SoundDecoder.cpp
    ImageDecoder.h
    ImageDecoder.cpp
    ImageDecoderMemory.h
    ImageDecoderMemory.cpp
    ImageDecoderArchive.h
    ImageDecoderArchive.cpp
    ImageEncoder.cpp
    ImageEncoder.h        

    DecoderFactory.h
    EncoderFactory.h    
    ConverterFactory.h
    DataflowFactory.h
)

ADD_FILTER(
Factory
    Factorable.h
    Factorable.cpp  
	FactorablePointer.h
    FactorableUnique.h
	Factory.h
	Factory.cpp
    FactoryAssertion.h
    FactoryAssertion.cpp
    FactoryWithListener.h
    FactoryWithListener.cpp
    FactoryWithMutex.h
    FactoryWithMutex.cpp	
    FactoryDefault.h    
    FactoryPool.h
    FactoryPoolWithListener.h
)

ADD_FILTER(
Archive
	Archive.h
	ArchiveWrite.h
	ArchiveWrite.cpp
)

ADD_FILTER(
Logger
	Logger.h
	Logger.cpp
)

ADD_FILTER(
Visitors
    DebugRenderVisitor.h
    DebugRenderVisitor.cpp
)

INCLUDE_DIRECTORIES(
    ${SOURCE_DIR}
    ${SOURCE_DIR}/Utils
    ${THIRDPARTY_DIR}/boost/libs/config/include
    ${THIRDPARTY_DIR}/boost/libs/assert/include
    ${THIRDPARTY_DIR}/boost/libs/preprocessor/include
    ${THIRDPARTY_DIR}/boost/libs/predef/include
    ${THIRDPARTY_DIR}/boost/libs/numeric/conversion/include
    ${THIRDPARTY_DIR}/boost/libs/type_traits/include
    ${THIRDPARTY_DIR}/boost/libs/mpl/include
    ${THIRDPARTY_DIR}/boost/libs/math/include
    ${THIRDPARTY_DIR}/boost/libs/iterator/include
    ${THIRDPARTY_DIR}/boost/libs/range/include
    ${THIRDPARTY_DIR}/boost/libs/static_assert/include
    ${THIRDPARTY_DIR}/boost/libs/throw_exception/include
    ${THIRDPARTY_DIR}/boost/libs/throw_detail/include
    ${THIRDPARTY_DIR}/boost/libs/core/include
    ${THIRDPARTY_DIR}/boost/libs/detail/include
    ${THIRDPARTY_DIR}/boost/libs/utility/include
    ${THIRDPARTY_DIR}/boost/libs/integer/include
    ${THIRDPARTY_DIR}/boost/libs/array/include
    ${THIRDPARTY_DIR}/boost/libs/smart_ptr/include
    ${THIRDPARTY_DIR}/boost/libs/concept_check/include
    ${THIRDPARTY_DIR}/boost/libs/move/include
    ${THIRDPARTY_DIR}/boost/libs/format/include
    ${THIRDPARTY_DIR}/boost/libs/optional/include
    ${THIRDPARTY_DIR}/boost/libs/container/include
    ${THIRDPARTY_DIR}/boost/libs/variant/include
    ${THIRDPARTY_DIR}/boost/libs/tuple/include
    ${THIRDPARTY_DIR}/boost/libs/rational/include
    ${THIRDPARTY_DIR}/boost/libs/algorithm/include
    ${THIRDPARTY_DIR}/boost/libs/function/include
    ${THIRDPARTY_DIR}/boost/libs/type_index/include
    ${THIRDPARTY_DIR}/boost/libs/container_hash/include
    ${THIRDPARTY_DIR}/boost/libs/bind/include
    ${THIRDPARTY_DIR}/boost/libs/function_types/include
    ${THIRDPARTY_DIR}/boost/libs/intrusive/include
    ${THIRDPARTY_DIR}/boost/libs/tokenizer/include
    ${THIRDPARTY_DIR}/boost/libs/geometry/include
    ${THIRDPARTY_DIR}/boost/libs/lexical_cast/include
    ${THIRDPARTY_DIR}/boost/libs/qvm/include
    ${THIRDPARTY_DIR}/boost/libs/exception/include
    ${THIRDPARTY_DIR}/utf8/source
    ${THIRDPARTY_DIR}/tinyini/include
    ${THIRDPARTY_DIR}/metabuf
    ${THIRDPARTY_DIR}/libmath/include
    ${THIRDPARTY_DIR}/pybind/include
    )     

ADD_MENGINE_LIBRARY()

TARGET_LINK_LIBRARIES(${MY_LIB_NAME} ${THIRDPARTY_LIB_DIR}/${MENGINE_LIB_PREFIX}pybind${MENGINE_LIB_SUFFIX})

SET( APPLICATION_DEPENDENCIES
 ${APPLICATION_DEPENDENCIES}
 ${MY_LIB_NAME}
)


SET( APPLICATION_DEPENDENCIES
 ${APPLICATION_DEPENDENCIES}
 PARENT_SCOPE
)
