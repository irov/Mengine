MENGINE_PROJECT(Kernel)

ADD_FILTER(
Containers
    StlAllocator.h
    Array.h
    ArrayString.h
    Bitset.h
    DynamicArray.h
    Blobject.h
    String.h
    StringRegex.h
    VectorString.h
    Stringstream.h
    Vector.h
    Floats.h
    Deque.h
    Set.h
    Map.h
    Pair.h
    UnorderedMap.h
    List.h
    Tuple.h
    Regex.h
    Optional.h
    Variant.h
)

ADD_FILTER(
Mixin\\\\Unknowable
    Unknowable.h
)

ADD_FILTER(
Mixin\\\\Documentable
    Documentable.h
    Documentable.cpp
    DocumentableHelper.h
    DocumentableHelper.cpp
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
    UpdateMode.h
)

ADD_FILTER(
Mixin\\\\Renderable
    Renderable.h
    RenderContext.h
    RenderContextHelper.cpp
    RenderContextHelper.h
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
    BaseAffectorHub.h
    BaseAffectorHub.cpp
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
    EntityEventReceiverInterface.h
    EntityBehaviorInterface.h
    Entity.cpp
    Entity.h
    EntityEventable.cpp
    EntityEventable.h
    EntityHelper.cpp
    EntityHelper.h
)

ADD_FILTER(
Scene
    SceneEventReceiverInterface.h
    Scene.cpp
    Scene.h
    SceneHelper.cpp
    SceneHelper.h
)

ADD_FILTER(
Arrow
    Arrow.cpp
    Arrow.h
    ArrowInputMousePositionProvider.cpp
    ArrowInputMousePositionProvider.h
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
ZOrder
    ZOrder.cpp
    ZOrder.h
    ZOrderRender.cpp
    ZOrderRender.h
)

ADD_FILTER(
PrototypeGenerator
    BasePrototypeGenerator.cpp
    BasePrototypeGenerator.h
    FactoryPrototypeGenerator.cpp
    FactoryPrototypeGenerator.h
    DefaultPrototypeGenerator.h
    NodePrototypeGenerator.h
    BaseScriptablePrototypeGenerator.cpp
    BaseScriptablePrototypeGenerator.h
    ScriptablePrototypeGenerator.h
    ResourcePrototypeGenerator.h
    SurfacePrototypeGenerator.h
    ObjectPrototypeGenerator.h
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
    Transformable.h
    BaseTransformation.cpp
    BaseTransformation.h
)

ADD_FILTER(
Hierarchy
    Hierarchy.cpp
    Hierarchy.h
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
    CompilableReference.cpp
    CompilableReference.h
    ResourceCacher.h
    Resource.cpp
    Resource.h
    ResourcePtrView.h
    ResourceHelper.cpp
    ResourceHelper.h
)

ADD_FILTER(
Resource\\\\Image
    ResourceImage.cpp
    ResourceImage.h
    ResourceImageData.cpp
    ResourceImageData.h
    ResourceImageSolid.h
    ResourceImageSolid.cpp
    ResourceImageDefault.cpp
    ResourceImageDefault.h
    ResourceImageSubstract.cpp
    ResourceImageSubstract.h
    ResourceImageSubstractRGBAndAlpha.cpp
    ResourceImageSubstractRGBAndAlpha.h
    ResourceImageSequence.cpp
    ResourceImageSequence.h    
)

