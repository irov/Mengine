#include "NodeScriptEmbedding.h"

#include "Interface/ApplicationInterface.h"
#include "Interface/TimelineServiceInterface.h"
#include "Interface/RenderSystemInterface.h"
#include "Interface/InputServiceInterface.h"
#include "Interface/PrototypeServiceInterface.h"
#include "Interface/MemoryInterface.h"
#include "Interface/PlatformInterface.h"
#include "Interface/PackageInterface.h"
#include "Interface/SceneServiceInterface.h"
#include "Interface/VocabularyServiceInterface.h"
#include "Interface/ScriptServiceInterface.h"
#include "Interface/SchedulerInterface.h"
#include "Interface/TextFontInterface.h"
#include "Interface/AccountInterface.h"
#include "Interface/ResourceServiceInterface.h"
#include "Interface/TextServiceInterface.h"
#include "Interface/FontServiceInterface.h"

#include "Kernel/Reference.h"
#include "Kernel/Eventable.h"
#include "Kernel/ThreadTask.h"
#include "Kernel/Scene.h"
#include "Kernel/Arrow.h"
#include "Kernel/NodeRenderHierarchy.h"
#include "Kernel/MatrixProxy.h"
#include "Kernel/AssertionMemoryPanic.h"
#include "Kernel/ConstStringHelper.h"
#include "Kernel/UnicodeHelper.h"

#include "Engine/ResourceFile.h"
#include "Engine/ResourceMusic.h"
#include "Engine/ResourceSound.h"
#include "Engine/ResourceTestPick.h"
#include "Engine/ResourceHIT.h"
#include "Engine/ResourceShape.h"
#include "Engine/ResourceCursorICO.h"
#include "Engine/ResourceCursorSystem.h"

#include "Plugins/MoviePlugin/ResourceMovie2.h"

#include "Interface/ApplicationInterface.h"

#include "Engine/HotSpot.h"
#include "Engine/HotSpotPolygon.h"
#include "Engine/HotSpotGlobal.h"
#include "Engine/HotSpotCircle.h"
#include "Engine/HotSpotBubbles.h"
#include "Engine/HotSpotImage.h"
#include "Engine/HotSpotResourceShape.h"
#include "Engine/HotSpotSurface.h"
#include "Engine/Landscape2D.h"
#include "Engine/Grid2D.h"

#include "Engine/ShapeCircle.h"
#include "Engine/ShapePacMan.h"
#include "Engine/ShapeQuadFixed.h"
#include "Engine/ShapeQuadFlex.h"

#include "Engine/Gyroscope.h"
#include "Engine/TextField.h"
#include "Engine/SoundEmitter.h"
#include "Engine/Point.h"
#include "Engine/Line.h"
#include "Kernel/RenderCameraProjection.h"

#include "ScriptHolder.h"

#include "Environment/Python/PythonAnimatableEventReceiver.h"
#include "Environment/Python/PythonEventReceiver.h"
#include "Environment/Python/PythonDocumentTraceback.h"

#include "Engine/SurfaceSound.h"
#include "Engine/SurfaceImage.h"
#include "Engine/SurfaceImageSequence.h"
#include "Engine/SurfaceTrackMatte.h"
#include "Engine/SurfaceSolidColor.h"

#include "Kernel/Interender.h"
#include "Kernel/RenderViewport.h"
#include "Kernel/RenderScissor.h"
#include "Kernel/RenderCameraOrthogonal.h"
#include "Kernel/RenderCameraProjection.h"
#include "Kernel/RenderCameraOrthogonalTarget.h"
#include "Kernel/ResourceImage.h"
#include "Kernel/ResourceImageData.h"
#include "Kernel/ResourceImageSequence.h"
#include "Kernel/ResourceImageSolid.h"
#include "Kernel/ResourceImageDefault.h"
#include "Kernel/ResourceImageSubstractRGBAndAlpha.h"
#include "Kernel/ResourceImageSubstract.h"

#include "Engine/Layer2D.h"
#include "Engine/Meshget.h"
#include "Engine/Isometric.h"
#include "Engine/Window.h"

#include "Kernel/Shape.h"
#include "Kernel/Entity.h"
#include "Kernel/Logger.h"

#include "Interface/RenderSystemInterface.h"

#include "Kernel/Identity.h"
#include "Kernel/Affector.h"
#include "Kernel/AffectorHelper.h"
#include "Kernel/ThreadTask.h"

#include "Environment/Python/PythonScriptWrapper.h"

#include "ScriptableAffectorCallback.h"
#include "PythonEntityBehavior.h"
#include "PythonHotSpotEventReceiver.h"
#include "PythonScheduleTiming.h"
#include "PythonSchedulePipe.h"
#include "PythonScheduleEvent.h"
#include "DelaySchedulePipe.h"

#include "Kernel/Polygon.h"
#include "Kernel/MemoryStreamHelper.h"
#include "Kernel/ValueFollower.h"

#include "math/angle.h"
#include "math/vec4.h"
#include "math/mat3.h"
#include "math/mat4.h"
#include "math/quat.h"
#include "math/utils.h"

#include "Kernel/Rect.h"
#include "Kernel/Polygon.h"
#include "Kernel/ValueFollower.h"
#include "Kernel/ValueInterpolatorParabolic.h"

#include "Kernel/FactoryPool.h"
#include "Kernel/AssertionFactory.h"

#include "utf8.h"

