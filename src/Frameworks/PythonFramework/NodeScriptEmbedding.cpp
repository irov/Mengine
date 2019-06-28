#include "NodeScriptEmbedding.h"

#include "Interface/ApplicationInterface.h"
#include "Interface/TimelineServiceInterface.h"
#include "Interface/StringizeServiceInterface.h"
#include "Interface/RenderSystemInterface.h"
#include "Interface/InputServiceInterface.h"
#include "Interface/PrototypeServiceInterface.h"
#include "Interface/MemoryInterface.h"
#include "Interface/PlatformInterface.h"
#include "Interface/PackageInterface.h"
#include "Interface/SceneServiceInterface.h"
#include "Interface/VocabularyServiceInterface.h"

#include "Kernel/ThreadTask.h"
#include "Kernel/Scene.h"
#include "Kernel/Arrow.h"
#include "Kernel/NodeRenderHelper.h"
#include "Kernel/MatrixProxy.h"
#include "Kernel/AssertionMemoryPanic.h"

#include "Interface/ScriptServiceInterface.h"
#include "Interface/SchedulerInterface.h"

#include "Interface/TextFontInterface.h"
#include "Interface/AccountInterface.h"

#include "Interface/UnicodeSystemInterface.h"

#include "Interface/ResourceServiceInterface.h"

#include "Engine/ResourceImageData.h"
#include "Engine/ResourceFile.h"
#include "Engine/ResourceMusic.h"
#include "Engine/ResourceImageSequence.h"
#include "Engine/ResourceSound.h"
#include "Engine/ResourceImageSolid.h"
#include "Engine/ResourceImageDefault.h"
#include "Engine/ResourceTestPick.h"
#include "Engine/ResourceHIT.h"
#include "Engine/ResourceShape.h"
#include "Engine/ResourceCursorICO.h"
#include "Engine/ResourceCursorSystem.h"
#include "Engine/ResourceImageSubstractRGBAndAlpha.h"
#include "Engine/ResourceImageSubstract.h"

#include "Plugins/MoviePlugin/ResourceMovie2.h"

#include "Interface/ApplicationInterface.h"
#include "Interface/PickerTrapInterface.h"

#include "Engine/HotSpot.h"
#include "Engine/HotSpotPolygon.h"
#include "Engine/HotSpotCircle.h"
#include "Engine/HotSpotBubbles.h"
#include "Engine/HotSpotImage.h"
#include "Engine/HotSpotShape.h"
#include "Engine/Landscape2D.h"
#include "Engine/Grid2D.h"

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
#include "DocumentTraceback.h"

#include "Environment/Python/PythonAnimatableEventReceiver.h"
#include "Environment/Python/PythonEventReceiver.h"

#include "Engine/SurfaceSound.h"
#include "Engine/SurfaceImage.h"
#include "Engine/SurfaceImageSequence.h"
#include "Engine/SurfaceTrackMatte.h"
#include "Engine/SurfaceSolidColor.h"

#include "Kernel/Interender.h"
#include "Kernel/Isometric.h"
#include "Kernel/Parallax.h"
#include "Kernel/RenderViewport.h"
#include "Kernel/RenderScissor.h"
#include "Kernel/RenderCameraOrthogonal.h"
#include "Kernel/RenderCameraProjection.h"
#include "Kernel/RenderCameraOrthogonalTarget.h"
#include "Kernel/ResourceImage.h"

#include "Engine/Layer2D.h"
#include "Engine/Meshget.h"
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
#include "Kernel/MemoryHelper.h"
#include "Kernel/ValueFollower.h"

#include "math/angle.h"
#include "math/vec4.h"
#include "math/mat3.h"
#include "math/mat4.h"
#include "math/quat.h"
#include "math/utils.h"

#include "Kernel/Rect.h"
#include "Kernel/String.h"
#include "Kernel/Polygon.h"
#include "Kernel/ValueFollower.h"

#include "Kernel/FactoryPool.h"
#include "Kernel/AssertionFactory.h"

#include "stdex/xml_sax_parser.h"
#include "utf8.h"