ADD_FILTER(
Resource\\\\Sound
    ResourceSound.cpp
    ResourceSound.h
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

ADD_FILTER(
Shape\\\\ShapeCircle
    ShapeCircle.cpp
    ShapeCircle.h
)

ADD_FILTER(
Shape\\\\ShapePacMan
    ShapePacMan.cpp
    ShapePacMan.h
)

ADD_FILTER(
Shape\\\\ShapeQuad
    ShapeQuad.cpp
    ShapeQuad.h
    ShapeQuadSize.cpp
    ShapeQuadSize.h
    ShapeQuadFixed.cpp
    ShapeQuadFixed.h
    ShapeQuadFlex.cpp
    ShapeQuadFlex.h
)

ADD_FILTER(
Contentable
    BaseContent.cpp
    BaseContent.h
)

ADD_FILTER(
Vertices
    FixedVertices.h
    VectorVertices.cpp
    VectorVertices.h
)

ADD_FILTER(
Thread
    ThreadEnum.h
    ThreadTask.h
    ThreadTask.cpp
    ThreadJob.h
    ThreadJob.cpp
    ThreadTaskPacket.h
    ThreadTaskPacket.cpp
    ThreadHelper.h
    ThreadHelper.cpp
    ThreadGuard.h
    ThreadGuard.cpp
    ThreadGuardScope.h
    ThreadGuardScope.cpp
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
Assertion
    Assertion.h
    Assertion.cpp
    AssertionFactory.h
    AssertionFactory.cpp
    AssertionContainer.h
    AssertionNotImplemented.h
    AssertionToDo.h
    AssertionResourceType.h
    AssertionMemoryPanic.h
    AssertionMainThreadGuard.h
    AssertionVocabulary.h
    AssertionVocabulary.cpp
    AssertionNode.h
    AssertionType.h
    AssertionAllocator.h
    AssertionObservable.h
    AssertionObservable.cpp
    AssertionOptional.h
    AssertionUnique.h
)

ADD_FILTER(
Core    
    Pool.h
    
    PoolAllocator.h
    PoolAllocator.cpp

    CRC32.h
    CRC32.cpp

    ServantBase.h
    ServiceBase.h

    FontBase.h
    FontBase.cpp

    BaseFileGroup.h
    BaseFileGroup.cpp

    Exception.h
    Exception.cpp
    
    ExceptionHelper.h
    ExceptionHelper.cpp
    
    Visitor.h

    Polygon.h
    Polygon.cpp
    PolygonHelper.h
    PolygonHelper.cpp
    
    Geolygon.h
    Geolygon.cpp
    
    Viewport.h
    Viewport.cpp
    
    ViewportHelper.h
    ViewportHelper.cpp
    
    MapAspectRatioViewports.h

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
    FilePathProxy.h
    FilePathHelper.h
    FilePathHelper.cpp
    VectorFilePath.h
    LambdaFilePath.h
    
    PathHelper.h
    PathHelper.cpp
    
    TimepipeHelper.h
    TimepipeHelper.cpp
    
    MapHelper.h
    
    StreamLogger.cpp
    StreamLogger.h
        
    FileLogger.cpp
    FileLogger.h
    
    InputServiceHelper.cpp
    InputServiceHelper.h
    
    PixelFormat.h
    PixelFormat.cpp
    
    AllocatorHelper.h
    AllocatorHelper.cpp
    
    MemoryAllocator.h
    
    MemoryStreamHelper.h
    MemoryStreamHelper.cpp

    ValueInterpolator.h
    ValueInterpolatorLinear.h
    ValueInterpolatorQuadratic.h
    ValueInterpolatorBezier.h
    ValueInterpolatorParabolic.h
    ValueAccumulator.h
    ValueAccumulateLinear.h
    ValueFollower.h
    ValueFollowerLinear.h
    ValueFollowerAcceleration.h
    
    DecimalDigitsHelper.h
    
    StringHelper.h
    StringHelper.cpp
    
    Tags.h
    Tags.cpp
    
    TagsHelper.h
    TagsHelper.cpp
    
    BufferHelper.h
    BufferHelper.cpp
    
    StringFormat.h
    StringFormat.cpp
    
    Params.h
    
    ParamsHelper.h
    ParamsHelper.cpp
    
    Rect.h
    Rect.cpp
    
    Stream.h
    Stream.cpp
    
    Pointer.h
    
    Wrapper.h
    VectorBool.h
    VectorResources.h
    
    ShuffleVector.h

    ConstString.h
    ConstString.cpp
    ConstStringHelper.h
    ConstStringHelper.cpp
    ConstStringHolder.h
    ConstStringHolder.cpp
    ConstStringHolderLocalString.h
    ConstStringHolderLocalString.cpp
    ConstStringProxy.h
    VectorConstString.h
    
    VectorResourceImages.h
    
    UID.h
    UID.cpp
    
    Base64.h
    Base64.cpp
        
    RenderUtils.h
    RenderUtils.cpp
    
    RenderPrimitive.h
    
    ThreadMutexDummy.h
    ThreadMutexDummy.cpp
    
    PluginBase.h
    PluginBase.cpp
    
    ModuleBase.h
    ModuleBase.cpp
    
    ModuleFactory.h

    FrameworkBase.h
    FrameworkBase.cpp
    
    FrameworkFactory.h
    
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
    
    IntrusivePtr.h
    IntrusivePtrBase.h
    IntrusivePtrView.h
    
    IntrusiveList.h
    IntrusiveLinked.h
    
    IntrusiveSlug.h
    IntrusiveSlugIterator.h
    IntrusiveSlugHead.h
    IntrusiveSlugListSize.h
    IntrusiveSlugList.h
    IntrusiveSlugLinkedPtr.h
    
    LoggerBase.cpp
    LoggerBase.h
    
    AspectRatioViewport.h
    VectorAspectRatioViewports.h
    
    VectorAuxScope.h
    
    ContainerWriter.h
    
    BezierHelper.h
    ParabolicHelper.h
    
    UnicodeHelper.h
    UnicodeHelper.cpp
    
    Range.h
    
    Componentable.h
    
    BaseComponentHub.h
    BaseComponentHub.cpp
    
    Hashgen.h
    Hashtable.h
    Hashtable2.h
    HashHelper.h
    
    NodeHelper.h
    MixinDebug.h
    
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
    TouchCode.h
    
    Document.h
    Document.cpp    
    DocumentHelper.h
    
    Ravingcode.h
    Ravingcode.cpp
    
    SecureValue.h
    SecureValue.cpp
    
    Stringalized.h
    Stringalized.cpp
    
    BaseDebuggerBoundingBox.h
    
    Event.h
    Delegate.h
    
    DateTime.h

    DummyGameEventReceiver.h
    DummyGameEventReceiver.cpp
    DummyEntityEventReceiver.h
    DummyEntityEventReceiver.cpp
    DummySceneEventReceiver.h
    DummySceneEventReceiver.cpp
    
    FileStreamHelper.h
    FileStreamHelper.cpp

    RandomDevice.h
    RandomDevice.cpp
    
    RandomizerHelper.h
    RandomizerHelper.cpp
    
    Crash.h
    Crash.cpp
    
    Abort.h
    Abort.cpp
    
    DebugBreak.h
    DebugBreak.cpp
    
    DevelopmentConverter.cpp
    DevelopmentConverter.h
    
    ResourcePointer.h
    UnknownPointer.h
    
    BuildMode.h
    BuildMode.cpp
    
    FileContent.h
    FileContent.cpp
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
    ErrorLevel.h
)

ADD_FILTER(
Win32
    Win32Helper.h
    Win32Helper.cpp
)

INCLUDE_DIRECTORIES(${THIRDPARTY_DIR}/boost)
INCLUDE_DIRECTORIES(${THIRDPARTY_DIR}/utf8/source)
INCLUDE_DIRECTORIES(${THIRDPARTY_DIR}/stdex/include)

if(MENGINE_USE_SCRIPT_SERVICE)
    INCLUDE_DIRECTORIES(${THIRDPARTY_DIR}/pybind/include)
endif()

ADD_MENGINE_LIBRARY()

TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${THIRDPARTY_LIB_DIR}/${MENGINE_LIB_PREFIX}math${MENGINE_LIB_SUFFIX})
TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${THIRDPARTY_LIB_DIR}/${MENGINE_LIB_PREFIX}stdex${MENGINE_LIB_SUFFIX})

if(MENGINE_USE_SCRIPT_SERVICE)
    TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${THIRDPARTY_LIB_DIR}/${MENGINE_LIB_PREFIX}pybind${MENGINE_LIB_SUFFIX})
endif()