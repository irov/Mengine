set( MY_LIB_NAME Kernel )

ADD_FILTER(
Mixin\\\\Unknowable
	Unknowable.h
)

ADD_FILTER(
Mixin\\\\Eventable
	Eventable.h
    EventableHelper.h
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
    UpdateContext.h
)

ADD_FILTER(
Mixin\\\\Renderable
	Renderable.h
    RenderContext.h
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
Mixin\\\\Pickerable
    Pickerable.h
)

ADD_FILTER(
BaseUpdation    
    BaseUpdation.cpp
    BaseUpdation.h
)

ADD_FILTER(
BasePicker
    BasePicker.cpp
    BasePicker.h
	DummyPicker.cpp
    DummyPicker.h
)

ADD_FILTER(
BaseRender
	BaseRender.cpp
	BaseRender.h
    DummyRender.cpp
    DummyRender.h
)

ADD_FILTER(
Affector
	Affector.h
	Affector.cpp
    BaseAffector.h
    BaseAffector.cpp
    LambdaAffector.h
    LambdaAffectorInterpolate.h
    LambdaAffectorAccumulateLinear.h
    AffectorCallbackInterface.h
    AffectorHelper.h
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
    NodeRenderHierarchy.cpp
    NodeRenderHierarchy.h
    NodePickerHierarchy.cpp
    NodePickerHierarchy.h
    NodeUniqueFinder.cpp
    NodeUniqueFinder.h
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
    EntityEventable.cpp
    EntityEventable.h
    EntityHelper.cpp
    EntityHelper.h
)

ADD_FILTER(
Scene
	Scene.cpp
	Scene.h
    SceneHelper.cpp
    SceneHelper.h
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
	FactoryPrototypeGenerator.cpp
	FactoryPrototypeGenerator.h
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
    AnimationEventReceiver.h
	BaseAnimation.h
    BaseAnimation.cpp
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
    Resource.cpp
    Resource.h
)

ADD_FILTER(
Resource\\\\Image
	ResourceImage.cpp
	ResourceImage.h
)

ADD_FILTER(
Resource\\\\Content
    Contentable.h
	Content.cpp
	Content.h
)

ADD_FILTER(
Vertices
	FixedVertices.h
	VectorVertices.cpp
	VectorVertices.h
)

ADD_FILTER(
ThreadTask
    ThreadEnum.h
	ThreadTask.h
	ThreadTask.cpp
    ThreadJob.h
    ThreadJob.cpp
	ThreadTaskPacket.h
	ThreadTaskPacket.cpp
)

ADD_FILTER(
Visitable
	Visitable.h
)

ADD_FILTER(
Scheduler
    SchedulerHelper.cpp
    SchedulerHelper.h
	Scheduler.cpp
	Scheduler.h
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
    AssertionFactory.h
    AssertionFactory.cpp	
	AssertionNotImplemented.h
    AssertionResourceType.h
    AssertionMemoryPanic.h
    AssertionMainThreadGuard.h
    AssertionVocabulary.h
    AssertionVocabulary.cpp
    
    Pool.h

    CRC32.h
    CRC32.cpp
	
    ServantBase.h
	ServiceBase.h    
	
	FontBase.h
	FontBase.cpp
    
    Exception.h
    Visitor.h
    
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

	Color.h
	Color.cpp
        
    GlyphCode.h
    
    IniHelper.h
    IniHelper.cpp
    
    HashType.h
   
    PathString.h
   
    FilePath.h
    FilePath.cpp
    FilePathHelper.h
    FilePathHelper.cpp
    VectorFilePath.h
    LambdaFilePath.h
    
    PathHelper.h
    PathHelper.cpp
    
    StreamLogger.cpp
    StreamLogger.h
        
    FileLogger.cpp
    FileLogger.h
    
    InputServiceHelper.cpp
    InputServiceHelper.h
    
    PixelFormat.h
    PixelFormat.cpp
    
    MemoryAllocator.h
    
    MemoryHelper.h
    MemoryHelper.cpp
	
    ValueInterpolator.h
    ValueFollower.h
    ValueFollowerLinear.h
    ValueFollowerAcceleration.h
    
    StringHelper.h
	StringHelper.cpp
    
    Tags.h
    Tags.cpp
    
    TagsHelper.h
    TagsHelper.cpp
    
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
    ConstStringHelper.h
    ConstStringHelper.cpp
	ConstStringHolder.h
    ConstStringHolder.cpp
	ConstStringHolderLocalString.h
	ConstStringHolderLocalString.cpp
	VectorConstString.h
    
    VectorResourceImages.h
    
    UID.h
    UID.cpp        
        
    RenderUtils.h
	RenderUtils.cpp
    
    OutputStreamWriter.h
        
    ThreadMutexDummy.h
    ThreadMutexDummy.cpp
    
    PluginBase.h
    PluginBase.cpp
    
    ModuleBase.h
    ModuleBase.cpp
    
    ModuleFactory.h
    
    Holder.h
    
    RenderVertex2D.h
    RenderIndex.h
    
    VectorRenderVertex2D.h
    VectorRenderIndex.h
   
    Magic.h    
    Mixin.h
    MixinPointer.h
    Magicable.h
	
	Callback.h
    
    MixerValue.h
    MixerValue.cpp
    
    Typename.h
    SHA1.h
    
    IntrusivePtr.h
    IntrusivePtrBase.h
    
    IntrusiveSlug.h
    IntrusiveSlugIterator.h
    IntrusiveSlugHead.h
    IntrusiveSlugListSize.h
    IntrusiveSlugList.h    
    
    LoggerBase.cpp
    LoggerBase.h
    
    AspectRatioViewport.h
	VectorAspectRatioViewports.h
    
    VectorAuxScope.h
    
    BezierHelper.h
    ParabolicHelper.h
    
    Range.h
    
    Hashgen.h
    Hashtable.h
    HashHelper.h
    
    ThreadMutexScope.h
    ThreadMutexScope.cpp
    
    Dataflow.h
    Dataflow.cpp

    DataPointer.h
    
    StringArguments.h
    StringArguments.cpp
    
    GlobalInputHandlerHelper.h
    GlobalInputHandlerHelper.cpp

    LoggerLevel.h
    KeyCode.h
    MouseCode.h
    
    Document.h
    
    Ravingcode.h
    Ravingcode.cpp
    
    SecureValue.h
    SecureValue.cpp
    
    Stringalized.h
    Stringalized.cpp
    
    Event.h
    Delegate.h
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
)

ADD_FILTER(
Factory
    Factorable.h
    Factorable.cpp
    FactorablePointer.h
    FactorableUnique.h
    FactorablePlugin.h
	Factory.h
	Factory.cpp
    FactoryWithListener.h
    FactoryWithListener.cpp
    FactoryWithMutex.h
    FactoryWithMutex.cpp	
    FactoryDefault.h    
    FactoryPool.h
    FactoryPoolWithListener.h
)

ADD_FILTER(
Logger
	Logger.h
	Logger.cpp
)

ADD_FILTER(
Error
	Error.h
	Error.cpp
)

INCLUDE_DIRECTORIES( ${THIRDPARTY_DIR}/boost )
INCLUDE_DIRECTORIES( ${THIRDPARTY_DIR}/utf8/source )
INCLUDE_DIRECTORIES( ${THIRDPARTY_DIR}/libmath/include )
INCLUDE_DIRECTORIES( ${THIRDPARTY_DIR}/pybind/include )
INCLUDE_DIRECTORIES( ${THIRDPARTY_DIR}/stdex/include )

ADD_MENGINE_LIBRARY()