#include <math.h>

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class NodeScriptMethod
        : public Factorable
    {
    public:
        NodeScriptMethod()
        {
            m_factoryPythonScheduleEvent = new FactoryPool<PythonScheduleEvent, 16>();
            m_factoryDelaySchedulePipe = new FactoryPool<DelaySchedulePipe, 16>();
            m_factoryPythonScheduleTiming = new FactoryPool<PythonScheduleTiming, 16>();
            m_factoryPythonSchedulePipe = new FactoryPool<PythonSchedulePipe, 16>();
            m_factoryNodeAffectorCallback = new FactoryPool<ScriptableAffectorCallback, 4>();
        }

        ~NodeScriptMethod() override
        {
            MENGINE_ASSERTION_FACTORY_EMPTY( m_factoryPythonScheduleEvent );
            MENGINE_ASSERTION_FACTORY_EMPTY( m_factoryDelaySchedulePipe );
            MENGINE_ASSERTION_FACTORY_EMPTY( m_factoryPythonScheduleTiming );
            MENGINE_ASSERTION_FACTORY_EMPTY( m_factoryPythonSchedulePipe );
            MENGINE_ASSERTION_FACTORY_EMPTY( m_factoryNodeAffectorCallback );

            m_factoryPythonScheduleEvent = nullptr;
            m_factoryDelaySchedulePipe = nullptr;
            m_factoryPythonScheduleTiming = nullptr;
            m_factoryPythonSchedulePipe = nullptr;
            m_factoryNodeAffectorCallback = nullptr;
        }

    public:
        //////////////////////////////////////////////////////////////////////////
        PyObject * s_TextField_setTextFormatArgs( pybind::kernel_interface * _kernel, TextField * _textField, PyObject * _args, PyObject * _kwds )
        {
            MENGINE_UNUSED( _kwds );

            size_t args_count = _kernel->tuple_size( _args );

            VectorString cs_args;
            cs_args.reserve( args_count );

            for( uint32_t it = 0; it != args_count; ++it )
            {
                PyObject * py_string = _kernel->tuple_getitem( _args, it );

                if( _kernel->string_check( py_string ) == true )
                {
                    String key;
                    if( pybind::extract_value( _kernel, py_string, key, false ) == false )
                    {
                        LOGGER_ERROR( "textfield_setTextFormatArgs %s invalid get str %s"
                            , _kernel->object_repr( py_string )
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
                        LOGGER_ERROR( "textfield_setTextFormatArgs %s invalid get unicode %s"
                            , _kernel->object_repr( py_string )
                        );

                        return _kernel->ret_false();
                    }

                    String utf8_arg;
                    Helper::unicodeToUtf8( key, utf8_arg );

                    cs_args.emplace_back( utf8_arg );
                }
                else
                {
                    const char * value = _kernel->object_str( py_string );

                    if( value == nullptr )
                    {
                        LOGGER_ERROR( "textfield_setTextFormatArgs %s not suport arg %s"
                            , _kernel->object_repr( py_string )
                        );

                        return _kernel->ret_false();
                    }

                    cs_args.emplace_back( String( value ) );
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
                Helper::utf8ToUnicode( str_arg, unicode );

                ws_args.emplace_back( unicode );
            }

            return ws_args;
        }
        //////////////////////////////////////////////////////////////////////////
        //WString textfield_getText( TextField * _textField )
        //{
        //	const U32String & utf8 = _textField->getText();

        //	WString unicode;
        //	Helper::utf8ToUnicode( utf8, unicode );

        //	return unicode;
        //}
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
        void s_Transformation_setAngleDeg( Transformation * _transformation, float _angle )
        {
            float rad = _angle * mt::constant::deg2rad;

            _transformation->setLocalOrientationX( rad );
        }
        //////////////////////////////////////////////////////////////////////////
        void s_Transformation_removeRelationTransformation( Transformation * _transformation )
        {
            _transformation->removeRelationTransformation();
        }
        //////////////////////////////////////////////////////////////////////////
        const ConstString & s_Resource_getCategory( Resource * _resource )
        {
            const FileGroupInterfacePtr & fileGroup = _resource->getFileGroup();

            const ConstString & categoryName = fileGroup->getName();

            return categoryName;
        }
        //////////////////////////////////////////////////////////////////////////
        void Transformation_coordinate( Transformation * _transformation, const mt::vec3f & _coordinate )
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
            polygon.to_box2f( bb );

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

            const mt::mat4f & wm = _hs->getWorldMatrix();

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

            mt::box2f b1;
            _hs->getScreenPolygon( camera, viewport, contentResolution, &b1, nullptr );

            mt::vec2f c;
            mt::get_center_box( b1, c );

            return c;
        }
        //////////////////////////////////////////////////////////////////////////
        Polygon s_HotSpotPolygon_getWorldPolygon( HotSpotPolygon * _hs )
        {
            const Polygon & polygon = _hs->getPolygon();

            const mt::mat4f & wm = _hs->getWorldMatrix();

            Polygon pwm;
            polygon.mul_wm( pwm, wm );

            return pwm;
        }
        //////////////////////////////////////////////////////////////////////////
        const mt::vec2f & s_Shape_getSurfaceSize( Shape * _shape )
        {
            const SurfacePtr & surface = _shape->getSurface();

            if( surface == nullptr )
            {
                LOGGER_ERROR( "s_getSurfaceSize shape %s not setup surface"
                    , _shape->getName().c_str()
                );

                return mt::vec2f::identity();
            }

            const mt::vec2f & size = surface->getSize();

            return size;
        }
        //////////////////////////////////////////////////////////////////////////
        mt::vec2f s_Shape_getLocalImageCenter( Shape * _shape )
        {
            const SurfacePtr & surface = _shape->getSurface();

            if( surface == nullptr )
            {
                LOGGER_ERROR( "s_getLocalImageCenter shape %s not setup surface"
                    , _shape->getName().c_str()
                );

                return mt::vec2f::identity();
            }

            const mt::vec2f & offset = surface->getOffset();

            const mt::vec2f & size = surface->getSize();

            mt::vec2f center = offset + size * 0.5f;

            return center;
        }
        //////////////////////////////////////////////////////////////////////////
        mt::vec2f s_Shape_getWorldImageCenter( Shape * _shape )
        {
            mt::vec2f imageCenter = s_Shape_getLocalImageCenter( _shape );

            const mt::mat4f & wm = _shape->getWorldMatrix();

            mt::vec2f imageCenter_wm;
            mt::mul_v2_v2_m4( imageCenter_wm, imageCenter, wm );

            return imageCenter_wm;
        }
        //////////////////////////////////////////////////////////////////////////
        NodeAffectorCreator::NodeAffectorCreatorInterpolateLinear<mt::vec4f> m_nodeAffectorCreatorInterpolateLinearVec4;
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
        uint32_t s_ShapeQuadFlex_setPercentVisibilityTo( ShapeQuadFlex * _shape, float _time, const mt::vec4f& _percent, const ConstString & _easingType, const pybind::object & _cb, const pybind::args & _args )
        {
            if( _shape->isActivate() == false )
            {
                return 0;
            }

            EasingInterfacePtr easing = VOCABULARY_GET( STRINGIZE_STRING_LOCAL( "Easing" ), _easingType );

            ScriptableAffectorCallbackPtr callback = createNodeAffectorCallback( _shape, _cb, _args );

            AffectorPtr affector = m_nodeAffectorCreatorInterpolateLinearVec4.create( ETA_VISIBILITY
                , easing
                , callback
                , [_shape]( const mt::vec4f & _v ) { _shape->setPercentVisibility( _v ); }
                , _shape->getPercentVisibility(), _percent, _time
            );

            if( affector == nullptr )
            {
                return 0;
            }

            _shape->stopAffectors( ETA_VISIBILITY );

            if( _shape->isActivate() == false )
            {
                return 0;
            }

            AFFECTOR_ID id = _shape->addAffector( affector );

            return id;
        }
        //////////////////////////////////////////////////////////////////////////
        void s_ShapeQuadFlex_setPercentVisibilityStop( ShapeQuadFlex * _shape )
        {
            _shape->stopAffectors( ETA_VISIBILITY );
        }
        //////////////////////////////////////////////////////////////////////////
        FactoryPtr m_factoryPythonScheduleEvent;
        //////////////////////////////////////////////////////////////////////////
        uint32_t ScheduleInterface_schedule( SchedulerInterface * _scheduleManager, float _timing, const pybind::object & _script, const pybind::args & _args )
        {
            PythonScheduleEventPtr sl = m_factoryPythonScheduleEvent->createObject( MENGINE_DOCUMENT_PYBIND );

            sl->initialize( _script, _args );

            uint32_t id = _scheduleManager->event( _timing, sl );

            return id;
        }
        //////////////////////////////////////////////////////////////////////////
        FactoryPtr m_factoryDelaySchedulePipe;
        FactoryPtr m_factoryPythonScheduleTiming;
        //////////////////////////////////////////////////////////////////////////
        uint32_t ScheduleInterface_timing( SchedulerInterface * _scheduleManager, float _delay, const pybind::object & _timing, const pybind::object & _event, const pybind::args & _args )
        {
            DelaySchedulePipePtr pipe = m_factoryDelaySchedulePipe->createObject( MENGINE_DOCUMENT_PYBIND );
            pipe->initialize( _delay );

            PythonScheduleTimingPtr py_timing = m_factoryPythonScheduleTiming->createObject( MENGINE_DOCUMENT_PYBIND );
            py_timing->initialize( _timing, _args );

            PythonScheduleEventPtr py_event = m_factoryPythonScheduleEvent->createObject( MENGINE_DOCUMENT_PYBIND );
            py_event->initialize( _event, _args );

            uint32_t id = _scheduleManager->timing( pipe, py_timing, py_event );

            return id;
        }
        //////////////////////////////////////////////////////////////////////////
        FactoryPtr m_factoryPythonSchedulePipe;
        //////////////////////////////////////////////////////////////////////////
        uint32_t ScheduleInterface_pipe( SchedulerInterface * _scheduleManager, const pybind::object & _pipe, const pybind::object & _timing, const pybind::object & _event, const pybind::args & _args )
        {
            PythonSchedulePipePtr py_pipe = m_factoryPythonSchedulePipe->createObject( MENGINE_DOCUMENT_PYBIND );
            py_pipe->initialize( _pipe, _args );

            PythonScheduleTimingPtr py_timing = m_factoryPythonScheduleTiming->createObject( MENGINE_DOCUMENT_PYBIND );
            py_timing->initialize( _timing, _args );

            PythonScheduleEventPtr py_event;
            
            if( _event.is_none() == false )
            {
                py_event = m_factoryPythonScheduleEvent->createObject( MENGINE_DOCUMENT_PYBIND );
                py_event->initialize( _event, _args );
            }

            uint32_t id = _scheduleManager->timing( py_pipe, py_timing, py_event );

            return id;
        }
        //////////////////////////////////////////////////////////////////////////
        class PythonSceneChangeCallback
            : public SceneChangeCallbackInterface
        {
        public:
            void initialize( const pybind::object & _cb, const pybind::args & _args )
            {
                m_cb = _cb;
                m_args = _args;
            }

        public:
            void onSceneChange( const ScenePtr & _scene, bool _enable, bool _remove ) override
            {
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
                ->getTime();

            uint32_t id = _animation->play( time );

            return id;
        }
        //////////////////////////////////////////////////////////////////////////
        void s_Animation_resume( AnimationInterface * _animation )
        {
            float time = TIMELINE_SERVICE()
                ->getTime();

            _animation->resume( time );
        }
        //////////////////////////////////////////////////////////////////////////
        PyObject * s_SurfaceImageSequence_setEventListener( pybind::kernel_interface * _kernel, SurfaceImageSequence * _surface, PyObject * _args, PyObject * _kwds )
        {
            MENGINE_UNUSED( _args );

            if( _kwds == nullptr )
            {
                return _kernel->ret_none();
            }

            pybind::dict py_kwds( _kernel, _kwds );
            Helper::registerAnimatableEventReceiver<>( _kernel, py_kwds, _surface );

#ifdef MENGINE_DEBUG
            if( py_kwds.empty() == false )
            {
                for( pybind::dict::iterator
                    it = py_kwds.begin(),
                    it_end = py_kwds.end();
                    it != it_end;
                    ++it )
                {
                    String k = it.key();

                    LOGGER_ERROR( "surface '%s' invalid kwds '%s'"
                        , _surface->getName().c_str()
                        , k.c_str()
                    );
                }

                throw;
            }
#endif

            return _kernel->ret_none();
        }
        //////////////////////////////////////////////////////////////////////////
        PyObject * s_SurfaceSound_setEventListener( pybind::kernel_interface * _kernel, SurfaceSound * _surface, PyObject * _args, PyObject * _kwds )
        {
            MENGINE_UNUSED( _args );

            if( _kwds == nullptr )
            {
                return _kernel->ret_none();
            }

            pybind::dict py_kwds( _kernel, _kwds );
            Helper::registerAnimatableEventReceiver<>( _kernel, py_kwds, _surface );

#ifdef MENGINE_DEBUG
            if( py_kwds.empty() == false )
            {
                for( pybind::dict::iterator
                    it = py_kwds.begin(),
                    it_end = py_kwds.end();
                    it != it_end;
                    ++it )
                {
                    String k = it.key();

                    LOGGER_ERROR( "surface '%s' invalid kwds '%s'"
                        , _surface->getName().c_str()
                        , k.c_str()
                    );
                }

                throw;
            }
#endif

            return _kernel->ret_none();
        }
        //////////////////////////////////////////////////////////////////////////
        class PythonMeshEventReceiver
            : public PythonEventReceiver
            , public MeshgetEventReceiver
            , public Factorable
        {
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

            if( _kwds == nullptr )
            {
                return _kernel->ret_none();
            }

            pybind::dict py_kwds( _kernel, _kwds );
            Helper::registerPythonEventReceiver<PythonMeshEventReceiver>( _kernel, py_kwds, _node, "onMeshgetUpdate", EVENT_MESHGET_UPDATE );

#ifdef MENGINE_DEBUG
            if( py_kwds.empty() == false )
            {
                for( pybind::dict::iterator
                    it = py_kwds.begin(),
                    it_end = py_kwds.end();
                    it != it_end;
                    ++it )
                {
                    String k = it.key();

                    LOGGER_ERROR( "node '%s' invalid kwds '%s'"
                        , _node->getName().c_str()
                        , k.c_str()
                    );
                }

                throw;
            }
#endif

            return _kernel->ret_none();
        }
        //////////////////////////////////////////////////////////////////////////
        class PythonScriptHolderEventReceiver
            : public PythonEventReceiver
            , public ScriptHolderEventReceiver
            , public Factorable
        {
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

            if( _kwds == nullptr )
            {
                return _kernel->ret_none();
            }

            pybind::dict py_kwds( _kernel, _kwds );
            Helper::registerPythonEventReceiver<PythonScriptHolderEventReceiver>( _kernel, py_kwds, _node, "onKeepScript", EVENT_KEEP_SCRIPT );
            Helper::registerPythonEventReceiver<PythonScriptHolderEventReceiver>( _kernel, py_kwds, _node, "onReleaseScript", EVENT_RELEASE_SCRIPT );

#ifdef MENGINE_DEBUG
            if( py_kwds.empty() == false )
            {
                for( pybind::dict::iterator
                    it = py_kwds.begin(),
                    it_end = py_kwds.end();
                    it != it_end;
                    ++it )
                {
                    String k = it.key();

                    LOGGER_ERROR( "node '%s' invalid kwds '%s'"
                        , _node->getName().c_str()
                        , k.c_str()
                    );
                }

                throw;
            }
#endif

            return _kernel->ret_none();
        }
        //////////////////////////////////////////////////////////////////////////
        PyObject * s_HotSpot_setEventListener( pybind::kernel_interface * _kernel, HotSpot * _node, PyObject * _args, PyObject * _kwds )
        {
            MENGINE_UNUSED( _args );

            if( _kwds == nullptr )
            {
                return _kernel->ret_none();
            }

            pybind::dict py_kwds( _kernel, _kwds );

            Helper::registerPythonEventReceiver<PythonHotSpotEventReceiver>( _kernel, py_kwds, _node, "onHandleKeyEvent", EVENT_KEY );
            Helper::registerPythonEventReceiver<PythonHotSpotEventReceiver>( _kernel, py_kwds, _node, "onHandleMouseButtonEvent", EVENT_MOUSE_BUTTON );
            Helper::registerPythonEventReceiver<PythonHotSpotEventReceiver>( _kernel, py_kwds, _node, "onHandleMouseButtonEventBegin", EVENT_MOUSE_BUTTON_BEGIN );
            Helper::registerPythonEventReceiver<PythonHotSpotEventReceiver>( _kernel, py_kwds, _node, "onHandleMouseButtonEventEnd", EVENT_MOUSE_BUTTON_END );
            Helper::registerPythonEventReceiver<PythonHotSpotEventReceiver>( _kernel, py_kwds, _node, "onHandleMouseMove", EVENT_MOUSE_MOVE );
            Helper::registerPythonEventReceiver<PythonHotSpotEventReceiver>( _kernel, py_kwds, _node, "onHandleMouseWheel", EVENT_MOUSE_WHEEL );
            Helper::registerPythonEventReceiver<PythonHotSpotEventReceiver>( _kernel, py_kwds, _node, "onHandleMouseEnter", EVENT_MOUSE_ENTER );
            Helper::registerPythonEventReceiver<PythonHotSpotEventReceiver>( _kernel, py_kwds, _node, "onHandleMouseLeave", EVENT_MOUSE_LEAVE );
            Helper::registerPythonEventReceiver<PythonHotSpotEventReceiver>( _kernel, py_kwds, _node, "onHandleMouseOverDestroy", EVENT_MOUSE_OVER_DESTROY );
            Helper::registerPythonEventReceiver<PythonHotSpotEventReceiver>( _kernel, py_kwds, _node, "onActivate", EVENT_ACTIVATE );
            Helper::registerPythonEventReceiver<PythonHotSpotEventReceiver>( _kernel, py_kwds, _node, "onDeactivate", EVENT_DEACTIVATE );

#ifdef MENGINE_DEBUG
            if( py_kwds.empty() == false )
            {
                for( pybind::dict::iterator
                    it = py_kwds.begin(),
                    it_end = py_kwds.end();
                    it != it_end;
                    ++it )
                {
                    String k = it.key();

                    LOGGER_ERROR( "node '%s' kwds '%s'"
                        , _node->getName().c_str()
                        , k.c_str()
                    );
                }

                throw;
            }
#endif

            return _kernel->ret_none();
        }
        //////////////////////////////////////////////////////////////////////////
        PyObject * s_HotSpot_removeEventListener( pybind::kernel_interface * _kernel, HotSpot * _node, PyObject * _args, PyObject * _kwds )
        {
            MENGINE_UNUSED( _args );
            MENGINE_UNUSED( _kwds );

            _node->removeEvents();

            return _kernel->ret_none();
        }
        //////////////////////////////////////////////////////////////////////////
        bool s_Node_removeChild( Node * _node, const NodePtr & _child )
        {
            if( _node->removeChild( _child ) == false )
            {
                return false;
            }

            return true;
        }
        //////////////////////////////////////////////////////////////////////////
        void s_Node_removeAllChild( Node * _node )
        {
            _node->removeChildren( []( const NodePtr & _child )
            {
                MENGINE_UNUSED( _child );
            } );
        }
        //////////////////////////////////////////////////////////////////////////
        bool s_Node_removeFromParent( Node * _node )
        {
            if( _node->removeFromParent() == false )
            {
                return false;
            }

            return true;
        }
        //////////////////////////////////////////////////////////////////////////
        void s_Node_destroyAllChild( Node * _node )
        {
            _node->destroyChildren( []( const NodePtr & _child )
            {
                MENGINE_UNUSED( _child );
            } );
        }
        //////////////////////////////////////////////////////////////////////////
        bool s_Node_isHomeless( Node * _node )
        {
            return _node->hasParent() == false;
        }
        //////////////////////////////////////////////////////////////////////////
        mt::vec3f s_Node_getWorldOffsetPosition( Node * _node, const mt::vec3f & _position )
        {
            const mt::vec3f & wp = _node->getWorldPosition();

            mt::vec3f offset = _position - wp;

            return offset;
        }
        //////////////////////////////////////////////////////////////////////////
        float s_Node_getLengthTo( Node * _node, const mt::vec3f & _position )
        {
            const mt::vec3f & wp = _node->getWorldPosition();

            float length = mt::length_v3_v3( wp, _position );

            return length;
        }
        //////////////////////////////////////////////////////////////////////////
        std::string s_Node_getDebugId( Node * _node )
        {
            char debugId[256];
            sprintf( debugId, "%p", _node );

            return std::string( debugId );
        }
        //////////////////////////////////////////////////////////////////////////
        NodePtr s_Node_createChildren( Node * _node, const ConstString & _type )
        {
            NodePtr node = PROTOTYPE_SERVICE()
                ->generatePrototype( STRINGIZE_STRING_LOCAL( "Node" ), _type, MENGINE_DOCUMENT_PYBIND );

            MENGINE_ASSERTION_MEMORY_PANIC( node, nullptr );

            node->disable();

            _node->addChild( node );

            return node;
        }
        //////////////////////////////////////////////////////////////////////////
        pybind::list s_Node_getAllChildren( pybind::kernel_interface * _kernel, Node * _node )
        {
            IntrusiveSlugListNodeChild & children = _node->getChildren();

            IntrusiveSlugListNodeChild::size_type size = children.size();

            pybind::list py_children( _kernel, size );

            uint32_t index = 0;

            for( IntrusiveSlugChild it( children ); it.eof() == false; )
            {
                NodePtr child( *it );

                it.next_shuffle();

                py_children[index++] = child;
            }

            return py_children;
        }
        //////////////////////////////////////////////////////////////////////////
        void s_Node_moveStop( Node * _node )
        {
            _node->stopAffectors( ETA_POSITION );
            _node->setLinearSpeed( mt::vec3f( 0.f, 0.f, 0.f ) );
        }
        //////////////////////////////////////////////////////////////////////////
        NodeAffectorCreator::NodeAffectorCreatorAccumulateLinear<mt::vec3f> m_nodeAffectorCreatorAccumulateLinear;
        //////////////////////////////////////////////////////////////////////////
        uint32_t s_Node_velocityTo( Node * _node, float _speed, const mt::vec3f& _dir, const ConstString & _easingType, const pybind::object & _cb, const pybind::args & _args )
        {
            if( _node->isActivate() == false )
            {
                return 0;
            }

            if( _node->isAfterActive() == false )
            {
                return 0;
            }

            EasingInterfacePtr easing = VOCABULARY_GET( STRINGIZE_STRING_LOCAL( "Easing" ), _easingType );

            ScriptableAffectorCallbackPtr callback = createNodeAffectorCallback( _node, _cb, _args );

            AffectorPtr affector = m_nodeAffectorCreatorAccumulateLinear.create( ETA_POSITION
                , easing
                , callback
                , [_node]( const mt::vec3f & _v ) { _node->setLocalPosition( _v ); }
                , _node->getLocalPosition(), _dir, _speed
            );

            if( affector == nullptr )
            {
                return 0;
            }

            s_Node_moveStop( _node );

            if( _node->isActivate() == false )
            {
                return 0;
            }

            mt::vec3f linearSpeed = _dir * _speed;
            _node->setLinearSpeed( linearSpeed );

            AFFECTOR_ID id = _node->addAffector( affector );

            return id;
        }
        //////////////////////////////////////////////////////////////////////////
        class AffectorVelocity2
            : public BaseAffector
        {
        public:
            AffectorVelocity2()
                : m_node( nullptr )
                , m_velocity( 0.f, 0.f, 0.f )
                , m_time( 0.f )
            {
            }

        public:
            void setNode( const NodePtr & _node )
            {
                m_node = _node;
            }

        public:
            void initialize( const mt::vec3f & _velocity, float _time )
            {
                m_velocity = _velocity;
                m_time = _time;
            }

        protected:
            bool _affect( const UpdateContext * _context, float * _used ) override
            {
                if( m_time - _context->time < 0.f )
                {
                    m_node->translate( m_velocity * _context->time );

                    m_time -= _context->time;

                    *_used = _context->time;

                    return false;
                }

                m_node->translate( m_velocity * m_time );

                *_used = m_time;

                return true;
            }

        protected:
            NodePtr m_node;

            mt::vec3f m_velocity;
            float m_time;
        };
        //////////////////////////////////////////////////////////////////////////
        typedef IntrusivePtr<AffectorVelocity2> AffectorVelocity2Ptr;
        //////////////////////////////////////////////////////////////////////////
        class FactoryAffectorVelocity2
        {
        public:
            FactoryAffectorVelocity2()
            {
                m_factory = new FactoryPool<AffectorVelocity2, 4>();
            }

        public:
            AffectorPtr create( EAffectorType _type
                , const EasingInterfacePtr & _easing
                , const AffectorCallbackInterfacePtr & _cb
                , const NodePtr & _node, const mt::vec3f & _velocity, float _time, const Char * _doc )
            {
                AffectorVelocity2Ptr affector = m_factory->createObject( _doc );

                affector->setAffectorType( _type );
                affector->setEasing( _easing );
                affector->setCallback( _cb );
                affector->setNode( _node );

                affector->initialize( _velocity, _time );

                return affector;
            }

        protected:
            FactoryPtr m_factory;
        };
        //////////////////////////////////////////////////////////////////////////
        FactoryAffectorVelocity2 m_factoryAffectorVelocity2;
        //////////////////////////////////////////////////////////////////////////
        uint32_t s_Node_velocityTo2( Node * _node, const mt::vec3f & _velocity, float _time, const ConstString & _easingType, const pybind::object & _cb, const pybind::args & _args )
        {
            if( _node->isActivate() == false )
            {
                return 0;
            }

            if( _node->isAfterActive() == false )
            {
                return 0;
            }

            EasingInterfacePtr easing = VOCABULARY_GET( STRINGIZE_STRING_LOCAL( "Easing" ), _easingType );

            ScriptableAffectorCallbackPtr callback = createNodeAffectorCallback( _node, _cb, _args );

            AffectorPtr affector = m_factoryAffectorVelocity2.create( ETA_POSITION
                , easing
                , callback
                , NodePtr( _node ), _velocity, _time, MENGINE_DOCUMENT_PYBIND
            );

            if( affector == nullptr )
            {
                return 0;
            }

            s_Node_moveStop( _node );

            if( _node->isActivate() == false )
            {
                return 0;
            }

            _node->setLinearSpeed( _velocity );

            AFFECTOR_ID id = _node->addAffector( affector );

            return id;
        }
        //////////////////////////////////////////////////////////////////////////
        NodeAffectorCreator::NodeAffectorCreatorInterpolateLinear<mt::vec3f> m_nodeAffectorCreatorInterpolateLinear;
        //////////////////////////////////////////////////////////////////////////
        uint32_t s_Node_moveTo( Node * _node, float _time, const mt::vec3f& _point, const ConstString & _easingType, const pybind::object & _cb, const pybind::args & _args )
        {
            if( _node->isActivate() == false )
            {
                return 0;
            }

            if( _node->isAfterActive() == false )
            {
                return 0;
            }

            EasingInterfacePtr easing = VOCABULARY_GET( STRINGIZE_STRING_LOCAL( "Easing" ), _easingType );

            ScriptableAffectorCallbackPtr callback = createNodeAffectorCallback( _node, _cb, _args );

            AffectorPtr affector =
                m_nodeAffectorCreatorInterpolateLinear.create( ETA_POSITION
                    , easing
                    , callback
                    , [_node]( const mt::vec3f & _v ) { _node->setLocalPosition( _v ); }
                    , _node->getLocalPosition(), _point, _time
                );

            if( affector == nullptr )
            {
                return 0;
            }

            s_Node_moveStop( _node );

            if( _node->isActivate() == false )
            {
                return 0;
            }

            float invTime = 1.f / _time;
            const mt::vec3f & pos = _node->getLocalPosition();
            mt::vec3f linearSpeed = (_point - pos) * invTime;

            _node->setLinearSpeed( linearSpeed );

            AFFECTOR_ID id = _node->addAffector( affector );

            return id;
        }
        //////////////////////////////////////////////////////////////////////////
        NodeAffectorCreator::NodeAffectorCreatorInterpolateQuadratic<mt::vec3f> m_nodeAffectorCreatorInterpolateQuadratic;
        //////////////////////////////////////////////////////////////////////////
        uint32_t s_Node_accMoveTo( Node * _node, float _time, const mt::vec3f& _point, const ConstString & _easingType, const pybind::object & _cb, const pybind::args & _args )
        {
            if( _node->isActivate() == false )
            {
                return 0;
            }

            if( _node->isAfterActive() == false )
            {
                return 0;
            }

            EasingInterfacePtr easing = VOCABULARY_GET( STRINGIZE_STRING_LOCAL( "Easing" ), _easingType );

            const mt::vec3f & linearSpeed = _node->getLinearSpeed();

            ScriptableAffectorCallbackPtr callback = createNodeAffectorCallback( _node, _cb, _args );

            AffectorPtr affector = m_nodeAffectorCreatorInterpolateQuadratic.create( ETA_POSITION
                , easing
                , callback
                , [_node]( const mt::vec3f & _v ) { _node->setLocalPosition( _v ); }
                , _node->getLocalPosition(), _point, linearSpeed, _time
            );

            if( affector == nullptr )
            {
                return 0;
            }

            s_Node_moveStop( _node );

            if( _node->isActivate() == false )
            {
                return 0;
            }

            AFFECTOR_ID id = _node->addAffector( affector );

            return id;
        }
        //////////////////////////////////////////////////////////////////////////
        NodeAffectorCreator::NodeAffectorCreatorInterpolateBezier<mt::vec3f, 1> m_nodeAffectorCreatorInterpolateQuadraticBezier;
        //////////////////////////////////////////////////////////////////////////
        uint32_t s_Node_bezier2To( Node * _node
            , float _time
            , const mt::vec3f& _to
            , const mt::vec3f& _v0
            , const ConstString & _easingType
            , const pybind::object & _cb
            , const pybind::args & _args )
        {
            if( _node->isActivate() == false )
            {
                return 0;
            }

            if( _node->isAfterActive() == false )
            {
                return 0;
            }

            EasingInterfacePtr easing = VOCABULARY_GET( STRINGIZE_STRING_LOCAL( "Easing" ), _easingType );

            const mt::vec3f & from = _node->getLocalPosition();

            ScriptableAffectorCallbackPtr callback = createNodeAffectorCallback( _node, _cb, _args );

            AffectorPtr affector = m_nodeAffectorCreatorInterpolateQuadraticBezier.create( ETA_POSITION
                , easing
                , callback
                , [_node]( const mt::vec3f & _v ) { _node->setLocalPosition( _v ); }
            , [from]() { return from; }
            , [_to]() { return _to; }
            , [_v0]( mt::vec3f * _v ) { _v[0] = _v0; }
                , _time
                );

            if( affector == nullptr )
            {
                return 0;
            }

            s_Node_moveStop( _node );

            if( _node->isActivate() == false )
            {
                return 0;
            }

            AFFECTOR_ID id = _node->addAffector( affector );

            return id;
        }
        //////////////////////////////////////////////////////////////////////////
        uint32_t s_Node_bezier2Follower( Node * _node
            , float _time
            , const NodePtr & _follow
            , const mt::vec3f & _offset
            , const ConstString & _easingType
            , const pybind::object & _cb
            , const pybind::args & _args )
        {
            if( _follow == nullptr )
            {
                return 0;
            }

            if( _node->isActivate() == false )
            {
                return 0;
            }

            if( _node->isAfterActive() == false )
            {
                return 0;
            }

            EasingInterfacePtr easing = VOCABULARY_GET( STRINGIZE_STRING_LOCAL( "Easing" ), _easingType );

            const mt::vec3f & node_pos = _node->getWorldPosition();

            ScriptableAffectorCallbackPtr callback = createNodeAffectorCallback( _node, _cb, _args );

            AffectorPtr affector = m_nodeAffectorCreatorInterpolateQuadraticBezier.create( ETA_POSITION
                , easing
                , callback
                , [_node]( const mt::vec3f & _v ) { _node->setWorldPosition( _v ); }
            , [node_pos]() { return node_pos; }
            , [_follow, _offset]() { return _follow->getWorldPosition() + _offset; }
                , [node_pos, _follow, _offset]( mt::vec3f * _v )
            {
                float x = _follow->getWorldPosition().x + _offset.x;

                _v[0] = mt::vec3f( x, node_pos.y, 0.f );
            }
                , _time
                );

            if( affector == nullptr )
            {
                return 0;
            }

            s_Node_moveStop( _node );

            if( _node->isActivate() == false )
            {
                return 0;
            }

            AFFECTOR_ID id = _node->addAffector( affector );

            return id;
        }
        //////////////////////////////////////////////////////////////////////////
        NodeAffectorCreator::NodeAffectorCreatorInterpolateBezier<mt::vec3f, 2> m_nodeAffectorCreatorInterpolateCubicBezier;
        //////////////////////////////////////////////////////////////////////////
        uint32_t s_Node_bezier3To( Node * _node
            , float _time
            , const mt::vec3f& _to
            , const mt::vec3f& _v0
            , const mt::vec3f& _v1
            , const ConstString & _easingType
            , const pybind::object & _cb
            , const pybind::args & _args )
        {
            if( _node->isActivate() == false )
            {
                return 0;
            }

            if( _node->isAfterActive() == false )
            {
                return 0;
            }

            EasingInterfacePtr easing = VOCABULARY_GET( STRINGIZE_STRING_LOCAL( "Easing" ), _easingType );

            ScriptableAffectorCallbackPtr callback = createNodeAffectorCallback( _node, _cb, _args );

            AffectorPtr affector =
                m_nodeAffectorCreatorInterpolateCubicBezier.create( ETA_POSITION
                    , easing
                    , callback
                    , [_node]( const mt::vec3f & _v ) { _node->setLocalPosition( _v ); }
            , [_node]() {return _node->getLocalPosition(); }
            , [_to]() {return _to; }
            , [_v0, _v1]( mt::vec3f * _v ) {_v[0] = _v0; _v[1] = _v1; }
                , _time
                );

            if( affector == nullptr )
            {
                return 0;
            }

            s_Node_moveStop( _node );

            if( _node->isActivate() == false )
            {
                return 0;
            }

            AFFECTOR_ID id = _node->addAffector( affector );

            return id;
        }
        //////////////////////////////////////////////////////////////////////////
        NodeAffectorCreator::NodeAffectorCreatorInterpolateBezier<mt::vec3f, 3> m_nodeAffectorCreatorInterpolateQuarticBezier;
        //////////////////////////////////////////////////////////////////////////
        uint32_t s_Node_bezier4To( Node * _node
            , float _time
            , const mt::vec3f& _to
            , const mt::vec3f& _v0
            , const mt::vec3f& _v1
            , const mt::vec3f& _v2
            , const ConstString & _easingType
            , const pybind::object & _cb
            , const pybind::args & _args )
        {
            if( _node->isActivate() == false )
            {
                return 0;
            }

            if( _node->isAfterActive() == false )
            {
                return 0;
            }

            EasingInterfacePtr easing = VOCABULARY_GET( STRINGIZE_STRING_LOCAL( "Easing" ), _easingType );

            ScriptableAffectorCallbackPtr callback = createNodeAffectorCallback( _node, _cb, _args );

            AffectorPtr affector =
                m_nodeAffectorCreatorInterpolateQuarticBezier.create( ETA_POSITION
                    , easing
                    , callback
                    , [_node]( const mt::vec3f & _v ) { _node->setLocalPosition( _v ); }
            , [_node]() { return _node->getLocalPosition(); }
            , [_to]() { return _to; }
            , [_v0, _v1, _v2]( mt::vec3f * _v ) {_v[0] = _v0; _v[1] = _v1; _v[2] = _v2; }
                , _time
                );

            if( affector == nullptr )
            {
                return 0;
            }

            s_Node_moveStop( _node );

            if( _node->isActivate() == false )
            {
                return 0;
            }

            AFFECTOR_ID id = _node->addAffector( affector );

            return id;
        }
        //////////////////////////////////////////////////////////////////////////
        class AffectorCreatorInterpolateParabolic
            : public BaseAffector
        {
        public:
            AffectorCreatorInterpolateParabolic()
                : m_speed( 5.f )
            {
            }

        public:
            void setNode( const NodePtr & _node )
            {
                m_node = _node;
            }

        public:
            void initialize( const mt::vec3f & _end, const mt::vec3f & _v0, float _time )
            {
                const mt::vec3f & start_position = m_node->getLocalPosition();

                m_interpolator.start( start_position, _end, _v0, _time );

                mt::vec3f next_position;
                m_interpolator.step( 100.f, &next_position );

                mt::vec3f dir;
                mt::dir_v3_v3( dir, start_position, next_position );

                m_prevDir = dir;
                m_currentDir = dir;
                m_targetDir = dir;

                m_node->setBillboard( dir, mt::vec3f( 0.f, 1.f, 1.f ) );
            }

        protected:
            bool _affect( const UpdateContext * _context, float * _used ) override
            {
                const EasingInterfacePtr & easing = this->getEasing();

                mt::vec3f position;
                bool finish = m_interpolator.update( easing, _context, &position, _used );

                this->updateDirection_( _context, position );
                this->updatePosition_( position );

                if( finish == false )
                {
                    return false;
                }

                return true;
            }

            void _stop() override
            {
                m_interpolator.stop();
            }

        protected:
            void updateDirection_( const UpdateContext * _context, const mt::vec3f & _position )
            {
                const mt::vec3f & prev_position = m_node->getLocalPosition();

                if( mt::sqrlength_v3_v3( prev_position, _position ) > mt::constant::eps )
                {
                    mt::dir_v3_v3( m_targetDir, prev_position, _position );
                }

                float length = mt::length_v3_v3( m_targetDir, m_currentDir );

                if( length < mt::constant::eps )
                {
                    return;
                }

                float t = length / _context->time * m_speed;

                if( t > 1.f )
                {
                    m_currentDir = m_targetDir;

                    m_node->setBillboard( m_currentDir, mt::vec3f( 0.f, 1.f, 1.f ) );
                }
                else
                {
                    m_currentDir = m_currentDir + (m_targetDir - m_currentDir) * t;

                    m_node->setBillboard( m_currentDir, mt::vec3f( 0.f, 1.f, 1.f ) );
                }
            }

            void updatePosition_( const mt::vec3f & _position )
            {
                const mt::vec3f & prev_position = m_node->getLocalPosition();

                if( mt::sqrlength_v3_v3( prev_position, _position ) < mt::constant::eps )
                {
                    return;
                }

                m_node->setLocalPosition( _position );
            }

        protected:
            NodePtr m_node;

            mt::vec3f m_prevDir;
            mt::vec3f m_currentDir;
            mt::vec3f m_targetDir;
            float m_speed;

            ValueInterpolatorParabolic<mt::vec3f> m_interpolator;
        };
        //////////////////////////////////////////////////////////////////////////
        typedef IntrusivePtr<AffectorCreatorInterpolateParabolic> AffectorCreatorInterpolateParabolicPtr;
        //////////////////////////////////////////////////////////////////////////
        class FactoryAffectorInterpolateParabolic
        {
        public:
            FactoryAffectorInterpolateParabolic()
            {
                m_factory = new FactoryPool<AffectorCreatorInterpolateParabolic, 4>();
            }

        public:
            AffectorPtr create( EAffectorType _type
                , const EasingInterfacePtr & _easing
                , const AffectorCallbackInterfacePtr & _cb
                , const NodePtr & _node, const mt::vec3f & _end, const mt::vec3f & _v0, float _time, const Char * _doc )
            {
                AffectorCreatorInterpolateParabolicPtr affector = m_factory->createObject( _doc );

                affector->setAffectorType( _type );
                affector->setEasing( _easing );
                affector->setCallback( _cb );
                affector->setNode( _node );

                affector->initialize( _end, _v0, _time );

                return affector;
            }

        protected:
            FactoryPtr m_factory;
        };
        //////////////////////////////////////////////////////////////////////////
        FactoryAffectorInterpolateParabolic m_nodeAffectorCreatorInterpolateParabolic;
        //////////////////////////////////////////////////////////////////////////
        uint32_t s_Node_parabolaTo( Node * _node
            , float _time
            , const mt::vec3f& _end
            , const mt::vec3f& _v0
            , const ConstString & _easingType
            , const pybind::object & _cb
            , const pybind::args & _args )
        {
            if( _node->isActivate() == false )
            {
                return 0;
            }

            if( _node->isAfterActive() == false )
            {
                return 0;
            }

            EasingInterfacePtr easing = VOCABULARY_GET( STRINGIZE_STRING_LOCAL( "Easing" ), _easingType );

            ScriptableAffectorCallbackPtr callback = createNodeAffectorCallback( _node, _cb, _args );

            AffectorPtr affector =
                m_nodeAffectorCreatorInterpolateParabolic.create( ETA_POSITION
                    , easing
                    , callback
                    , NodePtr( _node ), _end, _v0, _time, MENGINE_DOCUMENT_PYBIND
                );

            if( affector == nullptr )
            {
                return 0;
            }

            s_Node_moveStop( _node );

            if( _node->isActivate() == false )
            {
                return 0;
            }

            AFFECTOR_ID id = _node->addAffector( affector );

            return id;
        }
        //////////////////////////////////////////////////////////////////////////
        class AffectorCreatorFollowTo
            : public BaseAffector
        {
        public:
            AffectorCreatorFollowTo()
                : m_offset( 0.f, 0.f, 0.f )
                , m_distance( 0.f )
                , m_moveSpeed( 0.f )
                , m_moveAcceleration( 0.f )
                , m_moveLimit( 0.f )
                , m_rotate( false )
                , m_rotationSpeed( 0.f )
                , m_rotationAcceleration( 0.f )
                , m_rotationLimit( 0.f )
            {
            }

        public:
            bool initialize( const NodePtr & _node, const NodePtr & _target, const mt::vec3f & _offset, float _distance, float _moveSpeed, float _moveAcceleration, float _moveLimit, bool _rotate, float _rotationSpeed, float _rotationAcceleration, float _rotationLimit )
            {
                if( _node == nullptr )
                {
                    return false;
                }

                if( _target == nullptr )
                {
                    return false;
                }

                m_node = _node;
                m_target = _target;
                m_offset = _offset;
                m_distance = _distance;
                m_moveSpeed = _moveSpeed;
                m_moveAcceleration = _moveAcceleration;
                m_moveLimit = _moveLimit;
                m_rotate = _rotate;
                m_rotationSpeed = _rotationSpeed;
                m_rotationAcceleration = _rotationAcceleration;
                m_rotationLimit = _rotationLimit;

                return true;
            }

        protected:
            bool _affect( const UpdateContext * _context, float * _used ) override
            {
                *_used = _context->time;

                mt::vec3f node_position = m_node->getLocalPosition();
                mt::vec3f follow_position = m_target->getLocalPosition();

                mt::vec3f current_direction;

                if( m_rotate == true )
                {
                    mt::vec3f direction = follow_position - node_position;

                    mt::mat4f mr;
                    mt::make_rotate_m4_direction( mr, direction, mt::vec3f( 0.f, 0.f, 1.f ) );

                    mt::vec3f target_orientation;
                    mt::make_euler_angles( target_orientation, mr );

                    const mt::vec3f & node_orientation = m_node->getLocalOrientation();

                    mt::vec3f correct_rotate_from;
                    mt::vec3f correct_rotate_to;
                    mt::angle_correct_interpolate_from_to( node_orientation.x, target_orientation.x, correct_rotate_from.x, correct_rotate_to.x );
                    mt::angle_correct_interpolate_from_to( node_orientation.y, target_orientation.y, correct_rotate_from.y, correct_rotate_to.y );
                    mt::angle_correct_interpolate_from_to( node_orientation.z, target_orientation.z, correct_rotate_from.z, correct_rotate_to.z );

                    float roatationSpeedStep = m_rotationAcceleration * _context->time;

                    if( m_rotationSpeed + roatationSpeedStep > m_rotationLimit )
                    {
                        m_rotationSpeed = m_rotationLimit;
                    }
                    else
                    {
                        m_rotationSpeed += m_rotationAcceleration * _context->time;
                    }

                    mt::vec3f new_orientation;
                    mt::follow_v3( new_orientation, correct_rotate_from, correct_rotate_to, m_rotationSpeed * _context->time );

                    m_node->setLocalOrientation( new_orientation );

                    current_direction = m_node->getAxisDirection();
                }
                else
                {
                    mt::dir_v3_v3( current_direction, node_position, follow_position );
                }

                float directionSpeedStep = m_moveAcceleration * _context->time;

                if( m_moveSpeed + directionSpeedStep > m_moveLimit )
                {
                    m_moveSpeed = m_moveLimit;
                }
                else
                {
                    m_moveSpeed += m_moveAcceleration * _context->time;
                }

                float step = m_moveSpeed * _context->time;

                float length = mt::length_v3_v3( node_position, follow_position );

                if( m_distance > 0.0 )
                {
                    if( length - step < m_distance )
                    {
                        mt::vec3f distance_position = follow_position + mt::norm_v3( node_position - follow_position ) * m_distance;

                        m_node->setLocalPosition( distance_position );

                        return true;
                    }
                }
                else
                {
                    if( length - step < 0.f )
                    {
                        m_node->setLocalPosition( follow_position );

                        return false;
                    }
                }

                mt::vec3f new_position = node_position + current_direction * step;

                m_node->setLocalPosition( new_position );

                return false;
            }

        protected:
            NodePtr m_node;
            NodePtr m_target;

            mt::vec3f m_offset;
            float m_distance;
            float m_moveSpeed;
            float m_moveAcceleration;
            float m_moveLimit;
            bool m_rotate;
            float m_rotationSpeed;
            float m_rotationAcceleration;
            float m_rotationLimit;
        };
        //////////////////////////////////////////////////////////////////////////
        typedef IntrusivePtr<AffectorCreatorFollowTo> AffectorCreatorFollowToPtr;
        //////////////////////////////////////////////////////////////////////////
        class FactoryAffectorFollowTo
        {
        public:
            FactoryAffectorFollowTo()
            {
                m_factory = new FactoryPool<AffectorCreatorFollowTo, 4>();
            }

        public:
            AffectorPtr create( EAffectorType _type
                , const EasingInterfacePtr & _easing
                , const AffectorCallbackInterfacePtr & _cb
                , const NodePtr & _node, const NodePtr & _target, const mt::vec3f & _offset, float _distance, float _moveSpeed, float _moveAcceleration, float _moveLimit
                , bool _rotate
                , float _rotationSpeed, float _rotationAcceleration, float _rotationLimit, const Char * _doc )
            {
                AffectorCreatorFollowToPtr affector = m_factory->createObject( _doc );

                affector->setAffectorType( _type );
                affector->setEasing( _easing );
                affector->setCallback( _cb );

                if( affector->initialize( _node, _target, _offset, _distance, _moveSpeed, _moveAcceleration, _moveLimit, _rotate, _rotationSpeed, _rotationAcceleration, _rotationLimit ) == false )
                {
                    return nullptr;
                }

                return affector;
            }

        protected:
            FactoryPtr m_factory;
        };
        //////////////////////////////////////////////////////////////////////////
        FactoryAffectorFollowTo m_nodeAffectorCreatorFollowTo;
        //////////////////////////////////////////////////////////////////////////		
        uint32_t s_Node_followTo( Node * _node
            , const NodePtr & _target
            , const mt::vec3f & _offset
            , float _distance
            , float _moveSpeed
            , float _moveAcceleration
            , float _moveLimit
            , bool _rotate
            , float _rotationSpeed
            , float _rotationAcceleration
            , float _rotationLimit
            , const ConstString & _easingType
            , const pybind::object & _cb
            , const pybind::args & _args )
        {
            if( _node->isActivate() == false )
            {
                return 0;
            }

            if( _node->isAfterActive() == false )
            {
                return 0;
            }

            EasingInterfacePtr easing = VOCABULARY_GET( STRINGIZE_STRING_LOCAL( "Easing" ), _easingType );

            ScriptableAffectorCallbackPtr callback = createNodeAffectorCallback( _node, _cb, _args );

            AffectorPtr affector =
                m_nodeAffectorCreatorFollowTo.create( ETA_POSITION
                    , easing
                    , callback
                    , NodePtr( _node ), _target, _offset, _distance
                    , _moveSpeed, _moveAcceleration, _moveLimit
                    , _rotate
                    , _rotationSpeed, _rotationAcceleration, _rotationLimit
                    , MENGINE_DOCUMENT_PYBIND
                );

            MENGINE_ASSERTION_MEMORY_PANIC( affector, 0 );

            s_Node_moveStop( _node );

            if( _node->isActivate() == false )
            {
                return 0;
            }

            AFFECTOR_ID id = _node->addAffector( affector );

            return id;
        }
        //////////////////////////////////////////////////////////////////////////
        class AffectorCreatorFollowToW
            : public BaseAffector
        {
        public:
            AffectorCreatorFollowToW()
                : m_offset( 0.f, 0.f, 0.f )
                , m_distance( 0.f )
                , m_moveSpeed( 0.f )
                , m_moveAcceleration( 0.f )
                , m_moveLimit( 0.f )
            {
            }

        public:
            bool initialize( const NodePtr & _node, const NodePtr & _target, const mt::vec3f & _offset, float _distance, float _moveSpeed, float _moveAcceleration, float _moveLimit )
            {
                if( _node == nullptr )
                {
                    return false;
                }

                if( _target == nullptr )
                {
                    return false;
                }

                m_node = _node;
                m_target = _target;
                m_offset = _offset;
                m_distance = _distance;
                m_moveSpeed = _moveSpeed;
                m_moveAcceleration = _moveAcceleration;
                m_moveLimit = _moveLimit;

                return true;
            }

        protected:
            bool _affect( const UpdateContext * _context, float * _used ) override
            {
                *_used = _context->time;

                mt::vec3f node_position = m_node->getWorldPosition();
                mt::vec3f follow_position = m_target->getWorldPosition();

                mt::vec3f current_direction;

                mt::dir_v3_v3( current_direction, node_position, follow_position );

                float directionSpeedStep = m_moveAcceleration * _context->time;

                if( m_moveSpeed + directionSpeedStep > m_moveLimit )
                {
                    m_moveSpeed = m_moveLimit;
                }
                else
                {
                    m_moveSpeed += m_moveAcceleration * _context->time;
                }

                float step = m_moveSpeed * _context->time;

                float length = mt::length_v3_v3( node_position, follow_position );

                if( m_distance > 0.0 )
                {
                    if( length - step < m_distance )
                    {
                        mt::vec3f distance_position = follow_position + mt::norm_v3( node_position - follow_position ) * m_distance;

                        m_node->setWorldPosition( distance_position );

                        return true;
                    }
                }
                else
                {
                    if( length - step < 0.f )
                    {
                        m_node->setWorldPosition( follow_position );

                        return false;
                    }
                }

                mt::vec3f new_position = node_position + current_direction * step;

                m_node->setWorldPosition( new_position );

                return false;
            }

        protected:
            NodePtr m_node;
            NodePtr m_target;

            mt::vec3f m_offset;
            float m_distance;
            float m_moveSpeed;
            float m_moveAcceleration;
            float m_moveLimit;
        };
        //////////////////////////////////////////////////////////////////////////
        typedef IntrusivePtr<AffectorCreatorFollowToW> AffectorCreatorFollowToWPtr;
        //////////////////////////////////////////////////////////////////////////
        class FactoryAffectorFollowToW
        {
        public:
            FactoryAffectorFollowToW()
            {
                m_factory = new FactoryPool<AffectorCreatorFollowToW, 4>();
            }

        public:
            AffectorPtr create( EAffectorType _type
                , const EasingInterfacePtr & _easing
                , const AffectorCallbackInterfacePtr & _cb
                , const NodePtr & _node, const NodePtr & _target, const mt::vec3f & _offset
                , float _distance, float _moveSpeed, float _moveAcceleration, float _moveLimit, const Char * _doc )
            {
                AffectorCreatorFollowToWPtr affector = m_factory->createObject( _doc );

                affector->setAffectorType( _type );
                affector->setEasing( _easing );
                affector->setCallback( _cb );

                if( affector->initialize( _node, _target, _offset, _distance, _moveSpeed, _moveAcceleration, _moveLimit ) == false )
                {
                    return nullptr;
                }

                return affector;
            }

        protected:
            FactoryPtr m_factory;
        };
        //////////////////////////////////////////////////////////////////////////
        FactoryAffectorFollowToW m_nodeAffectorCreatorFollowToW;
        //////////////////////////////////////////////////////////////////////////		
        uint32_t s_Node_followToW( Node * _node
            , const NodePtr & _target
            , const mt::vec3f & _offset
            , float _distance
            , float _moveSpeed
            , float _moveAcceleration
            , float _moveLimit
            , const ConstString & _easingType
            , const pybind::object & _cb
            , const pybind::args & _args )
        {
            if( _node->isActivate() == false )
            {
                return 0;
            }

            if( _node->isAfterActive() == false )
            {
                return 0;
            }

            EasingInterfacePtr easing = VOCABULARY_GET( STRINGIZE_STRING_LOCAL( "Easing" ), _easingType );

            ScriptableAffectorCallbackPtr callback = createNodeAffectorCallback( _node, _cb, _args );

            AffectorPtr affector =
                m_nodeAffectorCreatorFollowToW.create( ETA_POSITION
                    , easing
                    , callback
                    , NodePtr( _node ), _target, _offset, _distance
                    , _moveSpeed, _moveAcceleration, _moveLimit
                    , MENGINE_DOCUMENT_PYBIND
                );

            MENGINE_ASSERTION_MEMORY_PANIC( affector, 0 );

            s_Node_moveStop( _node );

            if( _node->isActivate() == false )
            {
                return 0;
            }

            AFFECTOR_ID id = _node->addAffector( affector );

            return id;
        }
        //////////////////////////////////////////////////////////////////////////
        void s_Node_angleStop( Node * _node )
        {
            _node->stopAffectors( ETA_ANGLE );
            _node->setAngularSpeed( 0.f );
        }
        //////////////////////////////////////////////////////////////////////////
        NodeAffectorCreator::NodeAffectorCreatorInterpolateLinear<float> m_nodeAffectorCreatorInterpolateLinearFloat;
        //////////////////////////////////////////////////////////////////////////
        uint32_t s_Node_angleTo( Node * _node, float _time, float _angle, const ConstString & _easingType, const pybind::object & _cb, const pybind::args & _args )
        {
            if( _node->isActivate() == false )
            {
                return 0;
            }

            if( _node->isAfterActive() == false )
            {
                return 0;
            }

            float angle = _node->getLocalOrientationX();

            float correct_angle_from = angle;
            float correct_angle_to = _angle;

            EasingInterfacePtr easing = VOCABULARY_GET( STRINGIZE_STRING_LOCAL( "Easing" ), _easingType );

            ScriptableAffectorCallbackPtr callback = createNodeAffectorCallback( _node, _cb, _args );

            AffectorPtr affector = m_nodeAffectorCreatorInterpolateLinearFloat.create( ETA_ANGLE
                , easing
                , callback
                , [_node]( float _v ) {_node->setLocalOrientationX( _v ); }
                , correct_angle_from, correct_angle_to, _time
            );

            if( affector == nullptr )
            {
                return 0;
            }

            s_Node_angleStop( _node );

            if( _node->isActivate() == false )
            {
                return 0;
            }

            float invTime = 1.f / _time;
            float angularSpeed = fabsf( correct_angle_from - correct_angle_to ) * invTime;

            _node->setAngularSpeed( angularSpeed );
            AFFECTOR_ID id = _node->addAffector( affector );

            return id;
        }
        //////////////////////////////////////////////////////////////////////////
        NodeAffectorCreator::NodeAffectorCreatorInterpolateQuadratic<float> m_nodeAffectorCreatorInterpolateQuadraticFloat;
        //////////////////////////////////////////////////////////////////////////
        uint32_t s_Node_accAngleTo( Node * _node, float _time, float _angle, const ConstString & _easingType, const pybind::object & _cb, const pybind::args & _args )
        {
            if( _node->isActivate() == false )
            {
                return 0;
            }

            if( _node->isAfterActive() == false )
            {
                return 0;
            }

            float angularSpeed = _node->getAngularSpeed();

            float angle = _node->getLocalOrientationX();

            float correct_angle_from = angle;
            float correct_angle_to = _angle;
            //mt::angle_correct_interpolate_from_to( angle, _angle, correct_angle_from, correct_angle_to );

            EasingInterfacePtr easing = VOCABULARY_GET( STRINGIZE_STRING_LOCAL( "Easing" ), _easingType );

            ScriptableAffectorCallbackPtr callback = createNodeAffectorCallback( _node, _cb, _args );

            AffectorPtr affector =
                m_nodeAffectorCreatorInterpolateQuadraticFloat.create( ETA_ANGLE
                    , easing
                    , callback
                    , [_node]( float _v ) {_node->setLocalOrientationX( _v ); }
                    , correct_angle_from, correct_angle_to, angularSpeed, _time
                );

            if( affector == nullptr )
            {
                return 0;
            }

            s_Node_angleStop( _node );

            if( _node->isActivate() == false )
            {
                return 0;
            }

            AFFECTOR_ID id = _node->addAffector( affector );

            return id;
        }
        //////////////////////////////////////////////////////////////////////////
        void s_Node_scaleStop( Node * _node )
        {
            _node->stopAffectors( ETA_SCALE );
        }
        //////////////////////////////////////////////////////////////////////////
        uint32_t s_Node_scaleTo( Node * _node, float _time, const mt::vec3f& _scale, const ConstString & _easingType, const pybind::object & _cb, const pybind::args & _args )
        {
            if( _node->isActivate() == false )
            {
                return 0;
            }

            if( _node->isAfterActive() == false )
            {
                return 0;
            }

            EasingInterfacePtr easing = VOCABULARY_GET( STRINGIZE_STRING_LOCAL( "Easing" ), _easingType );

            ScriptableAffectorCallbackPtr callback = createNodeAffectorCallback( _node, _cb, _args );

            AffectorPtr affector = m_nodeAffectorCreatorInterpolateLinear.create( ETA_SCALE
                , easing
                , callback
                , [_node]( const mt::vec3f & _v ) { _node->setLocalScale( _v ); }
                , _node->getLocalScale(), _scale, _time
            );

            if( affector == nullptr )
            {
                return 0;
            }

            s_Node_scaleStop( _node );

            if( _node->isActivate() == false )
            {
                return 0;
            }

            AFFECTOR_ID id = _node->addAffector( affector );

            return id;
        }
        //////////////////////////////////////////////////////////////////////////
        void s_Node_colorStop( Node * _node )
        {
            _node->stopAffectors( ETA_COLOR );
        }
        //////////////////////////////////////////////////////////////////////////
        NodeAffectorCreator::NodeAffectorCreatorInterpolateLinear<Color> m_nodeAffectorCreatorInterpolateLinearColor;
        //////////////////////////////////////////////////////////////////////////
        uint32_t s_Node_colorTo( Node * _node, float _time, const Color& _color, const ConstString & _easingType, const pybind::object & _cb, const pybind::args & _args )
        {
            if( _node == nullptr )
            {
                LOGGER_ERROR( "Node.colorTo _node is None" );

                return 0;
            }

            if( _node->isActivate() == false )
            {
                LOGGER_ERROR( "Node.colorTo node '%s' is not activate"
                    , _node->getName().c_str()
                );

                return 0;
            }

            if( _node->isAfterActive() == false )
            {
                LOGGER_ERROR( "Node.colorTo node '%s' is not after activate"
                    , _node->getName().c_str()
                );

                return 0;
            }

            RenderInterface * render = _node->getRender();

            if( render == nullptr )
            {
                LOGGER_ERROR( "Node.colorTo node '%s' is not renderable"
                    , _node->getName().c_str()
                );

                return 0;
            }

            EasingInterfacePtr easing = VOCABULARY_GET( STRINGIZE_STRING_LOCAL( "Easing" ), _easingType );

            ScriptableAffectorCallbackPtr callback = createNodeAffectorCallback( _node, _cb, _args );

            AffectorPtr affector =
                m_nodeAffectorCreatorInterpolateLinearColor.create( ETA_COLOR
                    , easing
                    , callback
                    , [render]( const Color & _v ) { render->setLocalColor( _v ); }
                    , render->getLocalColor(), _color, _time
                );

            if( affector == nullptr )
            {
                LOGGER_ERROR( "Node.colorTo node '%s' invalid create affector"
                    , _node->getName().c_str()
                );

                return 0;
            }

            s_Node_colorStop( _node );

            if( _node->isActivate() == false )
            {
                LOGGER_ERROR( "Node.colorTo node '%s' after color stop is inactivate"
                    , _node->getName().c_str()
                );

                return 0;
            }

            AFFECTOR_ID id = _node->addAffector( affector );

            if( id == 0 )
            {
                LOGGER_ERROR( "Node.colorTo node '%s' invalid add affector"
                    , _node->getName().c_str()
                );

                return 0;
            }

            return id;
        }
        //////////////////////////////////////////////////////////////////////////
        uint32_t s_Node_alphaTo( Node * _node, float _time, float _alpha, const ConstString & _easingType, const pybind::object & _cb, const pybind::args & _args )
        {
            if( _node == nullptr )
            {
                LOGGER_ERROR( "Node.alphaTo _node is None" );

                return 0;
            }

            if( _node->isActivate() == false )
            {
                LOGGER_ERROR( "Node.alphaTo node '%s' is not activate"
                    , _node->getName().c_str()
                );

                return 0;
            }

            if( _node->isAfterActive() == false )
            {
                LOGGER_ERROR( "Node.alphaTo node '%s' is not after activate"
                    , _node->getName().c_str()
                );

                return 0;
            }

            RenderInterface * render = _node->getRender();

            if( render == nullptr )
            {
                LOGGER_ERROR( "Node.alphaTo node '%s' type '%s' is not renderable"
                    , _node->getName().c_str()
                    , _node->getType().c_str()
                );

                return 0;
            }

            EasingInterfacePtr easing = VOCABULARY_GET( STRINGIZE_STRING_LOCAL( "Easing" ), _easingType );

            ScriptableAffectorCallbackPtr callback = createNodeAffectorCallback( _node, _cb, _args );

            AffectorPtr affector =
                m_nodeAffectorCreatorInterpolateLinearFloat.create( ETA_COLOR
                    , easing
                    , callback
                    , [render]( float _v ) { render->setLocalAlpha( _v ); }
                    , render->getLocalAlpha(), _alpha, _time
                );

            if( affector == nullptr )
            {
                LOGGER_ERROR( "Node.alphaTo node '%s' invalid create affector"
                    , _node->getName().c_str()
                );

                return 0;
            }

            s_Node_colorStop( _node );

            if( _node->isActivate() == false )
            {
                LOGGER_ERROR( "Node.alphaTo node '%s' after color stop is inactivate"
                    , _node->getName().c_str()
                );

                return 0;
            }

            AFFECTOR_ID id = _node->addAffector( affector );

            if( id == 0 )
            {
                LOGGER_ERROR( "Node.alphaTo node '%s' invalid add affector"
                    , _node->getName().c_str()
                );

                return 0;
            }

            return id;
        }
    };
    //////////////////////////////////////////////////////////////////////////
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
#define SCRIPT_CLASS_WRAPPING( Class )\
    VOCABULARY_SET(ScriptWrapperInterface, STRINGIZE_STRING_LOCAL("ClassWrapping"), STRINGIZE_STRING_LOCAL(#Class), Helper::makeFactorableUnique<PythonScriptWrapper<Class> >(_kernel))

        SCRIPT_CLASS_WRAPPING( Node );
        SCRIPT_CLASS_WRAPPING( Layer );
        SCRIPT_CLASS_WRAPPING( Layer2D );
        SCRIPT_CLASS_WRAPPING( HotSpot );
        SCRIPT_CLASS_WRAPPING( HotSpotPolygon );
        SCRIPT_CLASS_WRAPPING( HotSpotCircle );
        SCRIPT_CLASS_WRAPPING( HotSpotBubbles );
        SCRIPT_CLASS_WRAPPING( HotSpotImage );
        SCRIPT_CLASS_WRAPPING( HotSpotShape );

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

        NodeScriptMethod * nodeScriptMethod = new FactorableUnique<NodeScriptMethod>();

        pybind::interface_<Mixin>( _kernel, "Mixin", true )
            .def_smart_pointer()
            ;

        pybind::interface_<Scriptable, pybind::bases<Mixin> >( _kernel, "Scriptable" )
            .def_bindable()
            ;

        pybind::interface_<Identity, pybind::bases<Mixin> >( _kernel, "Identity" )
            .def( "setName", &Identity::setName )
            .def( "getName", &Identity::getName )
            .def( "getType", &Identity::getType )
            ;

        pybind::interface_<BoundingBox, pybind::bases<Mixin> >( _kernel, "BoundingBox" )
            .def_proxy_static( "getBoundingBox", nodeScriptMethod, &NodeScriptMethod::s_BoundingBox_getBoundingBox )
            ;

        pybind::interface_<Transformation, pybind::bases<Mixin> >( _kernel, "Transformation" )
            .def( "setLocalPosition", &Transformation::setLocalPosition )
            .def( "getLocalPosition", &Transformation::getLocalPosition )
            .def( "setLocalPositionX", &Transformation::setLocalPositionX )
            .def( "getLocalPositionX", &Transformation::getLocalPositionX )
            .def( "setLocalPositionY", &Transformation::setLocalPositionY )
            .def( "getLocalPositionY", &Transformation::getLocalPositionY )
            .def( "setLocalPositionZ", &Transformation::setLocalPositionZ )
            .def( "getLocalPositionZ", &Transformation::getLocalPositionZ )
            .def( "setLocalOrigin", &Transformation::setLocalOrigin )
            .def( "getLocalOrigin", &Transformation::getLocalOrigin )
            .def( "setLocalScale", &Transformation::setLocalScale )
            .def( "getLocalScale", &Transformation::getLocalScale )
            .def( "setLocalSkew", &Transformation::setLocalSkew )
            .def( "getLocalSkew", &Transformation::getLocalSkew )
            .def( "setLocalOrientationX", &Transformation::setLocalOrientationX )
            .def( "getLocalOrientationX", &Transformation::getLocalOrientationX )
            .def( "setLocalOrientationY", &Transformation::setLocalOrientationY )
            .def( "getLocalOrientationY", &Transformation::getLocalOrientationY )
            .def( "setLocalOrientationZ", &Transformation::setLocalOrientationZ )
            .def( "getLocalOrientationZ", &Transformation::getLocalOrientationZ )
            .def( "setLocalOrientation", &Transformation::setLocalOrientation )
            .def( "getLocalOrientation", &Transformation::getLocalOrientation )

            .def( "setOrigin", &Transformation::setLocalOrigin )
            .def( "getOrigin", &Transformation::getLocalOrigin )
            .def( "setScale", &Transformation::setLocalScale )
            .def( "getScale", &Transformation::getLocalScale )
            .def( "setSkew", &Transformation::setLocalSkew )
            .def( "getSkew", &Transformation::getLocalSkew )
            .def( "setOrientationX", &Transformation::setLocalOrientationX )
            .def( "getOrientationX", &Transformation::getLocalOrientationX )
            .def( "setOrientationY", &Transformation::setLocalOrientationY )
            .def( "getOrientationY", &Transformation::getLocalOrientationY )
            .def( "setOrientationZ", &Transformation::setLocalOrientationZ )
            .def( "getOrientationZ", &Transformation::getLocalOrientationZ )
            .def( "setOrientation", &Transformation::setLocalOrientation )
            .def( "getOrientation", &Transformation::getLocalOrientation )

            .def( "setAngle", &Transformation::setLocalOrientationX )
            .def( "getAngle", &Transformation::getLocalOrientationX )
            .def_proxy_static( "setAngleDeg", nodeScriptMethod, &NodeScriptMethod::s_Transformation_setAngleDeg )

            .def( "setDirection", &Transformation::setDirection )
            .def( "setBillboard", &Transformation::setBillboard )
            .def( "setAxes", &Transformation::setAxes )

            .def( "billboardAt", &Transformation::billboardAt )
            .def( "lookAt", &Transformation::lookAt )

            .def( "getAxisDirection", &Transformation::getAxisDirection )
            .def( "getAxisLeft", &Transformation::getAxisLeft )
            .def( "getAxisUp", &Transformation::getAxisUp )

            .def( "translate", &Transformation::translate )
            .def( "rotate", &Transformation::rotate )
            .def( "coordinate", &Transformation::coordinate )

            .def( "resetTransformation", &Transformation::resetTransformation )
            .def( "setRelationTransformation", &Transformation::setRelationTransformation )
            .def_proxy_static( "removeRelationTransformation", nodeScriptMethod, &NodeScriptMethod::s_Transformation_removeRelationTransformation )
            ;

        pybind::interface_<Compilable, pybind::bases<Mixin> >( _kernel, "Compilable" )
            .def( "compile", &Compilable::compile )
            .def( "release", &Compilable::release )
            .def( "isCompile", &Compilable::isCompile )
            ;

        pybind::interface_<Reference, pybind::bases<Mixin> >( _kernel, "Reference" )
            .def( "incrementReference", &Reference::incrementReference )
            .def( "decrementReference", &Reference::decrementReference )
            .def( "countReference", &Reference::countReference )
            ;

        pybind::interface_<FileGroupInterface, pybind::bases<Mixin> >( _kernel, "FileGroupInterface" )
            .def( "getName", &FileGroupInterface::getName )
            .def( "getCategory", &FileGroupInterface::getCategory )
            .def( "isPacked", &FileGroupInterface::isPacked )
            .def( "getRelationPath", &FileGroupInterface::getRelationPath )
            .def( "existFile", &FileGroupInterface::existFile )
            .def( "existDirectory", &FileGroupInterface::existDirectory )
            ;

        pybind::interface_<Content, pybind::bases<Mixin> >( _kernel, "Content", false )
            .def( "setFilePath", &Content::setFilePath )
            .def( "getFilePath", &Content::getFilePath )
            .def( "setCodecType", &Content::setCodecType )
            .def( "getCodecType", &Content::getCodecType )
            .def( "setConverterType", &Content::setConverterType )
            .def( "getConverterType", &Content::getConverterType )
            .def( "setDataflowType", &Content::setDataflowType )
            .def( "getDataflowType", &Content::getDataflowType )
            .def( "setValidNoExist", &Content::setValidNoExist )
            .def( "isValidNoExist", &Content::isValidNoExist )
            ;

        pybind::interface_<Contentable, pybind::bases<Mixin>>( _kernel, "Contentable", false )
            .def( "getContent", &Contentable::getContent )
            ;

        pybind::interface_<Resource, pybind::bases<Contentable, Scriptable, Compilable, Identity, Reference> >( _kernel, "ResourceReference", false )
            .def( "setLocale", &Resource::setLocale )
            .def( "getLocale", &Resource::getLocale )
            .def( "setFileGroup", &Resource::setFileGroup )
            .def( "getFileGroup", &Resource::getFileGroup )
            .def( "setGroupName", &Resource::setGroupName )
            .def( "getGroupName", &Resource::getGroupName )
            .def( "cache", &Resource::cache )
            .def( "uncache", &Resource::uncache )
            ;

        pybind::interface_<ResourceImage, pybind::bases<Resource> >( _kernel, "ResourceImage", false )
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

        pybind::interface_<ResourceImageData, pybind::bases<Resource, Content> >( _kernel, "ResourceImageData", false )
            .def_deprecated( "setImagePath", &ResourceImageData::setFilePath, "use setFilePath" )
            .def_deprecated( "getImagePath", &ResourceImageData::getFilePath, "use getFilePath" )
            .def( "setImageMaxSize", &ResourceImageData::setImageMaxSize )
            .def( "getImageMaxSize", &ResourceImageData::getImageMaxSize )
            .def( "setImageWidth", &ResourceImageData::setImageWidth )
            .def( "getImageWidth", &ResourceImageData::getImageWidth )
            .def( "setImageHeight", &ResourceImageData::setImageHeight )
            .def( "getImageHeight", &ResourceImageData::getImageHeight )
            ;

        pybind::interface_<ResourceImageDefault, pybind::bases<ResourceImage, Content> >( _kernel, "ResourceImageDefault", false )
            .def( "correctUVTexture", &ResourceImageDefault::correctUVTexture )
            ;

        pybind::interface_<ResourceImageSubstractRGBAndAlpha, pybind::bases<ResourceImage> >( _kernel, "ResourceImageSubstractRGBAndAlpha", false )
            ;

        pybind::interface_<ResourceImageSubstract, pybind::bases<ResourceImage> >( _kernel, "ResourceImageSubstract", false )
            ;

        pybind::interface_<ResourceImageSolid, pybind::bases<ResourceImage> >( _kernel, "ResourceImageSolid", false )
            ;

        pybind::interface_<ResourceMusic, pybind::bases<Resource, Content> >( _kernel, "ResourceMusic", false )
            ;

        pybind::interface_<ResourceImageSequence, pybind::bases<Resource> >( _kernel, "ResourceImageSequence", false )
            ;

        pybind::interface_<ResourceSound, pybind::bases<Resource, Content> >( _kernel, "ResourceSound", false )
            ;

        pybind::interface_<ResourceFile, pybind::bases<Resource> >( _kernel, "ResourceFile", false )
            ;

        pybind::interface_<ResourceShape, pybind::bases<Resource> >( _kernel, "ResourceShape", false )
            .def( "getPolygon", &ResourceShape::getPolygon )
            ;

        pybind::interface_<ResourceCursor, pybind::bases<Resource> >( _kernel, "ResourceCursor", false )
            ;

        pybind::interface_<ResourceCursorICO, pybind::bases<ResourceCursor> >( _kernel, "ResourceCursorICO", false )
            ;

        pybind::interface_<ResourceCursorSystem, pybind::bases<ResourceCursor> >( _kernel, "ResourceCursorSystem", false )
            ;

        pybind::interface_<ResourceWindow, pybind::bases<Resource> >( _kernel, "ResourceWindow", false )
            ;

        pybind::interface_<ResourceTestPick, pybind::bases<Resource> >( _kernel, "ResourceTestPick", false )
            .def_deprecated( "getWidth", &ResourceTestPick::getImageWidth, "use getImageWidth" )
            .def_deprecated( "getHeight", &ResourceTestPick::getImageHeight, "use getImageHeight" )
            .def( "getImageWidth", &ResourceTestPick::getImageWidth )
            .def( "getImageHeight", &ResourceTestPick::getImageHeight )
            ;

        pybind::interface_<ResourceHIT, pybind::bases<ResourceTestPick, Content> >( _kernel, "ResourceHIT", false )
            ;

        pybind::interface_<UpdationInterface, pybind::bases<Mixin> >( _kernel, "Updation" )
            ;

        pybind::interface_<Updatable, pybind::bases<Mixin> >( _kernel, "Updatable" )
            .def( "getUpdation", &Updatable::getUpdation )
            ;

        pybind::interface_<Renderable, pybind::bases<Mixin> >( _kernel, "Renderable" )
            .def( "getRender", &Renderable::getRender )
            ;

        pybind::interface_<Colorable, pybind::bases<Mixin> >( _kernel, "Colorable" )
            .def( "setLocalColor", &Colorable::setLocalColor )
            .def( "getLocalColor", &Colorable::getLocalColor )
            .def( "setLocalColorR", &Colorable::setLocalColorR )
            .def( "getLocalColorR", &Colorable::getLocalColorR )
            .def( "setLocalColorG", &Colorable::setLocalColorG )
            .def( "getLocalColorG", &Colorable::getLocalColorG )
            .def( "setLocalColorB", &Colorable::setLocalColorB )
            .def( "getLocalColorB", &Colorable::getLocalColorB )
            .def( "setLocalColorRGB", &Colorable::setLocalColorRGB )
            .def( "setLocalAlpha", &Colorable::setLocalAlpha )
            .def( "getLocalAlpha", &Colorable::getLocalAlpha )
            .def( "setPersonalColor", &Colorable::setPersonalColor )
            .def( "getPersonalColor", &Colorable::getPersonalColor )
            .def( "setPersonalAlpha", &Colorable::setPersonalAlpha )
            .def( "getPersonalAlpha", &Colorable::getPersonalAlpha )
            .def( "getWorldColor", &Colorable::getWorldColor )
            .def( "isSolidColor", &Colorable::isSolidColor )
            ;

        pybind::interface_<RenderInterface, pybind::bases<Colorable, BoundingBox> >( _kernel, "RenderInterface" )
            .def( "setRelationRender", &RenderInterface::setRelationRender )
            .def( "removeRelationRender", &RenderInterface::removeRelationRender )
            .def( "getRelationRender", &RenderInterface::getRelationRender )
            .def( "setHide", &RenderInterface::setHide )
            .def_deprecated( "hide", &RenderInterface::setHide, "use setHide" )
            .def( "isHide", &RenderInterface::isHide )
            .def( "localHide", &RenderInterface::setLocalHide )
            .def( "isLocalHide", &RenderInterface::isLocalHide )
            .def( "setRenderViewport", &RenderInterface::setRenderViewport )
            .def( "getRenderViewport", &RenderInterface::getRenderViewport )
            .def( "setRenderCamera", &RenderInterface::setRenderCamera )
            .def( "getRenderCamera", &RenderInterface::getRenderCamera )
            .def( "setRenderScissor", &RenderInterface::setRenderScissor )
            .def( "getRenderScissor", &RenderInterface::getRenderScissor )
            .def( "setRenderTarget", &RenderInterface::setRenderTarget )
            .def( "getRenderTarget", &RenderInterface::getRenderTarget )
            .def( "setExternalRender", &RenderInterface::setExternalRender )
            .def( "isExternalRender", &RenderInterface::isExternalRender )
            ;

        pybind::interface_<AnimationInterface, pybind::bases<Mixin> >( _kernel, "Animation" )
            .def_proxy_static( "play", nodeScriptMethod, &NodeScriptMethod::s_Animation_play )
            .def( "stop", &AnimationInterface::stop )
            .def( "pause", &AnimationInterface::pause )
            .def_proxy_static( "resume", nodeScriptMethod, &NodeScriptMethod::s_Animation_resume )
            .def( "interrupt", &AnimationInterface::interrupt )
            .def( "isInterrupt", &AnimationInterface::isInterrupt )
            .def( "isPlay", &AnimationInterface::isPlay )
            .def( "getPlayId", &AnimationInterface::getPlayId )
            .def( "setAnimationSpeedFactor", &AnimationInterface::setAnimationSpeedFactor )
            .def( "getAnimationSpeedFactor", &AnimationInterface::getAnimationSpeedFactor )
            .def( "setFirstFrame", &AnimationInterface::setFirstFrame )
            .def( "setLastFrame", &AnimationInterface::setLastFrame )
            .def( "setTime", &AnimationInterface::setTime )
            .def( "getTime", &AnimationInterface::getTime )
            .def( "getDuration", &AnimationInterface::getDuration )
            .def( "setPlayCount", &AnimationInterface::setPlayCount )
            .def( "getPlayCount", &AnimationInterface::getPlayCount )
            .def( "getPlayIterator", &AnimationInterface::getPlayIterator )
            .def( "setAutoPlay", &AnimationInterface::setAutoPlay )
            .def_deprecated( "getAutoPlay", &AnimationInterface::isAutoPlay, "use isAutoPlay" )
            .def( "isAutoPlay", &AnimationInterface::isAutoPlay )
            .def( "setLoop", &AnimationInterface::setLoop )
            .def_deprecated( "getLoop", &AnimationInterface::isLoop, "use isLoop" )
            .def( "isLoop", &AnimationInterface::isLoop )
            .def( "setInterval", &AnimationInterface::setInterval )
            .def( "getIntervalBegin", &AnimationInterface::getIntervalBegin )
            .def( "getIntervalEnd", &AnimationInterface::getIntervalEnd )
            .def( "setIntervalStart", &AnimationInterface::setIntervalStart )
            .def( "getIntervalStart", &AnimationInterface::getIntervalStart )
            ;

        pybind::interface_<Animatable, pybind::bases<Mixin> >( _kernel, "Animatable" )
            .def( "getAnimation", &Animatable::getAnimation )
            ;

        pybind::interface_<Eventable, pybind::bases<Mixin> >( _kernel, "Eventable" )
            ;

        pybind::interface_<Soundable, pybind::bases<Mixin> >( _kernel, "Soundable" )
            .def( "setVolume", &Soundable::setVolume )
            .def( "getVolume", &Soundable::getVolume )
            ;

        pybind::interface_<Affectorable, pybind::bases<Mixin> >( _kernel, "Affectorable" )
            .def( "addAffector", &Affectorable::addAffector )
            .def( "stopAffector", &Affectorable::stopAffector )
            .def( "stopAllAffectors", &Affectorable::stopAllAffectors )
            .def( "setLinearSpeed", &Affectorable::setLinearSpeed )
            .def( "getLinearSpeed", &Affectorable::getLinearSpeed )
            ;

        pybind::interface_<Materialable, pybind::bases<Mixin> >( _kernel, "Materialable", false )
            .def( "setMaterialName", &Materialable::setMaterialName )
            .def( "getMaterialName", &Materialable::getMaterialName )
            .def( "setDisableTextureColor", &Materialable::setDisableTextureColor )
            .def( "getDisableTextureColor", &Materialable::getDisableTextureColor )
            .def( "setBlendMode", &Materialable::setBlendMode )
            .def( "getBlendMode", &Materialable::getBlendMode )
            ;

        pybind::interface_<Node, pybind::bases<Scriptable, Identity, Transformation, Compilable, Renderable, Affectorable> >( _kernel, "Node", false )
            .def( "enable", &Node::enable )
            .def( "disable", &Node::disable )
            .def( "isEnable", &Node::isEnable )
            .def( "isActivate", &Node::isActivate )
            .def( "freeze", &Node::freeze )
            .def( "isFreeze", &Node::isFreeze )
            .def( "getUniqueIdentity", &Node::getUniqueIdentity )
            .def( "findUniqueChild", &Node::findUniqueChild )
            .def_deprecated( "setSpeedFactor", &Node::setSpeedFactor, "don't work, use getAnimation" )
            .def_deprecated( "getSpeedFactor", &Node::getSpeedFactor, "don't work, use getAnimation" )
            .def( "addChildren", &Node::addChild )
            .def( "addChildrenFront", &Node::addChildFront )
            .def( "addChildrenAfter", &Node::addChildAfter )
            .def_proxy_static( "removeChildren", nodeScriptMethod, &NodeScriptMethod::s_Node_removeChild )
            .def_proxy_static( "removeAllChild", nodeScriptMethod, &NodeScriptMethod::s_Node_removeAllChild )
            .def_proxy_static( "removeFromParent", nodeScriptMethod, &NodeScriptMethod::s_Node_removeFromParent )
            .def_proxy_static( "destroyAllChild", nodeScriptMethod, &NodeScriptMethod::s_Node_destroyAllChild )
            //.def( "isHomeless", &Node::isHomeless )
            .def_proxy_static( "isHomeless", nodeScriptMethod, &NodeScriptMethod::s_Node_isHomeless )
            //.def_static( "getChild", &ScriptMethod::s_getChild )
            .def( "findChildren", &Node::findChild )
            .def( "getSiblingPrev", &Node::getSiblingPrev )
            .def( "getSiblingNext", &Node::getSiblingNext )
            .def( "emptyChild", &Node::emptyChildren )
            .def( "hasChildren", &Node::hasChild )
            .def( "getParent", &Node::getParent )
            .def( "hasParent", &Node::hasParent )

            .def( "getWorldPosition", &Node::getWorldPosition )
            .def( "setWorldPosition", &Node::setWorldPosition )
            //.def( "getWorldDirection", &Node::getWorldDirection )
            .def_proxy_static( "getWorldOffsetPosition", nodeScriptMethod, &NodeScriptMethod::s_Node_getWorldOffsetPosition )
            .def_proxy_static( "getLengthTo", nodeScriptMethod, &NodeScriptMethod::s_Node_getLengthTo )

            .def_proxy_static( "getDebugId", nodeScriptMethod, &NodeScriptMethod::s_Node_getDebugId )

            .def_proxy_static( "createChildren", nodeScriptMethod, &NodeScriptMethod::s_Node_createChildren )
            .def_proxy_static_kernel( "getAllChildren", nodeScriptMethod, &NodeScriptMethod::s_Node_getAllChildren )

            .def_proxy_static_args( "colorTo", nodeScriptMethod, &NodeScriptMethod::s_Node_colorTo )
            .def_proxy_static_args( "alphaTo", nodeScriptMethod, &NodeScriptMethod::s_Node_alphaTo )
            .def_proxy_static( "colorStop", nodeScriptMethod, &NodeScriptMethod::s_Node_colorStop )

            .def_proxy_static_args( "velocityTo", nodeScriptMethod, &NodeScriptMethod::s_Node_velocityTo )
            .def_proxy_static_args( "velocityTo2", nodeScriptMethod, &NodeScriptMethod::s_Node_velocityTo2 )

            .def_proxy_static_args( "moveTo", nodeScriptMethod, &NodeScriptMethod::s_Node_moveTo )
            .def_proxy_static_args( "bezier2To", nodeScriptMethod, &NodeScriptMethod::s_Node_bezier2To )
            .def_proxy_static_args( "bezier3To", nodeScriptMethod, &NodeScriptMethod::s_Node_bezier3To )
            .def_proxy_static_args( "bezier4To", nodeScriptMethod, &NodeScriptMethod::s_Node_bezier4To )
            .def_proxy_static_args( "parabolaTo", nodeScriptMethod, &NodeScriptMethod::s_Node_parabolaTo )
            .def_proxy_static_args( "followTo", nodeScriptMethod, &NodeScriptMethod::s_Node_followTo )
            .def_proxy_static_args( "followToW", nodeScriptMethod, &NodeScriptMethod::s_Node_followToW )
            .def_proxy_static_args( "bezier2Follower", nodeScriptMethod, &NodeScriptMethod::s_Node_bezier2Follower )
            .def_proxy_static( "moveStop", nodeScriptMethod, &NodeScriptMethod::s_Node_moveStop )

            .def_proxy_static_args( "angleTo", nodeScriptMethod, &NodeScriptMethod::s_Node_angleTo )
            .def_proxy_static( "angleStop", nodeScriptMethod, &NodeScriptMethod::s_Node_angleStop )
            .def_proxy_static_args( "scaleTo", nodeScriptMethod, &NodeScriptMethod::s_Node_scaleTo )
            .def_proxy_static( "scaleStop", nodeScriptMethod, &NodeScriptMethod::s_Node_scaleStop )

            .def_proxy_static_args( "accMoveTo", nodeScriptMethod, &NodeScriptMethod::s_Node_accMoveTo )
            .def_proxy_static_args( "accAngleTo", nodeScriptMethod, &NodeScriptMethod::s_Node_accAngleTo )
            ;

        pybind::interface_<Affector, pybind::bases<Updatable> >( _kernel, "Affector", true )
            .def( "stop", &Affector::stop )
            .def( "getId", &Affector::getId )
            .def( "setFreeze", &Affector::setFreeze )
            .def( "getFreeze", &Affector::getFreeze )
            .def( "setSpeedFactor", &Affector::setSpeedFactor )
            .def( "getSpeedFactor", &Affector::getSpeedFactor )
            ;

        pybind::interface_<Surface, pybind::bases<Scriptable, Identity, Materialable, Compilable> >( _kernel, "Surface", false )
            .def( "getMaxSize", &Surface::getMaxSize )
            .def( "getSize", &Surface::getSize )
            .def( "getOffset", &Surface::getOffset )
            .def( "getUVCount", &Surface::getUVCount )
            .def( "getUV", &Surface::getUV )
            .def( "getColor", &Surface::getColor )
            .def_deprecated( "getColour", &Surface::getColor, "use getColor" )
            ;

        pybind::interface_<SurfaceSound, pybind::bases<Surface, Eventable, Animatable, Soundable> >( _kernel, "SurfaceSound", false )
            .def( "setResourceSound", &SurfaceSound::setResourceSound )
            .def( "getResourceSound", &SurfaceSound::getResourceSound )
            .def_proxy_native_kernel( "setEventListener", nodeScriptMethod, &NodeScriptMethod::s_SurfaceSound_setEventListener )
            ;

        pybind::interface_<SurfaceImage, pybind::bases<Surface> >( _kernel, "SurfaceImage", false )
            .def( "setResourceImage", &SurfaceImage::setResourceImage )
            .def( "getResourceImage", &SurfaceImage::getResourceImage )
            ;

        pybind::interface_<SurfaceImageSequence, pybind::bases<Surface, Eventable, Animatable> >( _kernel, "SurfaceImageSequence", false )
            .def( "setResourceImageSequence", &SurfaceImageSequence::setResourceImageSequence )
            .def( "getResourceImageSequence", &SurfaceImageSequence::getResourceImageSequence )
            .def( "getFrameCount", &SurfaceImageSequence::getFrameCount )
            .def( "getFrameDelay", &SurfaceImageSequence::getFrameDelay )
            .def( "setCurrentFrame", &SurfaceImageSequence::setCurrentFrame )
            .def( "getCurrentFrame", &SurfaceImageSequence::getCurrentFrame )
            .def_proxy_native_kernel( "setEventListener", nodeScriptMethod, &NodeScriptMethod::s_SurfaceImageSequence_setEventListener )
            ;

        pybind::interface_<SurfaceTrackMatte, pybind::bases<Surface> >( _kernel, "SurfaceTrackMatte", false )
            .def( "setResourceImage", &SurfaceTrackMatte::setResourceImage )
            .def( "getResourceImage", &SurfaceTrackMatte::getResourceImage )
            .def( "setResourceTrackMatteImage", &SurfaceTrackMatte::setResourceTrackMatteImage )
            .def( "getResourceTrackMatteImage", &SurfaceTrackMatte::getResourceTrackMatteImage )
            .def( "setTrackMatteMode", &SurfaceTrackMatte::setTrackMatteMode )
            .def( "getTrackMatteMode", &SurfaceTrackMatte::getTrackMatteMode )
            ;

        pybind::interface_<SurfaceSolidColor, pybind::bases<Surface> >( _kernel, "SurfaceSolidColor", false )
            .def( "setSolidColor", &SurfaceSolidColor::setSolidColor )
            .def( "getSolidColor", &SurfaceSolidColor::getSolidColor )
            .def( "setSolidSize", &SurfaceSolidColor::setSolidSize )
            .def( "getSolidSize", &SurfaceSolidColor::getSolidSize )
            ;

        pybind::interface_<Visitable, pybind::bases<Mixin> >( _kernel, "Visitable" )
            ;

        pybind::interface_<ThreadTask, pybind::bases<Visitable> >( _kernel, "Task" )
            ;

        pybind::interface_<RenderViewportInterface, pybind::bases<Mixin> >( _kernel, "RenderViewportInterface" )
            .def( "getViewport", &RenderViewportInterface::getViewport )
            ;

        pybind::interface_<RenderViewport, pybind::bases<Node, RenderViewportInterface> >( _kernel, "RenderViewport", false )
            .def( "setFixedViewport", &RenderViewport::setFixedViewport )
            .def( "getFixedViewport", &RenderViewport::getFixedViewport )
            .def( "setViewport", &RenderViewport::setViewport )
            ;

        pybind::interface_<RenderScissorInterface, pybind::bases<Mixin> >( _kernel, "RenderScissorInterface" )
            .def( "getScissorViewport", &RenderScissorInterface::getScissorViewport )
            ;

        pybind::interface_<RenderScissor, pybind::bases<Node, RenderScissorInterface> >( _kernel, "RenderScissor", false )
            .def( "setViewport", &RenderScissor::setViewport )
            .def( "getViewport", &RenderScissor::getViewport )
            ;

        pybind::interface_<RenderCameraInterface, pybind::bases<Mixin> >( _kernel, "RenderCameraInterface" )
            ;

        pybind::interface_<RenderCameraOrthogonal, pybind::bases<Node, RenderCameraInterface> >( _kernel, "RenderCameraOrthogonal", false )
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

        pybind::interface_<RenderCameraProjection, pybind::bases<Node, RenderCameraInterface> >( _kernel, "RenderCameraProjection", false )
            .def( "setCameraPosition", &RenderCameraProjection::setCameraPosition )
            .def( "setCameraDirection", &RenderCameraProjection::setCameraDirection )
            .def( "setCameraUp", &RenderCameraProjection::setCameraUp )
            .def( "setCameraRightSign", &RenderCameraProjection::setCameraRightSign )
            .def( "setCameraFOV", &RenderCameraProjection::setCameraFOV )
            .def( "setCameraAspect", &RenderCameraProjection::setCameraAspect )
            .def( "setCameraNear", &RenderCameraProjection::setCameraNear )
            .def( "setCameraFar", &RenderCameraProjection::setCameraFar )
            ;

        pybind::interface_<RenderCameraOrthogonalTarget, pybind::bases<Node> >( _kernel, "RenderCameraOrthogonalTarget", false )
            .def( "setRenderCameraOrthogonal", &RenderCameraOrthogonalTarget::setRenderCameraOrthogonal )
            .def( "getRenderCameraOrthogonal", &RenderCameraOrthogonalTarget::getRenderCameraOrthogonal )
            .def( "setSpeed", &RenderCameraOrthogonalTarget::setSpeed )
            .def( "getSpeed", &RenderCameraOrthogonalTarget::getSpeed )
            .def( "setFixedHorizont", &RenderCameraOrthogonalTarget::setFixedHorizont )
            .def( "getFixedHorizont", &RenderCameraOrthogonalTarget::getFixedHorizont )
            .def( "isFixedHorizont", &RenderCameraOrthogonalTarget::isFixedHorizont )
            ;


        pybind::interface_<RenderScissorInterface, pybind::bases<Mixin> >( _kernel, "RenderScissorInterface", false )
            .def( "getScissorRect", &RenderScissorInterface::getScissorViewport )
            ;

        pybind::interface_<RenderScissor, pybind::bases<Node, RenderScissorInterface> >( _kernel, "RenderScissorInterface", false )
            ;

        pybind::interface_<RenderTargetInterface, pybind::bases<Mixin> >( _kernel, "RenderTargetInterface", false )
            .def( "getWidth", &RenderTargetInterface::getWidth )
            .def( "getHeight", &RenderTargetInterface::getHeight )
            ;

        {


            pybind::interface_<SoundEmitter, pybind::bases<Node> >( _kernel, "SoundEmitter", false )
                .def( "setSurfaceSound", &SoundEmitter::setSurfaceSound )
                .def( "getSurfaceSound", &SoundEmitter::getSurfaceSound )
                ;

            pybind::interface_<TextField, pybind::bases<Node> >( _kernel, "TextField", false )
                .def_deprecated( "setTextByKey", &TextField::setTextID, "use setTextID" )
                .def( "setTextID", &TextField::setTextID )
                .def( "removeTextID", &TextField::removeTextID )
                .def_proxy_native_kernel( "setTextFormatArgs", nodeScriptMethod, &NodeScriptMethod::s_TextField_setTextFormatArgs )
                .def_proxy_static( "getTextFormatArgs", nodeScriptMethod, &NodeScriptMethod::s_TextField_getTextFormatArgs )
                .def( "removeTextFormatArgs", &TextField::removeTextFormatArgs )
                .def_deprecated( "getTextKey", &TextField::getTextID, "use getTextID" )
                .def( "getTextID", &TextField::getTextID )
                .def( "getTextExpectedArgument", &TextField::getTextExpectedArgument )

                .def_deprecated( "getHeight", &TextField::getFontHeight, "use getFontHeight" )
                .def_deprecated( "getAlphaHeight", &TextField::getFontHeight, "use getFontHeight" )
                .def( "getFontHeight", &TextField::getFontHeight )
                .def( "setFontName", &TextField::setFontName )
                .def( "getFontName", &TextField::getFontName )

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

            pybind::interface_<ScriptHolder, pybind::bases<Node, Eventable> >( _kernel, "ScriptHolder", false )
                .def_proxy_native_kernel( "setEventListener", nodeScriptMethod, &NodeScriptMethod::s_ScriptHolder_setEventListener )
                ;

            pybind::interface_<Point, pybind::bases<Node> >( _kernel, "Point", false )
                .def( "setLinkedPoint", &Point::setLinkedPoint )
                .def( "removeLinkedPoint", &Point::removeLinkedPoint )
                .def( "getLinkedPoint", &Point::getLinkedPoint )
                .def( "setWidth", &Point::setWidth )
                .def( "getWidth", &Point::getWidth )
                ;

            pybind::interface_<Line, pybind::bases<Node> >( _kernel, "Line", false )
                .def( "setFrom", &Line::setFrom )
                .def( "getFrom", &Line::getFrom )
                .def( "setTo", &Line::setTo )
                .def( "getTo", &Line::getTo )
                .def( "setWidth", &Line::setWidth )
                .def( "getWidth", &Line::getWidth )
                ;

            pybind::interface_<Layer, pybind::bases<Node> >( _kernel, "Layer", false )
                ;

            pybind::interface_<Layer2D, pybind::bases<Layer> >( _kernel, "Layer2D", false )
                .def( "setSize", &Layer2D::setSize )
                .def( "getSize", &Layer2D::getSize )
                .def( "setViewport", &Layer2D::setViewport )
                .def( "removeViewport", &Layer2D::removeViewport )
                .def( "setImageMask", &Layer2D::setImageMask )
                .def( "removeImageMask", &Layer2D::removeImageMask )
                ;

            //pybind::interface_<Parallax, pybind::bases<Node> >( kernel, "Parallax", false )
            //    .def( "setParallaxFactor", &Parallax::setParallaxFactor )
            //    .def( "getParallaxFactor", &Parallax::getParallaxFactor )
            //    ;

            pybind::interface_<PickerTrapInterface, pybind::bases<Mixin> >( _kernel, "PickerTrap", false )
                .def( "pick", &PickerTrapInterface::pick )
                ;

            pybind::interface_<HotSpot, pybind::bases<Node, Eventable> >( _kernel, "HotSpot", false )
                .def( "testPoint", &HotSpot::testPoint )
                .def( "setOutward", &HotSpot::setOutward )
                .def( "getOutward", &HotSpot::getOutward )
                .def( "setGlobal", &HotSpotPolygon::setGlobal )
                .def( "getGlobal", &HotSpotPolygon::getGlobal )
                .def( "setDefaultHandle", &HotSpot::setDefaultHandle )
                .def( "getDefaultHandle", &HotSpot::getDefaultHandle )
                .def( "isMousePickerOver", &HotSpot::isMousePickerOver )
                .def_proxy_native_kernel( "setEventListener", nodeScriptMethod, &NodeScriptMethod::s_HotSpot_setEventListener )
                .def_proxy_native_kernel( "removeEventListener", nodeScriptMethod, &NodeScriptMethod::s_HotSpot_removeEventListener )
                ;

            pybind::interface_<HotSpotPolygon, pybind::bases<HotSpot> >( _kernel, "HotSpotPolygon", false )
                .def( "setPolygon", &HotSpotPolygon::setPolygon )
                .def( "getPolygon", &HotSpotPolygon::getPolygon )
                .def( "clearPoints", &HotSpotPolygon::clearPoints )
                .def_proxy_static( "getLocalPolygonCenter", nodeScriptMethod, &NodeScriptMethod::s_HotSpotPolygon_getLocalPolygonCenter )
                .def_proxy_static( "getWorldPolygonCenter", nodeScriptMethod, &NodeScriptMethod::s_HotSpotPolygon_getWorldPolygonCenter )
                .def_proxy_static( "getScreenPolygonCenter", nodeScriptMethod, &NodeScriptMethod::s_HotSpotPolygon_getScreenPolygonCenter )
                .def_proxy_static( "getWorldPolygon", nodeScriptMethod, &NodeScriptMethod::s_HotSpotPolygon_getWorldPolygon )
                ;

            pybind::interface_<HotSpotShape, pybind::bases<HotSpotPolygon> >( _kernel, "HotSpotShape", false )
                .def( "setResourceShape", &HotSpotShape::setResourceShape )
                .def( "getResourceShape", &HotSpotShape::getResourceShape )
                ;

            pybind::interface_<HotSpotImage, pybind::bases<HotSpot> >( _kernel, "HotSpotImage", false )
                .def_deprecated( "setResourceHIT", &HotSpotImage::setResourceTestPick, "use setResourceTestPick" )
                .def_deprecated( "getResourceHIT", &HotSpotImage::getResourceTestPick, "use getResourceTestPick" )
                .def( "setResourceTestPick", &HotSpotImage::setResourceTestPick )
                .def( "getResourceTestPick", &HotSpotImage::getResourceTestPick )
                .def( "setAlphaTest", &HotSpotImage::setAlphaTest )
                .def( "getAlphaTest", &HotSpotImage::getAlphaTest )
                .def( "getWidth", &HotSpotImage::getWidth )
                .def( "getHeight", &HotSpotImage::getHeight )
                ;

            pybind::interface_<HotSpotCircle, pybind::bases<HotSpot> >( _kernel, "HotSpotCircle", false )
                .def( "setRadius", &HotSpotCircle::setRadius )
                .def( "getRadius", &HotSpotCircle::getRadius )
                .def( "setEllipse", &HotSpotCircle::setEllipse )
                .def( "getEllipse", &HotSpotCircle::getEllipse )
                ;

            pybind::interface_<HotSpotBubbles, pybind::bases<HotSpot> >( _kernel, "HotSpotBubbles", false )
                .def( "addBubble", &HotSpotBubbles::addBubble )
                ;

            pybind::interface_<Shape, pybind::bases<Node> >( _kernel, "Shape", false )
                .def( "setSurface", &Shape::setSurface )
                .def( "getSurface", &Shape::getSurface )

                .def_proxy_static( "getSurfaceSize", nodeScriptMethod, &NodeScriptMethod::s_Shape_getSurfaceSize )
                .def_proxy_static( "getLocalImageCenter", nodeScriptMethod, &NodeScriptMethod::s_Shape_getLocalImageCenter )
                .def_proxy_static( "getWorldImageCenter", nodeScriptMethod, &NodeScriptMethod::s_Shape_getWorldImageCenter )
                ;


            pybind::interface_<ShapePacMan, pybind::bases<Shape> >( _kernel, "ShapePacMan", false )
                .def( "setAngleFrom", &ShapePacMan::setAngleFrom )
                .def( "getAngleFrom", &ShapePacMan::getAngleFrom )
                .def( "setAngleTo", &ShapePacMan::setAngleTo )
                .def( "getAngleTo", &ShapePacMan::getAngleTo )
                ;

            pybind::interface_<ShapeQuad, pybind::bases<Shape> >( _kernel, "ShapeQuad", false )
                ;

            pybind::interface_<ShapeQuadFixed, pybind::bases<ShapeQuad> >( _kernel, "ShapeQuadFixed", false )
                ;

            pybind::interface_<ShapeQuadFlex, pybind::bases<ShapeQuad> >( _kernel, "ShapeQuadFlex", false )
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

            pybind::interface_<Landscape2D, pybind::bases<Node, Materialable> >( _kernel, "Landscape2D", false )
                .def( "setBackParts", &Landscape2D::setBackParts )
                ;

            pybind::interface_<Grid2D, pybind::bases<Node, Materialable> >( _kernel, "Grid2D", false )
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

            pybind::interface_<Gyroscope, pybind::bases<Node> >( _kernel, "Gyroscope", false )
                ;

            pybind::interface_<Interender, pybind::bases<Node> >( _kernel, "Interender", false )
                ;

            pybind::interface_<Isometric, pybind::bases<Node> >( _kernel, "Isometric", false )
                ;

            pybind::interface_<MatrixProxy, pybind::bases<Node> >( _kernel, "MatrixProxy", false )
                ;


            pybind::interface_<Meshget, pybind::bases<Node, Eventable> >( _kernel, "Meshget", false )
                .def( "setSurface", &Meshget::setSurface )
                .def( "getSurface", &Meshget::getSurface )
                .def( "setVertices", &Meshget::setVertices )
                .def_proxy_native_kernel( "setEventListener", nodeScriptMethod, &NodeScriptMethod::s_Meshget_setEventListener )
                ;

            pybind::interface_<Window, pybind::bases<Node> >( _kernel, "Window", false )
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

        pybind::interface_<SchedulerInterface, pybind::bases<Mixin> >( _kernel, "SchedulerInterface", true )
            .def_proxy_static_args( "timing", nodeScriptMethod, &NodeScriptMethod::ScheduleInterface_timing )
            .def_proxy_static_args( "schedule", nodeScriptMethod, &NodeScriptMethod::ScheduleInterface_schedule )
            .def_proxy_static_args( "pipe", nodeScriptMethod, &NodeScriptMethod::ScheduleInterface_pipe )
            .def( "refresh", &SchedulerInterface::refresh )
            .def( "exist", &SchedulerInterface::exist )
            .def( "remove", &SchedulerInterface::remove )
            .def( "removeAll", &SchedulerInterface::removeAll )
            .def( "freeze", &SchedulerInterface::freeze )
            .def( "freezeAll", &SchedulerInterface::freezeAll )
            .def( "isFreeze", &SchedulerInterface::isFreeze )
            .def_deprecated( "time", &SchedulerInterface::getTimePassed, "use getTimePassed" )
            .def_deprecated( "left", &SchedulerInterface::getTimeLeft, "use getTimeLeft" )
            .def( "getTimePassed", &SchedulerInterface::getTimePassed )
            .def( "getTimeLeft", &SchedulerInterface::getTimeLeft )
            .def( "setSpeedFactor", &SchedulerInterface::setSpeedFactor )
            .def( "getSpeedFactor", &SchedulerInterface::getSpeedFactor )
            .def( "getTiming", &SchedulerInterface::getTime )
            ;

        m_implement = nodeScriptMethod;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void NodeScriptEmbedding::ejecting( pybind::kernel_interface * _kernel )
    {
        MENGINE_UNUSED( _kernel );

#define UNSCRIPT_CLASS_WRAPPING( Class )\
    VOCABULARY_REMOVE(STRINGIZE_STRING_LOCAL("ClassWrapping"), STRINGIZE_STRING_LOCAL(#Class))

        UNSCRIPT_CLASS_WRAPPING( Node );
        UNSCRIPT_CLASS_WRAPPING( Layer );
        UNSCRIPT_CLASS_WRAPPING( Layer2D );
        UNSCRIPT_CLASS_WRAPPING( HotSpot );
        UNSCRIPT_CLASS_WRAPPING( HotSpotPolygon );
        UNSCRIPT_CLASS_WRAPPING( HotSpotCircle );
        UNSCRIPT_CLASS_WRAPPING( HotSpotBubbles );
        UNSCRIPT_CLASS_WRAPPING( HotSpotImage );
        UNSCRIPT_CLASS_WRAPPING( HotSpotShape );

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

        UNSCRIPT_CLASS_WRAPPING( ShapePacMan );
        UNSCRIPT_CLASS_WRAPPING( ShapeQuadFixed );
        UNSCRIPT_CLASS_WRAPPING( ShapeQuadFlex );

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

        m_implement = nullptr;
    }
}