namespace Mengine
{
    namespace
    {
        class NodeScriptMethod
            : public Factorable
        {
        public:
            NodeScriptMethod()
            {
            }

            ~NodeScriptMethod() override
            {
            }

        public:
            bool initialize()
            {
                return true;
            }

            void finalize()
            {
            }

        public:
            //////////////////////////////////////////////////////////////////////////
            VectorString cs_args;
            //////////////////////////////////////////////////////////////////////////
            PyObject * s_TextField_setTextFormatArgs( pybind::kernel_interface * _kernel, TextField * _textField, PyObject * _args, PyObject * _kwds )
            {
                MENGINE_UNUSED( _kwds );

                size_t args_count = _kernel->tuple_size( _args );

                cs_args.clear();
                cs_args.reserve( args_count );

                for( uint32_t it = 0; it != args_count; ++it )
                {
                    PyObject * py_string = _kernel->tuple_getitem( _args, it );

                    if( _kernel->string_check( py_string ) == true )
                    {
                        String key;
                        if( pybind::extract_value( _kernel, py_string, key, false ) == false )
                        {
                            LOGGER_ERROR( "'%s' invalid get str '%s'"
                                , _textField->getName().c_str()
                                , _kernel->object_repr( py_string ).c_str()
                            );

                            return _kernel->ret_false();
                        }

                        cs_args.emplace_back( key );
                    }
                    else if( _kernel->unicode_check( py_string ) == true )
                    {
                        WString key;
                        if( pybind::extract_value( _kernel, py_string, key, false ) == false )
                        {
                            LOGGER_ERROR( "'%s' invalid get unicode '%s'"
                                , _textField->getName().c_str()
                                , _kernel->object_repr( py_string ).c_str()
                            );

                            return _kernel->ret_false();
                        }

                        String utf8_arg;
                        Helper::unicodeToUtf8( key, &utf8_arg );

                        cs_args.emplace_back( utf8_arg );
                    }
                    else
                    {
                        pybind::string_view py_value_str = _kernel->object_str( py_string );

                        if( py_value_str.is_invalid() == true )
                        {
                            LOGGER_ERROR( "'%s' not suport arg '%s'"
                                , _textField->getName().c_str()
                                , _kernel->object_repr( py_string ).c_str()
                            );

                            return _kernel->ret_false();
                        }

                        const Char * value_str = py_value_str.c_str();

                        cs_args.emplace_back( value_str );
                    }
                }

                _textField->setTextFormatArgs( cs_args );

                return _kernel->ret_true();
            }
            //////////////////////////////////////////////////////////////////////////
            VectorWString s_TextField_getTextFormatArgs( TextField * _textField )
            {
                VectorWString ws_args;

                const VectorString & str_args = _textField->getTextFormatArgs();

                size_t args_count = str_args.size();
                ws_args.reserve( args_count );

                for( const String & str_arg : str_args )
                {
                    WString unicode;
                    Helper::utf8ToUnicode( str_arg, &unicode );

                    ws_args.emplace_back( unicode );
                }

                return ws_args;
            }
            //////////////////////////////////////////////////////////////////////////
            void s_TextField_setFontName( TextField * _textField, const ConstString & _fontName )
            {
                const TextFontInterfacePtr & font = FONT_SERVICE()
                    ->getFont( _fontName );

                _textField->setFont( font );
            }
            //////////////////////////////////////////////////////////////////////////
            const ConstString & s_TextField_getFontName( TextField * _textField )
            {
                const TextFontInterfacePtr & font = _textField->getFont();

                const ConstString & fontName = font->getName();

                return fontName;
            }
            //////////////////////////////////////////////////////////////////////////
            mt::box2f s_BoundingBox_getBoundingBox( BoundingBox * _boundingBox )
            {
                const mt::box2f * bb = _boundingBox->getBoundingBox();

                if( bb == nullptr )
                {
                    mt::box2f infinity_bb;
                    mt::infinity_box( infinity_bb );

                    return infinity_bb;
                }

                return *bb;
            }
            //////////////////////////////////////////////////////////////////////////
            void s_Transformation_setAngleDeg( TransformationInterface * _transformation, float _angle )
            {
                float rad = _angle * mt::constant::deg2rad;

                _transformation->setLocalOrientationX( rad );
            }
            //////////////////////////////////////////////////////////////////////////
            void s_Transformation_removeRelationTransformation( TransformationInterface * _transformation )
            {
                _transformation->removeRelationTransformation();
            }
            //////////////////////////////////////////////////////////////////////////
            void Transformation_coordinate( TransformationInterface * _transformation, const mt::vec3f & _coordinate )
            {
                const mt::vec3f & origin = _transformation->getLocalOrigin();
                _transformation->setLocalOrigin( origin + _coordinate );
                _transformation->translate( _coordinate );
            }
            //////////////////////////////////////////////////////////////////////////
            mt::vec2f s_HotSpotPolygon_getLocalPolygonCenter( HotSpotPolygon * _hs )
            {
                const Polygon & polygon = _hs->getPolygon();

                mt::box2f bb;
                polygon.to_box2f( &bb );

                mt::vec2f c;
                mt::get_center_box( bb, c );

                return c;
            }
            //////////////////////////////////////////////////////////////////////////
            mt::vec2f s_HotSpotPolygon_getWorldPolygonCenter( HotSpotPolygon * _hs )
            {
                const Polygon & polygon = _hs->getPolygon();

                mt::box2f bb;
                mt::insideout_box( bb );

                const TransformationInterface * transformation = _hs->getTransformation();

                const mt::mat4f & wm = transformation->getWorldMatrix();

                for( const mt::vec2f & v : polygon )
                {
                    mt::vec2f v_wm;
                    mt::mul_v2_v2_m4( v_wm, v, wm );

                    mt::add_internal_point( bb, v_wm );
                }

                mt::vec2f c;
                mt::get_center_box( bb, c );

                return c;
            }
            //////////////////////////////////////////////////////////////////////////
            mt::vec2f s_HotSpotPolygon_getScreenPolygonCenter( HotSpotPolygon * _hs )
            {
                const Resolution & contentResolution = APPLICATION_SERVICE()
                    ->getContentResolution();

                const RenderCameraInterfacePtr & camera = Helper::getNodeRenderCameraInheritance( _hs );
                const RenderViewportInterfacePtr & viewport = Helper::getNodeRenderViewportInheritance( _hs );

                MENGINE_ASSERTION_MEMORY_PANIC( camera, "invalid get node '%s' render camera inheritance"
                    , _hs->getName().c_str()
                );

                MENGINE_ASSERTION_MEMORY_PANIC( viewport, "invalid get node '%s' render viewport inheritance"
                    , _hs->getName().c_str()
                );

                RenderContext context;
                context.camera = camera.get();
                context.viewport = viewport.get();
                context.transformation = nullptr;
                context.scissor = nullptr;
                context.target = nullptr;

                mt::box2f b1;
                _hs->getScreenPolygon( &context, contentResolution, &b1, nullptr );

                mt::vec2f c;
                mt::get_center_box( b1, c );

                return c;
            }
            //////////////////////////////////////////////////////////////////////////
            Polygon s_HotSpotPolygon_getWorldPolygon( HotSpotPolygon * _hs )
            {
                const Polygon & polygon = _hs->getPolygon();

                const TransformationInterface * transformation = _hs->getTransformation();

                const mt::mat4f & wm = transformation->getWorldMatrix();

                Polygon pwm;
                polygon.mul_wm( &pwm, wm );

                return pwm;
            }
            //////////////////////////////////////////////////////////////////////////
            const mt::vec2f & s_Shape_getSurfaceSize( Shape * _shape )
            {
                const SurfacePtr & surface = _shape->getSurface();

                MENGINE_ASSERTION_MEMORY_PANIC( surface, "shape '%s' not setup surface"
                    , _shape->getName().c_str()
                );

                const mt::vec2f & size = surface->getSize();

                return size;
            }
            //////////////////////////////////////////////////////////////////////////
            mt::vec2f s_Shape_getLocalImageCenter( Shape * _shape )
            {
                const SurfacePtr & surface = _shape->getSurface();

                MENGINE_ASSERTION_MEMORY_PANIC( surface, "shape '%s' not setup surface"
                    , _shape->getName().c_str()
                );

                const mt::vec2f & offset = surface->getOffset();
                const mt::vec2f & size = surface->getSize();

                mt::vec2f center = offset + size * 0.5f;

                return center;
            }
            //////////////////////////////////////////////////////////////////////////
            mt::vec2f s_Shape_getWorldImageCenter( Shape * _shape )
            {
                mt::vec2f imageCenter = s_Shape_getLocalImageCenter( _shape );

                const TransformationInterface * transformation = _shape->getTransformation();

                const mt::mat4f & wm = transformation->getWorldMatrix();

                mt::vec2f imageCenter_wm;
                mt::mul_v2_v2_m4( imageCenter_wm, imageCenter, wm );

                return imageCenter_wm;
            }
            //////////////////////////////////////////////////////////////////////////
            IntrusivePtr<NodeAffectorCreator::NodeAffectorCreatorInterpolateLinear<mt::vec4f>> m_nodeAffectorCreatorInterpolateLinearVec4;
            //////////////////////////////////////////////////////////////////////////
            FactoryPtr m_factoryNodeAffectorCallback;
            //////////////////////////////////////////////////////////////////////////
            ScriptableAffectorCallbackPtr createNodeAffectorCallback( Scriptable * _scriptable, const pybind::object & _cb, const pybind::args & _args )
            {
                ScriptableAffectorCallbackPtr callback = m_factoryNodeAffectorCallback->createObject( MENGINE_DOCUMENT_PYBIND );

                callback->initialize( ScriptablePtr( _scriptable ), _cb, _args );

                return callback;
            }
            //////////////////////////////////////////////////////////////////////////
            uint32_t s_ShapeQuadFlex_setPercentVisibilityTo( ShapeQuadFlex * _shape, float _time, const mt::vec4f & _percent, const ConstString & _easingType, const pybind::object & _cb, const pybind::args & _args )
            {
                if( _shape->isActivate() == false )
                {
                    return 0;
                }

                EasingInterfacePtr easing = VOCABULARY_GET( STRINGIZE_STRING_LOCAL( "Easing" ), _easingType );

                ScriptableAffectorCallbackPtr callback = createNodeAffectorCallback( _shape, _cb, _args );

                AffectorPtr affector = m_nodeAffectorCreatorInterpolateLinearVec4->create( ETA_VISIBILITY
                    , easing
                    , callback
                    , [_shape]( const mt::vec4f & _v )
                {
                    _shape->setPercentVisibility( _v );
                }
                    , _shape->getPercentVisibility(), _percent, _time
                    , MENGINE_DOCUMENT_PYBIND
                    );

                MENGINE_ASSERTION_MEMORY_PANIC( affector, "invalid create affector" );

                const AffectorHubInterfacePtr & affectorHub = _shape->getAffectorHub();

                affectorHub->stopAffectors( ETA_VISIBILITY );

                if( _shape->isActivate() == false )
                {
                    return 0;
                }

                UniqueId id = affectorHub->addAffector( affector );

                return id;
            }
            //////////////////////////////////////////////////////////////////////////
            void s_ShapeQuadFlex_setPercentVisibilityStop( ShapeQuadFlex * _shape )
            {
                const AffectorHubInterfacePtr & affectorHub = _shape->getAffectorHub();

                MENGINE_ASSERTION_MEMORY_PANIC( affectorHub );

                affectorHub->stopAffectors( ETA_VISIBILITY );
            }
            //////////////////////////////////////////////////////////////////////////
            class PythonSceneChangeCallback
                : public SceneChangeCallbackInterface
            {
            public:
                PythonSceneChangeCallback()
                {
                }

                ~PythonSceneChangeCallback() override
                {
                }

            public:
                void initialize( const pybind::object & _cb, const pybind::args & _args )
                {
                    m_cb = _cb;
                    m_args = _args;
                }

            public:
                void onSceneChange( const ScenePtr & _scene, bool _enable, bool _remove, bool _error ) override
                {
                    MENGINE_UNUSED( _error );

                    if( _remove == false )
                    {
                        if( _scene == nullptr )
                        {
                            m_cb.call_args( nullptr, _enable, m_args );
                        }
                        else
                        {
                            const PythonEntityBehaviorPtr & behavior = _scene->getBehavior();
                            const pybind::object & py_scene = behavior->getScriptObject();

                            m_cb.call_args( py_scene, _enable, m_args );
                        }
                    }
                    else
                    {
                        m_cb.call_args( m_args );
                    }
                }

            public:
                pybind::object m_cb;
                pybind::args m_args;
            };
            //////////////////////////////////////////////////////////////////////////
            typedef IntrusivePtr<PythonSceneChangeCallback> PythonSceneChangeCallbackPtr;
            //////////////////////////////////////////////////////////////////////////
            FactoryPtr m_factoryPythonSceneChangeCallback;
            //////////////////////////////////////////////////////////////////////////
            uint32_t s_Animation_play( AnimationInterface * _animation )
            {
                float time = TIMELINE_SERVICE()
                    ->getTotalTime();

                uint32_t id = _animation->play( time );

                return id;
            }
            //////////////////////////////////////////////////////////////////////////
            void s_Animation_resume( AnimationInterface * _animation )
            {
                float time = TIMELINE_SERVICE()
                    ->getTotalTime();

                _animation->resume( time );
            }
            //////////////////////////////////////////////////////////////////////////
            PyObject * s_SurfaceImageSequence_setEventListener( pybind::kernel_interface * _kernel, SurfaceImageSequence * _surface, PyObject * _args, PyObject * _kwds )
            {
                MENGINE_UNUSED( _args );

                MENGINE_ASSERTION_MEMORY_PANIC( _kwds, "invalid set event listener" );

                pybind::dict py_kwds( _kernel, _kwds );
                Helper::registerAnimatableEventReceiver<>( _kernel, py_kwds, _surface, MENGINE_DOCUMENT_PYBIND );

                MENGINE_ASSERTION_PYTHON_EVENT_RECEIVER( _surface, py_kwds );

                return _kernel->ret_none();
            }
            //////////////////////////////////////////////////////////////////////////
            PyObject * s_SurfaceSound_setEventListener( pybind::kernel_interface * _kernel, SurfaceSound * _surface, PyObject * _args, PyObject * _kwds )
            {
                MENGINE_UNUSED( _args );

                MENGINE_ASSERTION_MEMORY_PANIC( _kwds, "invalid set event listener" );

                pybind::dict py_kwds( _kernel, _kwds );
                Helper::registerAnimatableEventReceiver<>( _kernel, py_kwds, _surface, MENGINE_DOCUMENT_PYBIND );

                MENGINE_ASSERTION_PYTHON_EVENT_RECEIVER( _surface, py_kwds );

                return _kernel->ret_none();
            }
            //////////////////////////////////////////////////////////////////////////
            class PythonMeshEventReceiver
                : public PythonEventReceiver
                , public MeshgetEventReceiverInterface
                , public Factorable
            {
            public:
                PythonMeshEventReceiver()
                {
                }

                ~PythonMeshEventReceiver() override
                {
                }

            public:
                void onMeshgetUpdate( const UpdateContext * _context ) override
                {
                    m_cb.call( _context->revision, _context->current, _context->time );
                }
            };
            //////////////////////////////////////////////////////////////////////////
            PyObject * s_Meshget_setEventListener( pybind::kernel_interface * _kernel, Meshget * _node, PyObject * _args, PyObject * _kwds )
            {
                MENGINE_UNUSED( _args );

                MENGINE_ASSERTION_MEMORY_PANIC( _kwds, "invalid set event listener" );

                pybind::dict py_kwds( _kernel, _kwds );
                Helper::registerPythonEventReceiver<PythonMeshEventReceiver>( _kernel, py_kwds, _node, "onMeshgetUpdate", EVENT_MESHGET_UPDATE, MENGINE_DOCUMENT_PYBIND );

                MENGINE_ASSERTION_PYTHON_EVENT_RECEIVER( _node, py_kwds );

                return _kernel->ret_none();
            }
            //////////////////////////////////////////////////////////////////////////
            class PythonScriptHolderEventReceiver
                : public PythonEventReceiver
                , public ScriptHolderEventReceiverInterface
                , public Factorable
            {
            public:
                PythonScriptHolderEventReceiver()
                {
                }

                ~PythonScriptHolderEventReceiver() override
                {
                }

            public:
                pybind::object onScriptHolderKeepScript() override
                {
                    return m_cb.call();
                }

                void onScriptHolderReleaseScript( const pybind::object & _script ) override
                {
                    m_cb.call( _script );
                }
            };
            //////////////////////////////////////////////////////////////////////////
            PyObject * s_ScriptHolder_setEventListener( pybind::kernel_interface * _kernel, ScriptHolder * _node, PyObject * _args, PyObject * _kwds )
            {
                MENGINE_UNUSED( _args );

                MENGINE_ASSERTION_MEMORY_PANIC( _kwds, "invalid set event listener" );

                pybind::dict py_kwds( _kernel, _kwds );
                Helper::registerPythonEventReceiver<PythonScriptHolderEventReceiver>( _kernel, py_kwds, _node, "onKeepScript", EVENT_SCRIPT_HOLDER_KEEP, MENGINE_DOCUMENT_PYBIND );
                Helper::registerPythonEventReceiver<PythonScriptHolderEventReceiver>( _kernel, py_kwds, _node, "onReleaseScript", EVENT_SCRIPT_HOLDER_RELEASE, MENGINE_DOCUMENT_PYBIND );

                MENGINE_ASSERTION_PYTHON_EVENT_RECEIVER( _node, py_kwds );

                return _kernel->ret_none();
            }
            //////////////////////////////////////////////////////////////////////////
            PyObject * s_HotSpot_setEventListener( pybind::kernel_interface * _kernel, HotSpot * _node, PyObject * _args, PyObject * _kwds )
            {
                MENGINE_UNUSED( _args );

                MENGINE_ASSERTION_MEMORY_PANIC( _kwds, "invalid set event listener" );

                pybind::dict py_kwds( _kernel, _kwds );

                Helper::registerPythonEventReceiver<PythonHotSpotEventReceiver>( _kernel, py_kwds, _node, "onHandleKeyEvent", EVENT_HOTSPOT_KEY, MENGINE_DOCUMENT_PYBIND );
                Helper::registerPythonEventReceiver<PythonHotSpotEventReceiver>( _kernel, py_kwds, _node, "onHandleMouseButtonEvent", EVENT_HOTSPOT_MOUSE_BUTTON, MENGINE_DOCUMENT_PYBIND );
                Helper::registerPythonEventReceiver<PythonHotSpotEventReceiver>( _kernel, py_kwds, _node, "onHandleMouseButtonEventBegin", EVENT_HOTSPOT_MOUSE_BUTTON_BEGIN, MENGINE_DOCUMENT_PYBIND );
                Helper::registerPythonEventReceiver<PythonHotSpotEventReceiver>( _kernel, py_kwds, _node, "onHandleMouseButtonEventEnd", EVENT_HOTSPOT_MOUSE_BUTTON_END, MENGINE_DOCUMENT_PYBIND );
                Helper::registerPythonEventReceiver<PythonHotSpotEventReceiver>( _kernel, py_kwds, _node, "onHandleMouseMove", EVENT_HOTSPOT_MOUSE_MOVE, MENGINE_DOCUMENT_PYBIND );
                Helper::registerPythonEventReceiver<PythonHotSpotEventReceiver>( _kernel, py_kwds, _node, "onHandleMouseWheel", EVENT_HOTSPOT_MOUSE_WHEEL, MENGINE_DOCUMENT_PYBIND );
                Helper::registerPythonEventReceiver<PythonHotSpotEventReceiver>( _kernel, py_kwds, _node, "onHandleMouseEnter", EVENT_HOTSPOT_MOUSE_ENTER, MENGINE_DOCUMENT_PYBIND );
                Helper::registerPythonEventReceiver<PythonHotSpotEventReceiver>( _kernel, py_kwds, _node, "onHandleMouseLeave", EVENT_HOTSPOT_MOUSE_LEAVE, MENGINE_DOCUMENT_PYBIND );
                Helper::registerPythonEventReceiver<PythonHotSpotEventReceiver>( _kernel, py_kwds, _node, "onHandleMouseOverDestroy", EVENT_HOTSPOT_MOUSE_OVER_DESTROY, MENGINE_DOCUMENT_PYBIND );
                Helper::registerPythonEventReceiver<PythonHotSpotEventReceiver>( _kernel, py_kwds, _node, "onHandleActivate", EVENT_HOTSPOT_ACTIVATE, MENGINE_DOCUMENT_PYBIND );
                Helper::registerPythonEventReceiver<PythonHotSpotEventReceiver>( _kernel, py_kwds, _node, "onHandleDeactivate", EVENT_HOTSPOT_DEACTIVATE, MENGINE_DOCUMENT_PYBIND );

                MENGINE_ASSERTION_PYTHON_EVENT_RECEIVER( _node, py_kwds );

                return _kernel->ret_none();
            }
            //////////////////////////////////////////////////////////////////////////
        };
        //////////////////////////////////////////////////////////////////////////
        typedef IntrusivePtr<NodeScriptMethod> NodeScriptMethodPtr;
        //////////////////////////////////////////////////////////////////////////
    }
    NodeScriptEmbedding::NodeScriptEmbedding()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    NodeScriptEmbedding::~NodeScriptEmbedding()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool NodeScriptEmbedding::embedding( pybind::kernel_interface * _kernel )
    {
        NodeScriptMethodPtr nodeScriptMethod = Helper::makeFactorableUnique<NodeScriptMethod>( MENGINE_DOCUMENT_FACTORABLE );

        if( nodeScriptMethod->initialize() == false )
        {
            return false;
        }

        pybind::interface_<ResourceImage, pybind::bases<Resource>>( _kernel, "ResourceImage", false )
            .def( "setMaxSize", &ResourceImage::setMaxSize )
            .def( "getMaxSize", &ResourceImage::getMaxSize )
            .def( "setSize", &ResourceImage::setSize )
            .def( "getSize", &ResourceImage::getSize )
            .def( "setOffset", &ResourceImage::setOffset )
            .def( "getOffset", &ResourceImage::getOffset )
            .def( "setUVImage", &ResourceImage::setUVImage )
            .def( "getUVImage", &ResourceImage::getUVImage )
            .def( "setUVAlpha", &ResourceImage::setUVAlpha )
            .def( "getUVAlpha", &ResourceImage::getUVAlpha )
            .def( "getUVTextureImage", &ResourceImage::getUVTextureImage )
            .def( "getUVTextureAlpha", &ResourceImage::getUVTextureAlpha )
            .def( "setAlpha", &ResourceImage::setAlpha )
            .def( "isAlpha", &ResourceImage::hasAlpha )
            .def( "setPremultiply", &ResourceImage::setPremultiply )
            .def( "isPremultiply", &ResourceImage::isPremultiply )
            .def( "setPow2", &ResourceImage::setPow2 )
            .def( "isPow2", &ResourceImage::isPow2 )
            .def( "setColor", &ResourceImage::setColor )
            .def( "getColor", &ResourceImage::getColor )
            ;

        pybind::interface_<ResourceImageData, pybind::bases<Resource>>( _kernel, "ResourceImageData", false )
            .def( "setImageMaxSize", &ResourceImageData::setImageMaxSize )
            .def( "getImageMaxSize", &ResourceImageData::getImageMaxSize )
            .def( "setImageWidth", &ResourceImageData::setImageWidth )
            .def( "getImageWidth", &ResourceImageData::getImageWidth )
            .def( "setImageHeight", &ResourceImageData::setImageHeight )
            .def( "getImageHeight", &ResourceImageData::getImageHeight )
            ;

        pybind::interface_<ResourceImageDefault, pybind::bases<ResourceImage>>( _kernel, "ResourceImageDefault", false )
            .def( "correctUVTexture", &ResourceImageDefault::correctUVTexture )
            ;

        pybind::interface_<ResourceImageSubstractRGBAndAlpha, pybind::bases<ResourceImage>>( _kernel, "ResourceImageSubstractRGBAndAlpha", false )
            ;

        pybind::interface_<ResourceImageSubstract, pybind::bases<ResourceImage>>( _kernel, "ResourceImageSubstract", false )
            ;

        pybind::interface_<ResourceImageSolid, pybind::bases<ResourceImage>>( _kernel, "ResourceImageSolid", false )
            ;

        pybind::interface_<ResourceMusic, pybind::bases<Resource>>( _kernel, "ResourceMusic", false )
            ;

        pybind::interface_<ResourceImageSequence, pybind::bases<Resource>>( _kernel, "ResourceImageSequence", false )
            ;

        pybind::interface_<ResourceSound, pybind::bases<Resource>>( _kernel, "ResourceSound", false )
            ;

        pybind::interface_<ResourceFile, pybind::bases<Resource>>( _kernel, "ResourceFile", false )
            ;

        pybind::interface_<ResourceShape, pybind::bases<Resource>>( _kernel, "ResourceShape", false )
            .def( "setPolygon", &ResourceShape::setPolygon )
            .def( "getPolygon", &ResourceShape::getPolygon )
            ;

        pybind::interface_<ResourceCursor, pybind::bases<Resource>>( _kernel, "ResourceCursor", false )
            ;

        pybind::interface_<ResourceCursorICO, pybind::bases<ResourceCursor>>( _kernel, "ResourceCursorICO", false )
            ;

        pybind::interface_<ResourceCursorSystem, pybind::bases<ResourceCursor>>( _kernel, "ResourceCursorSystem", false )
            ;

        pybind::interface_<ResourceWindow, pybind::bases<Resource>>( _kernel, "ResourceWindow", false )
            ;

        pybind::interface_<ResourceTestPick, pybind::bases<Resource>>( _kernel, "ResourceTestPick", false )
            .def_deprecated( "getWidth", &ResourceTestPick::getImageWidth, "use getImageWidth" )
            .def_deprecated( "getHeight", &ResourceTestPick::getImageHeight, "use getImageHeight" )
            .def( "getImageWidth", &ResourceTestPick::getImageWidth )
            .def( "getImageHeight", &ResourceTestPick::getImageHeight )
            .def( "getImageWidthF", &ResourceTestPick::getImageWidthF )
            .def( "getImageHeightF", &ResourceTestPick::getImageHeightF )
            ;

        pybind::interface_<ResourceHIT, pybind::bases<ResourceTestPick>>( _kernel, "ResourceHIT", false )
            ;

        pybind::interface_<SurfaceSound, pybind::bases<Surface, Soundable>>( _kernel, "SurfaceSound", false )
            .def( "setResourceSound", &SurfaceSound::setResourceSound )
            .def( "getResourceSound", &SurfaceSound::getResourceSound )
            .def_proxy_native_kernel( "setEventListener", nodeScriptMethod, &NodeScriptMethod::s_SurfaceSound_setEventListener )
            ;

        pybind::interface_<SurfaceImage, pybind::bases<Surface>>( _kernel, "SurfaceImage", false )
            .def( "setResourceImage", &SurfaceImage::setResourceImage )
            .def( "getResourceImage", &SurfaceImage::getResourceImage )
            ;

        pybind::interface_<SurfaceImageSequence, pybind::bases<Surface>>( _kernel, "SurfaceImageSequence", false )
            .def( "setResourceImageSequence", &SurfaceImageSequence::setResourceImageSequence )
            .def( "getResourceImageSequence", &SurfaceImageSequence::getResourceImageSequence )
            .def( "getFrameCount", &SurfaceImageSequence::getFrameCount )
            .def( "getFrameDelay", &SurfaceImageSequence::getFrameDelay )
            .def( "setCurrentFrame", &SurfaceImageSequence::setCurrentFrame )
            .def( "getCurrentFrame", &SurfaceImageSequence::getCurrentFrame )
            .def_proxy_native_kernel( "setEventListener", nodeScriptMethod, &NodeScriptMethod::s_SurfaceImageSequence_setEventListener )
            ;

        pybind::interface_<SurfaceTrackMatte, pybind::bases<Surface>>( _kernel, "SurfaceTrackMatte", false )
            .def( "setResourceImage", &SurfaceTrackMatte::setResourceImage )
            .def( "getResourceImage", &SurfaceTrackMatte::getResourceImage )
            .def( "setResourceTrackMatteImage", &SurfaceTrackMatte::setResourceTrackMatteImage )
            .def( "getResourceTrackMatteImage", &SurfaceTrackMatte::getResourceTrackMatteImage )
            .def( "setTrackMatteMode", &SurfaceTrackMatte::setTrackMatteMode )
            .def( "getTrackMatteMode", &SurfaceTrackMatte::getTrackMatteMode )
            ;

        pybind::interface_<SurfaceSolidColor, pybind::bases<Surface>>( _kernel, "SurfaceSolidColor", false )
            .def( "setSolidColor", &SurfaceSolidColor::setSolidColor )
            .def( "getSolidColor", &SurfaceSolidColor::getSolidColor )
            .def( "setSolidSize", &SurfaceSolidColor::setSolidSize )
            .def( "getSolidSize", &SurfaceSolidColor::getSolidSize )
            ;

        pybind::interface_<RenderViewport, pybind::bases<Node, RenderViewportInterface>>( _kernel, "RenderViewport", false )
            .def( "setFixedViewport", &RenderViewport::setFixedViewport )
            .def( "getFixedViewport", &RenderViewport::getFixedViewport )
            .def( "setViewport", &RenderViewport::setViewport )
            ;

        pybind::interface_<RenderScissor, pybind::bases<Node, RenderScissorInterface>>( _kernel, "RenderScissor", false )
            .def( "setViewport", &RenderScissor::setViewport )
            .def( "getViewport", &RenderScissor::getViewport )
            ;

        pybind::interface_<RenderCameraOrthogonal, pybind::bases<Node, RenderCameraInterface>>( _kernel, "RenderCameraOrthogonal", false )
            .def( "setCameraPosition", &RenderCameraOrthogonal::setCameraPosition )
            .def( "setCameraDirection", &RenderCameraOrthogonal::setCameraDirection )
            .def( "setCameraUp", &RenderCameraOrthogonal::setCameraUp )
            .def( "setCameraRightSign", &RenderCameraOrthogonal::setCameraRightSign )
            .def( "setCameraNear", &RenderCameraOrthogonal::setCameraNear )
            .def( "setCameraFar", &RenderCameraOrthogonal::setCameraFar )
            .def( "setOrthogonalViewport", &RenderCameraOrthogonal::setOrthogonalViewport )
            .def( "getOrthogonalViewport", &RenderCameraOrthogonal::getOrthogonalViewport )
            .def( "setFixedOrthogonalViewport", &RenderCameraOrthogonal::setFixedOrthogonalViewport )
            .def( "getFixedOrthogonalViewport", &RenderCameraOrthogonal::getFixedOrthogonalViewport )
            .def( "setProxyViewMatrix", &RenderCameraOrthogonal::setProxyViewMatrix )
            .def( "getProxyViewMatrix", &RenderCameraOrthogonal::getProxyViewMatrix )
            ;

        pybind::interface_<RenderCameraProjection, pybind::bases<Node, RenderCameraInterface>>( _kernel, "RenderCameraProjection", false )
            .def( "setCameraPosition", &RenderCameraProjection::setCameraPosition )
            .def( "setCameraDirection", &RenderCameraProjection::setCameraDirection )
            .def( "setCameraUp", &RenderCameraProjection::setCameraUp )
            .def( "setCameraRightSign", &RenderCameraProjection::setCameraRightSign )
            .def( "setCameraFOV", &RenderCameraProjection::setCameraFOV )
            .def( "setCameraAspect", &RenderCameraProjection::setCameraAspect )
            .def( "setCameraNear", &RenderCameraProjection::setCameraNear )
            .def( "setCameraFar", &RenderCameraProjection::setCameraFar )
            ;

        pybind::interface_<RenderCameraOrthogonalTarget, pybind::bases<Node>>( _kernel, "RenderCameraOrthogonalTarget", false )
            .def( "setRenderCameraOrthogonal", &RenderCameraOrthogonalTarget::setRenderCameraOrthogonal )
            .def( "getRenderCameraOrthogonal", &RenderCameraOrthogonalTarget::getRenderCameraOrthogonal )
            .def( "setSpeed", &RenderCameraOrthogonalTarget::setSpeed )
            .def( "getSpeed", &RenderCameraOrthogonalTarget::getSpeed )
            .def( "setFixedHorizont", &RenderCameraOrthogonalTarget::setFixedHorizont )
            .def( "getFixedHorizont", &RenderCameraOrthogonalTarget::getFixedHorizont )
            .def( "isFixedHorizont", &RenderCameraOrthogonalTarget::isFixedHorizont )
            ;

        pybind::interface_<RenderScissor, pybind::bases<Node, RenderScissorInterface>>( _kernel, "RenderScissor", false )
            ;

        {
            pybind::interface_<SoundEmitter, pybind::bases<Node>>( _kernel, "SoundEmitter", false )
                .def( "setSurfaceSound", &SoundEmitter::setSurfaceSound )
                .def( "getSurfaceSound", &SoundEmitter::getSurfaceSound )
                ;

            pybind::interface_<TextFontInterface, pybind::bases<ServantInterface>>( _kernel, "TextFontInterface" )
                .def( "setName", &TextFontInterface::setName )
                .def( "getName", &TextFontInterface::getName )
                .def( "setType", &TextFontInterface::setType )
                .def( "getType", &TextFontInterface::getType )
                .def( "setHeight", &TextFontInterface::setHeight )
                .def( "getHeight", &TextFontInterface::getHeight )
                .def( "setFontColor", &TextFontInterface::setFontColor )
                .def( "getFontColor", &TextFontInterface::getFontColor )
                .def( "setLineOffset", &TextFontInterface::setLineOffset )
                .def( "getLineOffset", &TextFontInterface::getLineOffset )
                .def( "setCharOffset", &TextFontInterface::setCharOffset )
                .def( "getCharOffset", &TextFontInterface::getCharOffset )
                .def( "initialize", &TextFontInterface::initialize )
                .def( "finalize", &TextFontInterface::finalize )
                .def( "getFontAscent", &TextFontInterface::getFontAscent )
                .def( "getFontDescent", &TextFontInterface::getFontDescent )
                .def( "getFontHeight", &TextFontInterface::getFontHeight )
                .def( "getFontBearingYA", &TextFontInterface::getFontBearingYA )
                .def( "getFontSpacing", &TextFontInterface::getFontSpacing )
                .def( "getFontPremultiply", &TextFontInterface::getFontPremultiply )
                ;

            pybind::interface_<TextField, pybind::bases<Node>>( _kernel, "TextField", false )
                .def_deprecated( "setTextByKey", &TextField::setTextId, "use setTextID" )
                .def( "setTextID", &TextField::setTextId )
                .def( "removeTextID", &TextField::removeTextId )
                .def_proxy_native_kernel( "setTextFormatArgs", nodeScriptMethod, &NodeScriptMethod::s_TextField_setTextFormatArgs )
                .def_proxy_static( "getTextFormatArgs", nodeScriptMethod, &NodeScriptMethod::s_TextField_getTextFormatArgs )
                .def( "removeTextFormatArgs", &TextField::removeTextFormatArgs )
                .def_deprecated( "getTextKey", &TextField::getTotalTextId, "use getTextID" )
                .def( "getTextId", &TextField::getTextId )
                .def_deprecated( "getTextEntryId", &TextField::getTotalTextId, "use getTotalTextId" )
                .def( "getTotalTextId", &TextField::getTotalTextId )
                .def( "getTextExpectedArgument", &TextField::getTextExpectedArgument )

                .def_deprecated( "getHeight", &TextField::getFontHeight, "use getFontHeight" )
                .def_deprecated( "getAlphaHeight", &TextField::getFontHeight, "use getFontHeight" )
                .def( "getFontHeight", &TextField::getFontHeight )
                .def( "setFont", &TextField::setFont )
                .def( "getFont", &TextField::getFont )
                .def_proxy_static( "setFontName", nodeScriptMethod, &NodeScriptMethod::s_TextField_setFontName )
                .def_proxy_static( "getFontName", nodeScriptMethod, &NodeScriptMethod::s_TextField_getFontName )

                .def( "setWrap", &TextField::setWrap )
                .def( "getWrap", &TextField::getWrap )

                .def( "setFontColor", &TextField::setFontColor )
                .def( "getFontColor", &TextField::getFontColor )

                .def( "setLineOffset", &TextField::setLineOffset )
                .def( "getLineOffset", &TextField::getLineOffset )

                .def( "setCharOffset", &TextField::setCharOffset )
                .def( "getCharOffset", &TextField::getCharOffset )

                .def_deprecated( "getLength", &TextField::getTextSize, "use getTextSize" )
                .def( "getTextSize", &TextField::getTextSize )
                .def_deprecated( "setMaxLen", &TextField::setMaxLength, "use setMaxLength" )
                .def( "setMaxLength", &TextField::setMaxLength )
                .def( "getMaxLength", &TextField::getMaxLength )
                .def( "setAutoScale", &TextField::setAutoScale )
                .def( "getAutoScale", &TextField::getAutoScale )

                .def( "setAnchorHorizontalAlign", &TextField::setAnchorHorizontalAlign )
                .def( "getAnchorHorizontalAlign", &TextField::getAnchorHorizontalAlign )

                .def( "setAnchorVerticalAlign", &TextField::setAnchorVerticalAlign )
                .def( "getAnchorVerticalAlign", &TextField::getAnchorVerticalAlign )

                .def( "setAnchorPercent", &TextField::setAnchorPercent )
                .def( "getAnchorPercent", &TextField::getAnchorPercent )

                .def_deprecated( "setCenterAlign", &TextField::setHorizontalCenterAlign, "use setHorizontalCenterAlign" )
                .def_deprecated( "isCenterAlign", &TextField::isHorizontalCenterAlign, "use isHorizontalCenterAlign" )
                .def_deprecated( "setRightAlign", &TextField::setHorizontalRightAlign, "use setHorizontalRightAlign" )
                .def_deprecated( "isRightAlign", &TextField::isHorizontalRightAlign, "use isHorizontalRightAlign" )
                .def_deprecated( "setLeftAlign", &TextField::setHorizontalLeftAlign, "use setHorizontalLeftAlign" )
                .def_deprecated( "isLeftAlign", &TextField::isHorizontalLeftAlign, "use isHorizontalLeftAlign" )

                .def( "setHorizontalCenterAlign", &TextField::setHorizontalCenterAlign )
                .def( "isHorizontalCenterAlign", &TextField::isHorizontalCenterAlign )
                .def( "setHorizontalRightAlign", &TextField::setHorizontalRightAlign )
                .def( "isHorizontalRightAlign", &TextField::isHorizontalRightAlign )
                .def( "setHorizontalLeftAlign", &TextField::setHorizontalLeftAlign )
                .def( "isHorizontalLeftAlign", &TextField::isHorizontalLeftAlign )

                .def( "setVerticalTopAlign", &TextField::setVerticalTopAlign )
                .def( "isVerticalTopAlign", &TextField::isVerticalTopAlign )
                .def( "setVerticalBottomAlign", &TextField::setVerticalBottomAlign )
                .def( "isVerticalBottomAlign", &TextField::isVerticalBottomAlign )
                .def( "setVerticalCenterAlign", &TextField::setVerticalCenterAlign )
                .def( "isVerticalCenterAlign", &TextField::isVerticalCenterAlign )

                .def( "setPixelsnap", &TextField::setPixelsnap )
                .def( "getPixelsnap", &TextField::getPixelsnap )

                .def( "setMaxCharCount", &TextField::setMaxCharCount )
                .def( "getMaxCharCount", &TextField::getMaxCharCount )
                .def( "getCharCount", &TextField::getCharCount )
                ;

            pybind::interface_<ScriptHolder, pybind::bases<Node, Eventable>>( _kernel, "ScriptHolder", false )
                .def_proxy_native_kernel( "setEventListener", nodeScriptMethod, &NodeScriptMethod::s_ScriptHolder_setEventListener )
                ;

            pybind::interface_<Point, pybind::bases<Node>>( _kernel, "Point", false )
                .def( "setLinkedPoint", &Point::setLinkedPoint )
                .def( "removeLinkedPoint", &Point::removeLinkedPoint )
                .def( "getLinkedPoint", &Point::getLinkedPoint )
                .def( "setWidth", &Point::setWidth )
                .def( "getWidth", &Point::getWidth )
                ;

            pybind::interface_<Line, pybind::bases<Node>>( _kernel, "Line", false )
                .def( "setFrom", &Line::setFrom )
                .def( "getFrom", &Line::getFrom )
                .def( "setTo", &Line::setTo )
                .def( "getTo", &Line::getTo )
                .def( "setWeight", &Line::setWeight )
                .def( "getWeight", &Line::getWeight )
                .def_deprecated( "setWidth", &Line::setWeight, "use setWeight" )
                .def_deprecated( "getWidth", &Line::getWeight, "use getWeight" )
                ;

            pybind::interface_<Layer, pybind::bases<Node>>( _kernel, "Layer", false )
                ;

            pybind::interface_<Layer2D, pybind::bases<Layer>>( _kernel, "Layer2D", false )
                .def( "setSize", &Layer2D::setSize )
                .def( "getSize", &Layer2D::getSize )
                .def( "setViewport", &Layer2D::setViewport )
                .def( "removeViewport", &Layer2D::removeViewport )
                .def( "setImageMask", &Layer2D::setImageMask )
                .def( "removeImageMask", &Layer2D::removeImageMask )
                ;

            //pybind::interface_<Parallax, pybind::bases<Node>>( kernel, "Parallax", false )
            //    .def( "setParallaxFactor", &Parallax::setParallaxFactor )
            //    .def( "getParallaxFactor", &Parallax::getParallaxFactor )
            //    ;

            pybind::interface_<HotSpot, pybind::bases<Node, Eventable>>( _kernel, "HotSpot", false )
                .def( "testPoint", &HotSpot::testPoint )
                .def( "setOutward", &HotSpot::setOutward )
                .def( "getOutward", &HotSpot::getOutward )
                .def( "setGlobal", &HotSpot::setGlobal )
                .def( "getGlobal", &HotSpot::getGlobal )
                .def( "setExclusive", &HotSpot::setExclusive )
                .def( "getExclusive", &HotSpot::getExclusive )
                .def( "setDefaultHandle", &HotSpot::setDefaultHandle )
                .def( "getDefaultHandle", &HotSpot::getDefaultHandle )
                .def( "isMousePickerOver", &HotSpot::isMousePickerOver )
                .def_proxy_native_kernel( "setEventListener", nodeScriptMethod, &NodeScriptMethod::s_HotSpot_setEventListener )
                ;

            pybind::interface_<HotSpotGlobal, pybind::bases<HotSpot>>( _kernel, "HotSpotGlobal", false )
                ;

            pybind::interface_<HotSpotPolygon, pybind::bases<HotSpot>>( _kernel, "HotSpotPolygon", false )
                .def( "setPolygon", &HotSpotPolygon::setPolygon )
                .def( "getPolygon", &HotSpotPolygon::getPolygon )
                .def( "clearPoints", &HotSpotPolygon::clearPoints )
                .def_proxy_static( "getLocalPolygonCenter", nodeScriptMethod, &NodeScriptMethod::s_HotSpotPolygon_getLocalPolygonCenter )
                .def_proxy_static( "getWorldPolygonCenter", nodeScriptMethod, &NodeScriptMethod::s_HotSpotPolygon_getWorldPolygonCenter )
                .def_proxy_static( "getScreenPolygonCenter", nodeScriptMethod, &NodeScriptMethod::s_HotSpotPolygon_getScreenPolygonCenter )
                .def_proxy_static( "getWorldPolygon", nodeScriptMethod, &NodeScriptMethod::s_HotSpotPolygon_getWorldPolygon )
                ;

            pybind::interface_<HotSpotResourceShape, pybind::bases<HotSpotPolygon>>( _kernel, "HotSpotResourceShape", false )
                .def( "setResourceShape", &HotSpotResourceShape::setResourceShape )
                .def( "getResourceShape", &HotSpotResourceShape::getResourceShape )
                ;

            pybind::interface_<HotSpotSurface, pybind::bases<HotSpotPolygon>>( _kernel, "HotSpotSurface", false )
                .def( "setSurface", &HotSpotSurface::setSurface )
                .def( "getSurface", &HotSpotSurface::getSurface )
                ;

            pybind::interface_<HotSpotImage, pybind::bases<HotSpot>>( _kernel, "HotSpotImage", false )
                .def_deprecated( "setResourceHIT", &HotSpotImage::setResourceTestPick, "use setResourceTestPick" )
                .def_deprecated( "getResourceHIT", &HotSpotImage::getResourceTestPick, "use getResourceTestPick" )
                .def( "setResourceTestPick", &HotSpotImage::setResourceTestPick )
                .def( "getResourceTestPick", &HotSpotImage::getResourceTestPick )
                .def( "setAlphaTest", &HotSpotImage::setAlphaTest )
                .def( "getAlphaTest", &HotSpotImage::getAlphaTest )
                .def( "getWidth", &HotSpotImage::getWidth )
                .def( "getHeight", &HotSpotImage::getHeight )
                ;

            pybind::interface_<HotSpotCircle, pybind::bases<HotSpot>>( _kernel, "HotSpotCircle", false )
                .def( "setRadius", &HotSpotCircle::setRadius )
                .def( "getRadius", &HotSpotCircle::getRadius )
                .def( "setEllipse", &HotSpotCircle::setEllipse )
                .def( "getEllipse", &HotSpotCircle::getEllipse )
                ;

            pybind::interface_<HotSpotBubbles, pybind::bases<HotSpot>>( _kernel, "HotSpotBubbles", false )
                .def( "addBubble", &HotSpotBubbles::addBubble )
                ;

            pybind::interface_<Shape, pybind::bases<Node>>( _kernel, "Shape", false )
                .def( "setSurface", &Shape::setSurface )
                .def( "getSurface", &Shape::getSurface )

                .def_proxy_static( "getSurfaceSize", nodeScriptMethod, &NodeScriptMethod::s_Shape_getSurfaceSize )
                .def_proxy_static( "getLocalImageCenter", nodeScriptMethod, &NodeScriptMethod::s_Shape_getLocalImageCenter )
                .def_proxy_static( "getWorldImageCenter", nodeScriptMethod, &NodeScriptMethod::s_Shape_getWorldImageCenter )
                ;

            pybind::interface_<ShapeCircle, pybind::bases<Shape>>( _kernel, "ShapeCircle", false )
                .def( "setQuality", &ShapeCircle::setQuality )
                .def( "getQuality", &ShapeCircle::getQuality )
                ;

            pybind::interface_<ShapePacMan, pybind::bases<Shape>>( _kernel, "ShapePacMan", false )
                .def( "setAngleFrom", &ShapePacMan::setAngleFrom )
                .def( "getAngleFrom", &ShapePacMan::getAngleFrom )
                .def( "setAngleTo", &ShapePacMan::setAngleTo )
                .def( "getAngleTo", &ShapePacMan::getAngleTo )
                ;

            pybind::interface_<ShapeQuad, pybind::bases<Shape>>( _kernel, "ShapeQuad", false )
                ;

            pybind::interface_<ShapeQuadFixed, pybind::bases<ShapeQuad>>( _kernel, "ShapeQuadFixed", false )
                ;

            pybind::interface_<ShapeQuadFlex, pybind::bases<ShapeQuad>>( _kernel, "ShapeQuadFlex", false )
                .def( "setCenterAlign", &ShapeQuadFlex::setCenterAlign )
                .def( "getCenterAlign", &ShapeQuadFlex::getCenterAlign )
                .def( "setFlipX", &ShapeQuadFlex::setFlipX )
                .def( "getFlipX", &ShapeQuadFlex::getFlipX )
                .def( "setFlipY", &ShapeQuadFlex::setFlipY )
                .def( "getFlipY", &ShapeQuadFlex::getFlipY )
                .def( "setPercentVisibility", &ShapeQuadFlex::setPercentVisibility )
                .def( "getPercentVisibility", &ShapeQuadFlex::getPercentVisibility )
                .def( "setCustomSize", &ShapeQuadFlex::setCustomSize )
                .def( "getCustomSize", &ShapeQuadFlex::getCustomSize )
                .def( "removeCustomSize", &ShapeQuadFlex::removeCustomSize )
                .def( "hasCustomSize", &ShapeQuadFlex::hasCustomSize )
                .def_proxy_static_args( "setPercentVisibilityTo", nodeScriptMethod, &NodeScriptMethod::s_ShapeQuadFlex_setPercentVisibilityTo )
                .def_proxy_static( "setPercentVisibilityStop", nodeScriptMethod, &NodeScriptMethod::s_ShapeQuadFlex_setPercentVisibilityStop )
                .def( "setTextureUVOffset", &ShapeQuadFlex::setTextureUVOffset )
                .def( "getTextureUVOffset", &ShapeQuadFlex::getTextureUVOffset )
                .def( "setTextureUVScale", &ShapeQuadFlex::setTextureUVScale )
                .def( "getTextureUVScale", &ShapeQuadFlex::getTextureUVScale )
                ;

            pybind::interface_<Landscape2D, pybind::bases<Node, Materialable>>( _kernel, "Landscape2D", false )
                .def( "setBackParts", &Landscape2D::setBackParts )
                ;

            pybind::interface_<Grid2D, pybind::bases<Node, Materialable>>( _kernel, "Grid2D", false )
                .def( "setResourceImage", &Grid2D::setResourceImage )
                .def( "getResourceImage", &Grid2D::getResourceImage )
                .def( "setWidth", &Grid2D::setWidth )
                .def( "getWidth", &Grid2D::getWidth )
                .def( "setHeight", &Grid2D::setHeight )
                .def( "getHeight", &Grid2D::getHeight )
                .def( "setCountX", &Grid2D::setCountX )
                .def( "getCountX", &Grid2D::getCountX )
                .def( "setCountY", &Grid2D::setCountY )
                .def( "getCountY", &Grid2D::getCountY )
                .def( "setAngle", &Grid2D::setAngle )
                ;

            pybind::interface_<Gyroscope, pybind::bases<Node>>( _kernel, "Gyroscope", false )
                ;

            pybind::interface_<Interender, pybind::bases<Node>>( _kernel, "Interender", false )
                ;

            pybind::interface_<Isometric, pybind::bases<Node>>( _kernel, "Isometric", false )
                ;

            pybind::interface_<MatrixProxy, pybind::bases<Node>>( _kernel, "MatrixProxy", false )
                ;


            pybind::interface_<Meshget, pybind::bases<Node, Eventable>>( _kernel, "Meshget", false )
                .def( "setSurface", &Meshget::setSurface )
                .def( "getSurface", &Meshget::getSurface )
                .def( "setVertices", &Meshget::setVertices )
                .def_proxy_native_kernel( "setEventListener", nodeScriptMethod, &NodeScriptMethod::s_Meshget_setEventListener )
                ;

            pybind::interface_<Window, pybind::bases<Node>>( _kernel, "Window", false )
                .def( "setResourceWindow", &Window::setResourceWindow )
                .def( "getResourceWindow", &Window::getResourceWindow )
                .def( "setClientSize", &Window::setClientSize )
                .def( "setClientSizeClip", &Window::setClientSizeClip )
                .def( "setClientSizeInTiles", &Window::setClientSizeInTiles )
                .def( "getClientSize", &Window::getClientSize )
                .def( "getWindowSize", &Window::getWindowSize )
                .def( "getTileSize", &Window::getTileSize )
                ;
        }

#define SCRIPT_CLASS_WRAPPING( Class )\
    Helper::registerScriptWrapping<Class>(_kernel, STRINGIZE_STRING_LOCAL(#Class), MENGINE_DOCUMENT_FACTORABLE)

        SCRIPT_CLASS_WRAPPING( Node );
        SCRIPT_CLASS_WRAPPING( Layer );
        SCRIPT_CLASS_WRAPPING( Layer2D );
        SCRIPT_CLASS_WRAPPING( HotSpot );
        SCRIPT_CLASS_WRAPPING( HotSpotPolygon );
        SCRIPT_CLASS_WRAPPING( HotSpotGlobal );
        SCRIPT_CLASS_WRAPPING( HotSpotCircle );
        SCRIPT_CLASS_WRAPPING( HotSpotBubbles );
        SCRIPT_CLASS_WRAPPING( HotSpotImage );
        SCRIPT_CLASS_WRAPPING( HotSpotResourceShape );
        SCRIPT_CLASS_WRAPPING( HotSpotSurface );

        SCRIPT_CLASS_WRAPPING( ScriptHolder );

        SCRIPT_CLASS_WRAPPING( Gyroscope );
        SCRIPT_CLASS_WRAPPING( Interender );
        SCRIPT_CLASS_WRAPPING( Isometric );
        SCRIPT_CLASS_WRAPPING( MatrixProxy );
        SCRIPT_CLASS_WRAPPING( TextField );
        SCRIPT_CLASS_WRAPPING( SoundEmitter );
        SCRIPT_CLASS_WRAPPING( Meshget );
        SCRIPT_CLASS_WRAPPING( Point );
        SCRIPT_CLASS_WRAPPING( Line );
        SCRIPT_CLASS_WRAPPING( Landscape2D );
        SCRIPT_CLASS_WRAPPING( Grid2D );

        SCRIPT_CLASS_WRAPPING( ShapeCircle );
        SCRIPT_CLASS_WRAPPING( ShapePacMan );
        SCRIPT_CLASS_WRAPPING( ShapeQuadFixed );
        SCRIPT_CLASS_WRAPPING( ShapeQuadFlex );

        SCRIPT_CLASS_WRAPPING( Window );

        SCRIPT_CLASS_WRAPPING( RenderViewport );
        SCRIPT_CLASS_WRAPPING( RenderScissor );
        SCRIPT_CLASS_WRAPPING( RenderCameraOrthogonal );
        SCRIPT_CLASS_WRAPPING( RenderCameraProjection );
        SCRIPT_CLASS_WRAPPING( RenderCameraOrthogonalTarget );

        SCRIPT_CLASS_WRAPPING( Resource );
        SCRIPT_CLASS_WRAPPING( ResourceImage );
        SCRIPT_CLASS_WRAPPING( ResourceImageData );
        SCRIPT_CLASS_WRAPPING( ResourceImageDefault );
        SCRIPT_CLASS_WRAPPING( ResourceMusic );
        SCRIPT_CLASS_WRAPPING( ResourceImageSequence );
        SCRIPT_CLASS_WRAPPING( ResourceSound );
        SCRIPT_CLASS_WRAPPING( ResourceFile );

        SCRIPT_CLASS_WRAPPING( ResourceImageSolid );
        SCRIPT_CLASS_WRAPPING( ResourceShape );
        SCRIPT_CLASS_WRAPPING( ResourceCursorICO );
        SCRIPT_CLASS_WRAPPING( ResourceCursorSystem );
        SCRIPT_CLASS_WRAPPING( ResourceWindow );
        SCRIPT_CLASS_WRAPPING( ResourceImageSubstractRGBAndAlpha );
        SCRIPT_CLASS_WRAPPING( ResourceImageSubstract );

        SCRIPT_CLASS_WRAPPING( ResourceTestPick );
        SCRIPT_CLASS_WRAPPING( ResourceHIT );

        SCRIPT_CLASS_WRAPPING( Surface );
        SCRIPT_CLASS_WRAPPING( SurfaceSound );
        SCRIPT_CLASS_WRAPPING( SurfaceImage );
        SCRIPT_CLASS_WRAPPING( SurfaceImageSequence );
        SCRIPT_CLASS_WRAPPING( SurfaceTrackMatte );
        SCRIPT_CLASS_WRAPPING( SurfaceSolidColor );

#undef SCRIPT_CLASS_WRAPPING

        m_implement = nodeScriptMethod;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void NodeScriptEmbedding::ejecting( pybind::kernel_interface * _kernel )
    {
        MENGINE_UNUSED( _kernel );

#define UNSCRIPT_CLASS_WRAPPING( Class )\
    Helper::unregisterScriptWrapping(STRINGIZE_STRING_LOCAL(#Class))

        UNSCRIPT_CLASS_WRAPPING( Node );
        UNSCRIPT_CLASS_WRAPPING( Layer );
        UNSCRIPT_CLASS_WRAPPING( Layer2D );
        UNSCRIPT_CLASS_WRAPPING( HotSpot );
        UNSCRIPT_CLASS_WRAPPING( HotSpotPolygon );
        UNSCRIPT_CLASS_WRAPPING( HotSpotGlobal );
        UNSCRIPT_CLASS_WRAPPING( HotSpotCircle );
        UNSCRIPT_CLASS_WRAPPING( HotSpotBubbles );
        UNSCRIPT_CLASS_WRAPPING( HotSpotImage );
        UNSCRIPT_CLASS_WRAPPING( HotSpotResourceShape );
        UNSCRIPT_CLASS_WRAPPING( HotSpotSurface );

        UNSCRIPT_CLASS_WRAPPING( ScriptHolder );

        UNSCRIPT_CLASS_WRAPPING( Gyroscope );
        UNSCRIPT_CLASS_WRAPPING( Interender );
        UNSCRIPT_CLASS_WRAPPING( Isometric );
        UNSCRIPT_CLASS_WRAPPING( MatrixProxy );
        UNSCRIPT_CLASS_WRAPPING( TextField );
        UNSCRIPT_CLASS_WRAPPING( SoundEmitter );
        UNSCRIPT_CLASS_WRAPPING( Meshget );
        UNSCRIPT_CLASS_WRAPPING( Point );
        UNSCRIPT_CLASS_WRAPPING( Line );
        UNSCRIPT_CLASS_WRAPPING( Landscape2D );
        UNSCRIPT_CLASS_WRAPPING( Grid2D );

        UNSCRIPT_CLASS_WRAPPING( ShapeCircle );
        UNSCRIPT_CLASS_WRAPPING( ShapePacMan );
        UNSCRIPT_CLASS_WRAPPING( ShapeQuadFixed );
        UNSCRIPT_CLASS_WRAPPING( ShapeQuadFlex );

        UNSCRIPT_CLASS_WRAPPING( Graphics );

        UNSCRIPT_CLASS_WRAPPING( Window );

        UNSCRIPT_CLASS_WRAPPING( RenderViewport );
        UNSCRIPT_CLASS_WRAPPING( RenderScissor );
        UNSCRIPT_CLASS_WRAPPING( RenderCameraOrthogonal );
        UNSCRIPT_CLASS_WRAPPING( RenderCameraProjection );
        UNSCRIPT_CLASS_WRAPPING( RenderCameraOrthogonalTarget );

        UNSCRIPT_CLASS_WRAPPING( Resource );
        UNSCRIPT_CLASS_WRAPPING( ResourceImage );
        UNSCRIPT_CLASS_WRAPPING( ResourceImageData );
        UNSCRIPT_CLASS_WRAPPING( ResourceImageDefault );
        UNSCRIPT_CLASS_WRAPPING( ResourceMusic );
        UNSCRIPT_CLASS_WRAPPING( ResourceImageSequence );
        UNSCRIPT_CLASS_WRAPPING( ResourceSound );
        UNSCRIPT_CLASS_WRAPPING( ResourceFile );

        UNSCRIPT_CLASS_WRAPPING( ResourceImageSolid );
        UNSCRIPT_CLASS_WRAPPING( ResourceShape );
        UNSCRIPT_CLASS_WRAPPING( ResourceCursorICO );
        UNSCRIPT_CLASS_WRAPPING( ResourceCursorSystem );
        UNSCRIPT_CLASS_WRAPPING( ResourceWindow );
        UNSCRIPT_CLASS_WRAPPING( ResourceImageSubstractRGBAndAlpha );
        UNSCRIPT_CLASS_WRAPPING( ResourceImageSubstract );

        UNSCRIPT_CLASS_WRAPPING( ResourceTestPick );
        UNSCRIPT_CLASS_WRAPPING( ResourceHIT );

        UNSCRIPT_CLASS_WRAPPING( Surface );
        UNSCRIPT_CLASS_WRAPPING( SurfaceSound );
        UNSCRIPT_CLASS_WRAPPING( SurfaceImage );
        UNSCRIPT_CLASS_WRAPPING( SurfaceImageSequence );
        UNSCRIPT_CLASS_WRAPPING( SurfaceTrackMatte );
        UNSCRIPT_CLASS_WRAPPING( SurfaceSolidColor );

#undef UNSCRIPT_CLASS_WRAPPING

        NodeScriptMethodPtr nodeScriptMethod = m_implement;
        nodeScriptMethod->finalize();

        m_implement = nullptr;
    }
}
