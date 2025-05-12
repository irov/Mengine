#include "NodeScriptEmbedding.h"

#include "Interface/ApplicationInterface.h"
#include "Interface/TimelineServiceInterface.h"
#include "Interface/RenderSystemInterface.h"
#include "Interface/InputServiceInterface.h"
#include "Interface/PrototypeServiceInterface.h"
#include "Interface/MemoryInterface.h"
#include "Interface/PackageInterface.h"
#include "Interface/SceneServiceInterface.h"
#include "Interface/ScriptServiceInterface.h"
#include "Interface/SchedulerInterface.h"
#include "Interface/FontInterface.h"
#include "Interface/AccountInterface.h"
#include "Interface/ResourceServiceInterface.h"
#include "Interface/TextServiceInterface.h"
#include "Interface/FontServiceInterface.h"
#include "Interface/ApplicationInterface.h"

#include "Environment/Python/PythonAnimatableEventReceiver.h"
#include "Environment/Python/PythonEventReceiver.h"
#include "Environment/Python/PythonDocumentTraceback.h"
#include "Environment/Python/PythonScriptWrapper.h"

#include "Plugins/MoviePlugin/ResourceMovie2.h"

#include "ScriptHolder.h"
#include "ScriptableAffectorCallback.h"
#include "PythonEntityBehavior.h"
#include "PythonHotSpotEventReceiver.h"
#include "PythonScheduleTiming.h"
#include "PythonSchedulePipe.h"
#include "PythonScheduleEvent.h"
#include "DelaySchedulePipe.h"

#include "Engine/ResourceFile.h"
#include "Engine/ResourceMusic.h"
#include "Engine/ResourceTestPick.h"
#include "Engine/ResourceHIT.h"
#include "Engine/ResourceShape.h"
#include "Engine/ResourceCursorICO.h"
#include "Engine/ResourceCursorSystem.h"
#include "Engine/HotSpotPolygon.h"
#include "Engine/HotSpotGlobal.h"
#include "Engine/HotSpotCircle.h"
#include "Engine/HotSpotImage.h"
#include "Engine/HotSpotResourceShape.h"
#include "Engine/HotSpotSurface.h"
#include "Engine/Landscape2D.h"
#include "Engine/Grid2D.h"
#include "Engine/Gyroscope.h"
#include "Engine/TextField.h"
#include "Engine/SoundEmitter.h"
#include "Engine/Point.h"
#include "Engine/Line.h"
#include "Engine/Layer2D.h"
#include "Engine/Meshget.h"
#include "Engine/Isometric.h"
#include "Engine/Window.h"

#include "Kernel/Eventable.h"
#include "Kernel/ThreadTask.h"
#include "Kernel/Scene.h"
#include "Kernel/NodeRenderHierarchy.h"
#include "Kernel/MatrixProxy.h"
#include "Kernel/AssertionMemoryPanic.h"
#include "Kernel/ConstStringHelper.h"
#include "Kernel/UnicodeHelper.h"
#include "Kernel/ResourceSound.h"
#include "Kernel/RenderCameraProjection.h"
#include "Kernel/RenderContextHelper.h"
#include "Kernel/ShapeCircle.h"
#include "Kernel/ShapePacMan.h"
#include "Kernel/ShapeQuadFixed.h"
#include "Kernel/ShapeQuadFlex.h"
#include "Kernel/ShapeQuadSize.h"
#include "Kernel/SurfaceSound.h"
#include "Kernel/SurfaceImage.h"
#include "Kernel/SurfaceImageSequence.h"
#include "Kernel/SurfaceTrackMatte.h"
#include "Kernel/SurfaceSolidColor.h"
#include "Kernel/Interender.h"
#include "Kernel/RenderViewport.h"
#include "Kernel/RenderScissor.h"
#include "Kernel/RenderCameraOrthogonal.h"
#include "Kernel/RenderCameraProjection.h"
#include "Kernel/RenderCameraOrthogonalTarget.h"
#include "Kernel/ResourceImage.h"
#include "Kernel/ResourceImageData.h"
#include "Kernel/ResourceImageEmpty.h"
#include "Kernel/ResourceImageSequence.h"
#include "Kernel/ResourceImageSolid.h"
#include "Kernel/ResourceImageDefault.h"
#include "Kernel/ResourceImageSubstractRGBAndAlpha.h"
#include "Kernel/ResourceImageSubstract.h"
#include "Kernel/Shape.h"
#include "Kernel/Entity.h"
#include "Kernel/Logger.h"
#include "Kernel/Identity.h"
#include "Kernel/Affector.h"
#include "Kernel/AffectorHelper.h"
#include "Kernel/ThreadTask.h"
#include "Kernel/Polygon.h"
#include "Kernel/MemoryStreamHelper.h"
#include "Kernel/ValueFollower.h"
#include "Kernel/Rect.h"
#include "Kernel/Polygon.h"
#include "Kernel/ValueFollower.h"
#include "Kernel/ValueInterpolatorParabolic.h"
#include "Kernel/FactoryPool.h"
#include "Kernel/AssertionFactory.h"

#include "math/angle.h"
#include "math/vec4.h"
#include "math/mat3.h"
#include "math/mat4.h"
#include "math/quat.h"
#include "math/utils.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    namespace
    {
        //////////////////////////////////////////////////////////////////////////
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
            
            //////////////////////////////////////////////////////////////////////////
            PyObject * TextField_setTextFormatArgs( pybind::kernel_interface * _kernel, TextField * _textField, PyObject * _args, PyObject * _kwds )
            {
                MENGINE_UNUSED( _kwds );

                size_t args_count = _kernel->tuple_size( _args );

                VectorTextArguments arguments;
                arguments.reserve( args_count );

                for( uint32_t it = 0; it != args_count; ++it )
                {
                    PyObject * py_obj = _kernel->tuple_getitem( _args, it );

                    if( _kernel->string_check( py_obj ) == true )
                    {
                        String value;
                        if( pybind::extract_value( _kernel, py_obj, value, false ) == false )
                        {
                            LOGGER_ERROR( "text field '%s' invalid get str '%s'"
                                , _textField->getName().c_str()
                                , _kernel->object_repr( py_obj ).c_str()
                            );

                            return _kernel->ret_false();
                        }

                        TextArgumentInterfacePtr argument = TEXT_SERVICE()
                            ->createTextArgument( MENGINE_DOCUMENT_PYBIND );

                        argument->setValue( value );

                        arguments.emplace_back( argument );
                    }
                    else if( _kernel->unicode_check( py_obj ) == true )
                    {
                        WString value;
                        if( pybind::extract_value( _kernel, py_obj, value, false ) == false )
                        {
                            LOGGER_ERROR( "text field '%s' invalid get unicode '%s'"
                                , _textField->getName().c_str()
                                , _kernel->object_repr( py_obj ).c_str()
                            );

                            return _kernel->ret_false();
                        }

                        String utf8_value;
                        Helper::unicodeToUtf8( value, &utf8_value );

                        TextArgumentInterfacePtr argument = TEXT_SERVICE()
                            ->createTextArgument( MENGINE_DOCUMENT_PYBIND );

                        argument->setValue( utf8_value );

                        arguments.emplace_back( argument );
                    }
                    else if( _kernel->is_callable( py_obj ) == true )
                    {
                        TextArgumentInterfacePtr argument = TEXT_SERVICE()
                            ->createTextArgument( MENGINE_DOCUMENT_PYBIND );

                        argument->setContext( [_textField, _kernel, py_obj]( String * _value )
                        {
                            PyObject * py_result = _kernel->ask_native( py_obj, nullptr );

                            String value;
                            if( pybind::extract_value( _kernel, py_result, value, false ) == false )
                            {
                                LOGGER_ERROR( "call context '%s' for text field '%s' invalid get str '%s' type '%s'"
                                    , _kernel->object_repr( py_obj ).c_str()
                                    , _textField->getName().c_str()
                                    , _kernel->object_repr( py_result ).c_str()
                                    , _kernel->object_repr_type( py_result ).c_str()
                                );

                                return false;
                            }

                            if( *_value == value )
                            {
                                return false;
                            }

                            *_value = std::move( value );

                            return true;
                        } );

                        arguments.emplace_back( argument );
                    }
                    else
                    {
                        pybind::string_view py_value_str = _kernel->object_str( py_obj );

                        if( py_value_str.is_invalid() == true )
                        {
                            LOGGER_ERROR( "text field '%s' not suport arg '%s'"
                                , _textField->getName().c_str()
                                , _kernel->object_repr( py_obj ).c_str()
                            );

                            return _kernel->ret_false();
                        }

                        const Char * value_str = py_value_str.c_str();

                        TextArgumentInterfacePtr argument = TEXT_SERVICE()
                            ->createTextArgument( MENGINE_DOCUMENT_PYBIND );

                        argument->setValue( value_str );

                        arguments.emplace_back( argument );
                    }
                }

                _textField->setTextArguments( arguments );

                return _kernel->ret_true();
            }
            //////////////////////////////////////////////////////////////////////////
            VectorWString TextField_getTextFormatArgs( TextField * _textField )
            {
                VectorWString ws_args;

                const VectorTextArguments & _args = _textField->getTextArguments();

                size_t args_count = _args.size();
                ws_args.reserve( args_count );

                for( const TextArgumentInterfacePtr & arg : _args )
                {
                    arg->updateContext();

                    const String & arg_str = arg->getValue();

                    WString arg_unicode;
                    Helper::utf8ToUnicode( arg_str, &arg_unicode );

                    ws_args.emplace_back( arg_unicode );
                }

                return ws_args;
            }
            //////////////////////////////////////////////////////////////////////////
            void TextField_setFontName( TextField * _textField, const ConstString & _fontName )
            {
                const FontInterfacePtr & font = FONT_SERVICE()
                    ->getFont( _fontName );

                _textField->setFont( font );
            }
            //////////////////////////////////////////////////////////////////////////
            const ConstString & TextField_getFontName( TextField * _textField )
            {
                const FontInterfacePtr & font = _textField->calcFont();

                const ConstString & fontName = font->getName();

                return fontName;
            }
            //////////////////////////////////////////////////////////////////////////
            void TextField_setHorizontalNoneAlign( TextField * _textField )
            {
                _textField->setHorizontAlign( ETFHA_NONE );
            }
            //////////////////////////////////////////////////////////////////////////
            bool TextField_isHorizontalNoneAlign( TextField * _textField )
            {
                return _textField->getHorizontAlign() == ETFHA_NONE;
            }
            //////////////////////////////////////////////////////////////////////////
            void TextField_setHorizontalCenterAlign( TextField * _textField )
            {
                _textField->setHorizontAlign( ETFHA_CENTER );
            }
            //////////////////////////////////////////////////////////////////////////
            bool TextField_isHorizontalCenterAlign( TextField * _textField )
            {
                return _textField->getHorizontAlign() == ETFHA_CENTER;
            }
            //////////////////////////////////////////////////////////////////////////
            void TextField_setHorizontalRightAlign( TextField * _textField )
            {
                _textField->setHorizontAlign( ETFHA_RIGHT );
            }
            //////////////////////////////////////////////////////////////////////////
            bool TextField_isHorizontalRightAlign( TextField * _textField )
            {
                return _textField->getHorizontAlign() == ETFHA_RIGHT;
            }
            //////////////////////////////////////////////////////////////////////////
            void TextField_setHorizontalLeftAlign( TextField * _textField )
            {
                _textField->setHorizontAlign( ETFHA_LEFT );
            }
            //////////////////////////////////////////////////////////////////////////
            bool TextField_isHorizontalLeftAlign( TextField * _textField )
            {
                return _textField->getHorizontAlign() == ETFHA_LEFT;
            }
            //////////////////////////////////////////////////////////////////////////
            void TextField_setHorizontalJustifyAlign( TextField * _textField )
            {
                _textField->setHorizontAlign( ETFHA_JUSTIFY );
            }
            //////////////////////////////////////////////////////////////////////////
            bool TextField_isHorizontalJustifyAlign( TextField * _textField )
            {
                return _textField->getHorizontAlign() == ETFHA_JUSTIFY;
            }
            //////////////////////////////////////////////////////////////////////////
            void TextField_setVerticalNoneAlign( TextField * _textField )
            {
                _textField->setVerticalAlign( ETFVA_NONE );
            }
            //////////////////////////////////////////////////////////////////////////
            bool TextField_isVerticalNoneAlign( TextField * _textField )
            {
                return _textField->getVerticalAlign() == ETFVA_NONE;
            }
            //////////////////////////////////////////////////////////////////////////
            void TextField_setVerticalTopAlign( TextField * _textField )
            {
                _textField->setVerticalAlign( ETFVA_TOP );
            }
            //////////////////////////////////////////////////////////////////////////
            bool TextField_isVerticalTopAlign( TextField * _textField )
            {
                return _textField->getVerticalAlign() == ETFVA_TOP;
            }
            //////////////////////////////////////////////////////////////////////////
            void TextField_setVerticalBottomAlign( TextField * _textField )
            {
                _textField->setVerticalAlign( ETFVA_BOTTOM );
            }
            //////////////////////////////////////////////////////////////////////////
            bool TextField_isVerticalBottomAlign( TextField * _textField )
            {
                return _textField->getVerticalAlign() == ETFVA_BOTTOM;
            }
            //////////////////////////////////////////////////////////////////////////
            void TextField_setVerticalCenterAlign( TextField * _textField )
            {
                _textField->setVerticalAlign( ETFVA_CENTER );
            }
            //////////////////////////////////////////////////////////////////////////
            bool TextField_isVerticalCenterAlign( TextField * _textField )
            {
                return _textField->getVerticalAlign() == ETFVA_CENTER;
            }
            //////////////////////////////////////////////////////////////////////////
            PyObject * TextField_calcTotalTextViewport( pybind::kernel_interface * _kernel, TextField * _textField )
            {
                Viewport viewport;
                if( _textField->calcTotalTextViewport( &viewport ) == false )
                {
                    _kernel->ret_none();
                }

                return pybind::ptr( _kernel, viewport );
            }
            //////////////////////////////////////////////////////////////////////////
            mt::vec2f TextField_calcTotalTextSize( TextField * _textField )
            {
                mt::vec2f size;
                _textField->calcTotalTextSize( &size );

                return size;
            }
            //////////////////////////////////////////////////////////////////////////
            mt::box2f s_BoundingBox_getBoundingBox( BoundingBox * _boundingBox )
            {
                const mt::box2f * bb = _boundingBox->getBoundingBox();

                if( bb == nullptr )
                {
                    mt::box2f infinity_bb;
                    mt::box2_infinity( &infinity_bb );

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
            mt::vec2f HotSpotPolygon_getLocalPolygonCenter( HotSpotPolygon * _hs )
            {
                const Polygon & polygon = _hs->getPolygon();

                mt::box2f bb;
                polygon.to_box2f( &bb );

                mt::vec2f c;
                mt::box2_get_center( bb, &c );

                return c;
            }
            //////////////////////////////////////////////////////////////////////////
            mt::vec2f HotSpotPolygon_getWorldPolygonCenter( HotSpotPolygon * _hs )
            {
                const Polygon & polygon = _hs->getPolygon();

                mt::box2f bb;
                mt::box2_insideout( &bb );

                const TransformationInterface * transformation = _hs->getTransformation();

                const mt::mat4f & wm = transformation->getWorldMatrix();

                for( const mt::vec2f & v : polygon )
                {
                    mt::vec2f v_wm;
                    mt::mul_v2_v2_m4( &v_wm, v, wm );

                    mt::box2_add_internal_point( &bb, v_wm );
                }

                mt::vec2f c;
                mt::box2_get_center( bb, &c );

                return c;
            }
            //////////////////////////////////////////////////////////////////////////
            mt::vec2f HotSpotPolygon_getScreenPolygonCenter( HotSpotPolygon * _hs )
            {
                const RenderCameraInterface * camera = Helper::getNodeRenderCameraInheritance( _hs );
                const RenderViewportInterface * viewport = Helper::getNodeRenderViewportInheritance( _hs );

                MENGINE_ASSERTION_MEMORY_PANIC( camera, "invalid get node '%s' render camera inheritance"
                    , _hs->getName().c_str()
                );

                MENGINE_ASSERTION_MEMORY_PANIC( viewport, "invalid get node '%s' render viewport inheritance"
                    , _hs->getName().c_str()
                );

                RenderContext context;
                Helper::clearRenderContext( &context );

                context.camera = camera;
                context.viewport = viewport;

                mt::box2f b1;
                _hs->getScreenPolygon( &context, &b1, nullptr );

                mt::vec2f c;
                mt::box2_get_center( b1, &c );

                return c;
            }
            //////////////////////////////////////////////////////////////////////////
            Polygon HotSpotPolygon_getWorldPolygon( HotSpotPolygon * _hs )
            {
                const Polygon & polygon = _hs->getPolygon();

                const TransformationInterface * transformation = _hs->getTransformation();

                const mt::mat4f & wm = transformation->getWorldMatrix();

                Polygon pwm;
                polygon.mul_wm( &pwm, wm );

                return pwm;
            }
            //////////////////////////////////////////////////////////////////////////
            const mt::vec2f & Shape_getSurfaceSize( Shape * _shape )
            {
                const SurfacePtr & surface = _shape->getSurface();

                MENGINE_ASSERTION_MEMORY_PANIC( surface, "shape '%s' not setup surface"
                    , _shape->getName().c_str()
                );

                const mt::vec2f & size = surface->getSize();

                return size;
            }
            //////////////////////////////////////////////////////////////////////////
            mt::vec2f Shape_getLocalImageCenter( Shape * _shape )
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
            mt::vec2f Shape_getWorldImageCenter( Shape * _shape )
            {
                mt::vec2f imageCenter = Shape_getLocalImageCenter( _shape );

                const TransformationInterface * transformation = _shape->getTransformation();

                const mt::mat4f & wm = transformation->getWorldMatrix();

                mt::vec2f imageCenter_wm;
                mt::mul_v2_v2_m4( &imageCenter_wm, imageCenter, wm );

                return imageCenter_wm;
            }
            //////////////////////////////////////////////////////////////////////////
            IntrusivePtr<NodeAffectorCreator::NodeAffectorCreatorInterpolateLinear<mt::vec4f>> m_nodeAffectorCreatorInterpolateLinearVec4;
            //////////////////////////////////////////////////////////////////////////
            FactoryInterfacePtr m_factoryNodeAffectorCallback;
            //////////////////////////////////////////////////////////////////////////
            ScriptableAffectorCallbackPtr createNodeAffectorCallback( Scriptable * _scriptable, const pybind::object & _cb, const pybind::args & _args )
            {
                ScriptableAffectorCallbackPtr callback = m_factoryNodeAffectorCallback->createObject( MENGINE_DOCUMENT_PYBIND );

                callback->initialize( ScriptablePtr( _scriptable ), _cb, _args );

                return callback;
            }
            //////////////////////////////////////////////////////////////////////////
            uint32_t ShapeQuadFlex_setPercentVisibilityTo( ShapeQuadFlex * _shape, float _time, const mt::vec4f & _percent, const ConstString & _easingType, const pybind::object & _cb, const pybind::args & _args )
            {
                if( _shape->isActivate() == false )
                {
                    return 0;
                }

                EasingInterfacePtr easing = VOCABULARY_GET( STRINGIZE_STRING_LOCAL( "Easing" ), _easingType );

                ScriptableAffectorCallbackPtr callback = createNodeAffectorCallback( _shape, _cb, _args );

                AffectorPtr affector = m_nodeAffectorCreatorInterpolateLinearVec4->create( EAFFECTORTYPE_VISIBILITY
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

                affectorHub->stopAffectors( EAFFECTORTYPE_VISIBILITY );

                if( _shape->isActivate() == false )
                {
                    return 0;
                }

                UniqueId id = affectorHub->addAffector( affector );

                return id;
            }
            //////////////////////////////////////////////////////////////////////////
            void ShapeQuadFlex_setPercentVisibilityStop( ShapeQuadFlex * _shape )
            {
                const AffectorHubInterfacePtr & affectorHub = _shape->getAffectorHub();

                MENGINE_ASSERTION_MEMORY_PANIC( affectorHub, "shape '%s' not setup affector hub"
                    , _shape->getName().c_str()
                );

                affectorHub->stopAffectors( EAFFECTORTYPE_VISIBILITY );
            }
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
            PyObject * SurfaceImageSequence_setEventListener( pybind::kernel_interface * _kernel, SurfaceImageSequence * _surface, PyObject * _args, PyObject * _kwds )
            {
                MENGINE_UNUSED( _args );

                MENGINE_ASSERTION_MEMORY_PANIC( _kwds, "invalid set event listener" );

                pybind::dict py_kwds( _kernel, _kwds );
                Helper::registerAnimatableEventReceiver<>( _kernel, py_kwds, _surface, MENGINE_DOCUMENT_PYBIND );

                MENGINE_ASSERTION_PYTHON_EVENT_RECEIVER( _surface, py_kwds );

                return _kernel->ret_none();
            }
            //////////////////////////////////////////////////////////////////////////
            PyObject * SurfaceSound_setEventListener( pybind::kernel_interface * _kernel, SurfaceSound * _surface, PyObject * _args, PyObject * _kwds )
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
            PyObject * Meshget_setEventListener( pybind::kernel_interface * _kernel, Meshget * _node, PyObject * _args, PyObject * _kwds )
            {
                MENGINE_UNUSED( _args );

                MENGINE_ASSERTION_MEMORY_PANIC( _kwds, "invalid set event listener" );

                pybind::dict py_kwds( _kernel, _kwds );
                Helper::registerPythonEventReceiver<PythonMeshEventReceiver>( _kernel, py_kwds, _node, STRINGIZE_STRING_LOCAL( "onMeshgetUpdate" ), EVENT_MESHGET_UPDATE, MENGINE_DOCUMENT_PYBIND );

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
            PyObject * ScriptHolder_setEventListener( pybind::kernel_interface * _kernel, ScriptHolder * _node, PyObject * _args, PyObject * _kwds )
            {
                MENGINE_UNUSED( _args );

                MENGINE_ASSERTION_MEMORY_PANIC( _kwds, "invalid set event listener" );

                pybind::dict py_kwds( _kernel, _kwds );
                Helper::registerPythonEventReceiver<PythonScriptHolderEventReceiver>( _kernel, py_kwds, _node, STRINGIZE_STRING_LOCAL( "onKeepScript" ), EVENT_SCRIPT_HOLDER_KEEP, MENGINE_DOCUMENT_PYBIND );
                Helper::registerPythonEventReceiver<PythonScriptHolderEventReceiver>( _kernel, py_kwds, _node, STRINGIZE_STRING_LOCAL( "onReleaseScript" ), EVENT_SCRIPT_HOLDER_RELEASE, MENGINE_DOCUMENT_PYBIND );

                MENGINE_ASSERTION_PYTHON_EVENT_RECEIVER( _node, py_kwds );

                return _kernel->ret_none();
            }
            //////////////////////////////////////////////////////////////////////////
            PyObject * HotSpot_setEventListener( pybind::kernel_interface * _kernel, HotSpot * _node, PyObject * _args, PyObject * _kwds )
            {
                MENGINE_UNUSED( _args );

                MENGINE_ASSERTION_MEMORY_PANIC( _kwds, "invalid set event listener" );

                pybind::dict py_kwds( _kernel, _kwds );

                Helper::registerPythonEventReceiver<PythonHotSpotEventReceiver>( _kernel, py_kwds, _node, STRINGIZE_STRING_LOCAL( "onHandleKeyEvent" ), EVENT_HOTSPOT_KEY, MENGINE_DOCUMENT_PYBIND );
                Helper::registerPythonEventReceiver<PythonHotSpotEventReceiver>( _kernel, py_kwds, _node, STRINGIZE_STRING_LOCAL( "onHandleMouseButtonEvent" ), EVENT_HOTSPOT_MOUSE_BUTTON, MENGINE_DOCUMENT_PYBIND );
                Helper::registerPythonEventReceiver<PythonHotSpotEventReceiver>( _kernel, py_kwds, _node, STRINGIZE_STRING_LOCAL( "onHandleMouseButtonEventBegin" ), EVENT_HOTSPOT_MOUSE_BUTTON_BEGIN, MENGINE_DOCUMENT_PYBIND );
                Helper::registerPythonEventReceiver<PythonHotSpotEventReceiver>( _kernel, py_kwds, _node, STRINGIZE_STRING_LOCAL( "onHandleMouseButtonEventEnd" ), EVENT_HOTSPOT_MOUSE_BUTTON_END, MENGINE_DOCUMENT_PYBIND );
                Helper::registerPythonEventReceiver<PythonHotSpotEventReceiver>( _kernel, py_kwds, _node, STRINGIZE_STRING_LOCAL( "onHandleMouseMove" ), EVENT_HOTSPOT_MOUSE_MOVE, MENGINE_DOCUMENT_PYBIND );
                Helper::registerPythonEventReceiver<PythonHotSpotEventReceiver>( _kernel, py_kwds, _node, STRINGIZE_STRING_LOCAL( "onHandleMouseWheel" ), EVENT_HOTSPOT_MOUSE_WHEEL, MENGINE_DOCUMENT_PYBIND );
                Helper::registerPythonEventReceiver<PythonHotSpotEventReceiver>( _kernel, py_kwds, _node, STRINGIZE_STRING_LOCAL( "onHandleMouseEnter" ), EVENT_HOTSPOT_MOUSE_ENTER, MENGINE_DOCUMENT_PYBIND );
                Helper::registerPythonEventReceiver<PythonHotSpotEventReceiver>( _kernel, py_kwds, _node, STRINGIZE_STRING_LOCAL( "onHandleMouseLeave" ), EVENT_HOTSPOT_MOUSE_LEAVE, MENGINE_DOCUMENT_PYBIND );
                Helper::registerPythonEventReceiver<PythonHotSpotEventReceiver>( _kernel, py_kwds, _node, STRINGIZE_STRING_LOCAL( "onHandleMouseOverDestroy" ), EVENT_HOTSPOT_MOUSE_OVER_DESTROY, MENGINE_DOCUMENT_PYBIND );
                Helper::registerPythonEventReceiver<PythonHotSpotEventReceiver>( _kernel, py_kwds, _node, STRINGIZE_STRING_LOCAL( "onHandleActivate" ), EVENT_HOTSPOT_ACTIVATE, MENGINE_DOCUMENT_PYBIND );
                Helper::registerPythonEventReceiver<PythonHotSpotEventReceiver>( _kernel, py_kwds, _node, STRINGIZE_STRING_LOCAL( "onHandleDeactivate" ), EVENT_HOTSPOT_DEACTIVATE, MENGINE_DOCUMENT_PYBIND );

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
    bool NodeScriptEmbedding::embed( pybind::kernel_interface * _kernel )
    {
        NodeScriptMethodPtr nodeScriptMethod = Helper::makeFactorableUnique<NodeScriptMethod>( MENGINE_DOCUMENT_FACTORABLE );

        if( nodeScriptMethod->initialize() == false )
        {
            return false;
        }

        pybind::interface_<SurfaceSound, pybind::bases<Surface, Soundable>>( _kernel, "SurfaceSound", false )
            .def( "setResourceSound", &SurfaceSound::setResourceSound )
            .def( "getResourceSound", &SurfaceSound::getResourceSound )
            .def_proxy_native_kernel( "setEventListener", nodeScriptMethod, &NodeScriptMethod::SurfaceSound_setEventListener )
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
            .def_proxy_native_kernel( "setEventListener", nodeScriptMethod, &NodeScriptMethod::SurfaceImageSequence_setEventListener )
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
            .def( "getViewport", &RenderViewport::getViewport )         
            ;

        pybind::interface_<RenderScissor, pybind::bases<Node, RenderScissorInterface>>( _kernel, "RenderScissor", false )
            .def( "setScissorViewport", &RenderScissor::setScissorViewport )
            .def( "getScissorViewport", &RenderScissor::getScissorViewport )
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

        {
            pybind::interface_<SoundEmitter, pybind::bases<Node>>( _kernel, "SoundEmitter", false )
                .def( "setSurfaceSound", &SoundEmitter::setSurfaceSound )
                .def( "getSurfaceSound", &SoundEmitter::getSurfaceSound )
                ;

            pybind::interface_<FontInterface, pybind::bases<ServantInterface>>( _kernel, "FontInterface" )
                .def( "setName", &FontInterface::setName )
                .def( "getName", &FontInterface::getName )
                .def( "setHeight", &FontInterface::setHeight )
                .def( "getHeight", &FontInterface::getHeight )
                .def( "setFontColor", &FontInterface::setFontColor )
                .def( "getFontColor", &FontInterface::getFontColor )
                .def( "setLineOffset", &FontInterface::setLineOffset )
                .def( "getLineOffset", &FontInterface::getLineOffset )
                .def( "setCharOffset", &FontInterface::setCharOffset )
                .def( "getCharOffset", &FontInterface::getCharOffset )
                .def( "initialize", &FontInterface::initialize )
                .def( "finalize", &FontInterface::finalize )
                .def( "getFontAscender", &FontInterface::getFontAscender )
                .def( "getFontDescender", &FontInterface::getFontDescender )
                .def( "getFontHeight", &FontInterface::getFontHeight )
                .def( "getFontBearingYA", &FontInterface::getFontBearingYA )
                .def( "getFontSpacing", &FontInterface::getFontSpacing )
                .def( "getFontPremultiply", &FontInterface::getFontPremultiply )
                ;

            pybind::enum_<ETextHorizontAlign>( _kernel, "ETextHorizontAlign" )
                .def( "ETFHA_NONE", ETFHA_NONE )
                .def( "ETFHA_LEFT", ETFHA_LEFT )
                .def( "ETFHA_CENTER", ETFHA_CENTER )
                .def( "ETFHA_RIGHT", ETFHA_RIGHT )
                .def( "ETFHA_JUSTIFY", ETFHA_JUSTIFY )                
                ;

            pybind::enum_<ETextVerticalAlign>( _kernel, "ETextVerticalAlign" )
                .def( "ETFVA_NONE", ETFVA_NONE )
                .def( "ETFVA_TOP", ETFVA_TOP )
                .def( "ETFVA_BOTTOM", ETFVA_BOTTOM )
                .def( "ETFVA_CENTER", ETFVA_CENTER )
                ;

            pybind::interface_<TextField, pybind::bases<Node>>( _kernel, "TextField", false )
                .def( "setTextId", &TextField::setTextId )
                .def( "removeTextId", &TextField::removeTextId )
                .def_proxy_native_kernel( "setTextFormatArgs", nodeScriptMethod, &NodeScriptMethod::TextField_setTextFormatArgs )
                .def_proxy_static( "getTextFormatArgs", nodeScriptMethod, &NodeScriptMethod::TextField_getTextFormatArgs )
                .def( "removeTextFormatArgs", &TextField::removeTextArguments )
                .def( "getTextId", &TextField::getTextId )
                .def( "getTotalTextId", &TextField::getTotalTextId )
                .def( "getTextExpectedArgument", &TextField::getTextExpectedArgument )

                .def( "setTextAliasEnvironment", &TextField::setTextAliasEnvironment )
                .def( "getTextAliasEnvironment", &TextField::getTextAliasEnvironment )

                .def( "getFontHeight", &TextField::getFontHeight )
                .def( "setFont", &TextField::setFont )
                .def( "getFont", &TextField::getFont )
                .def_proxy_static( "setFontName", nodeScriptMethod, &NodeScriptMethod::TextField_setFontName )
                .def_proxy_static( "getFontName", nodeScriptMethod, &NodeScriptMethod::TextField_getFontName )

                .def( "setWrap", &TextField::setWrap )
                .def( "getWrap", &TextField::getWrap )

                .def( "setFontColor", &TextField::setFontColor )
                .def( "getFontColor", &TextField::getFontColor )

                .def( "setLineOffset", &TextField::setLineOffset )
                .def( "getLineOffset", &TextField::getLineOffset )

                .def( "setCharOffset", &TextField::setCharOffset )
                .def( "getCharOffset", &TextField::getCharOffset )

                .def( "getTextSize", &TextField::getTextSize )
                
                .def( "setMaxLength", &TextField::setMaxLength )
                .def( "getMaxLength", &TextField::getMaxLength )
                .def( "removeMaxLength", &TextField::removeMaxLength )
                .def( "hasMaxLength", &TextField::hasMaxLength )

                .def( "calcMaxLength", &TextField::calcMaxLength )

                .def( "setAutoScale", &TextField::setAutoScale )
                .def( "getAutoScale", &TextField::getAutoScale )

                .def( "setAnchorHorizontalAlign", &TextField::setAnchorHorizontalAlign )
                .def( "getAnchorHorizontalAlign", &TextField::getAnchorHorizontalAlign )

                .def( "setAnchorVerticalAlign", &TextField::setAnchorVerticalAlign )
                .def( "getAnchorVerticalAlign", &TextField::getAnchorVerticalAlign )

                .def( "setAnchorPercent", &TextField::setAnchorPercent )
                .def( "getAnchorPercent", &TextField::getAnchorPercent )

                .def( "setHorizontAlign", &TextField::setHorizontAlign )
                .def( "getHorizontAlign", &TextField::getHorizontAlign )

                .def( "setVerticalAlign", &TextField::setVerticalAlign )
                .def( "getVerticalAlign", &TextField::getVerticalAlign )

                .def_proxy_static( "setHorizontalNoneAlign", nodeScriptMethod, &NodeScriptMethod::TextField_setHorizontalNoneAlign )
                .def_proxy_static( "isHorizontalNoneAlign", nodeScriptMethod, &NodeScriptMethod::TextField_isHorizontalNoneAlign )
                .def_proxy_static( "setHorizontalCenterAlign", nodeScriptMethod, &NodeScriptMethod::TextField_setHorizontalCenterAlign )
                .def_proxy_static( "isHorizontalCenterAlign", nodeScriptMethod, &NodeScriptMethod::TextField_isHorizontalCenterAlign )
                .def_proxy_static( "setHorizontalRightAlign", nodeScriptMethod, &NodeScriptMethod::TextField_setHorizontalRightAlign )
                .def_proxy_static( "isHorizontalRightAlign", nodeScriptMethod, &NodeScriptMethod::TextField_isHorizontalRightAlign )
                .def_proxy_static( "setHorizontalLeftAlign", nodeScriptMethod, &NodeScriptMethod::TextField_setHorizontalLeftAlign )                
                .def_proxy_static( "isHorizontalLeftAlign", nodeScriptMethod, &NodeScriptMethod::TextField_isHorizontalLeftAlign )
                .def_proxy_static( "setHorizontalJustifyAlign", nodeScriptMethod, &NodeScriptMethod::TextField_setHorizontalJustifyAlign )
                .def_proxy_static( "isHorizontalJustifyAlign", nodeScriptMethod, &NodeScriptMethod::TextField_isHorizontalJustifyAlign )
                .def_proxy_static( "setVerticalNoneAlign", nodeScriptMethod, &NodeScriptMethod::TextField_setVerticalNoneAlign )
                .def_proxy_static( "isVerticalNoneAlign", nodeScriptMethod, &NodeScriptMethod::TextField_isVerticalNoneAlign )
                .def_proxy_static( "setVerticalTopAlign", nodeScriptMethod, &NodeScriptMethod::TextField_setVerticalTopAlign )
                .def_proxy_static( "isVerticalTopAlign", nodeScriptMethod, &NodeScriptMethod::TextField_isVerticalTopAlign )
                .def_proxy_static( "setVerticalBottomAlign", nodeScriptMethod, &NodeScriptMethod::TextField_setVerticalBottomAlign )
                .def_proxy_static( "isVerticalBottomAlign", nodeScriptMethod, &NodeScriptMethod::TextField_isVerticalBottomAlign )
                .def_proxy_static( "setVerticalCenterAlign", nodeScriptMethod, &NodeScriptMethod::TextField_setVerticalCenterAlign )
                .def_proxy_static( "isVerticalCenterAlign", nodeScriptMethod, &NodeScriptMethod::TextField_isVerticalCenterAlign )

                .def( "setPixelsnap", &TextField::setPixelsnap )
                .def( "getPixelsnap", &TextField::getPixelsnap )

                .def( "setMaxCharCount", &TextField::setMaxCharCount )
                .def( "getMaxCharCount", &TextField::getMaxCharCount )
                .def( "getCharCount", &TextField::getCharCount )

                .def_proxy_static_kernel( "calcTotalTextViewport", nodeScriptMethod, &NodeScriptMethod::TextField_calcTotalTextViewport )
                .def_proxy_static( "calcTotalTextSize", nodeScriptMethod, &NodeScriptMethod::TextField_calcTotalTextSize )
                ;

            pybind::interface_<ScriptHolder, pybind::bases<Node>>( _kernel, "ScriptHolder", false )
                .def_proxy_native_kernel( "setEventListener", nodeScriptMethod, &NodeScriptMethod::ScriptHolder_setEventListener )
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

            pybind::interface_<HotSpot, pybind::bases<Node>>( _kernel, "HotSpot", false )
                .def( "testPoint", &HotSpot::testPoint )
                .def( "testRadius", &HotSpot::testRadius )
                .def( "testPolygon", &HotSpot::testPolygon )
                .def( "testBounds", &HotSpot::testBounds )
                .def( "setOutward", &HotSpot::setOutward )
                .def( "getOutward", &HotSpot::getOutward )
                .def( "setGlobal", &HotSpot::setGlobal )
                .def( "getGlobal", &HotSpot::getGlobal )
                .def( "setExclusive", &HotSpot::setExclusive )
                .def( "getExclusive", &HotSpot::getExclusive )
                .def( "setDefaultHandle", &HotSpot::setDefaultHandle )
                .def( "getDefaultHandle", &HotSpot::getDefaultHandle )
                .def( "isMousePickerOver", &HotSpot::isMousePickerOver )
                .def_proxy_native_kernel( "setEventListener", nodeScriptMethod, &NodeScriptMethod::HotSpot_setEventListener )
                ;

            pybind::interface_<HotSpotGlobal, pybind::bases<HotSpot>>( _kernel, "HotSpotGlobal", false )
                ;

            pybind::interface_<HotSpotPolygon, pybind::bases<HotSpot>>( _kernel, "HotSpotPolygon", false )
                .def( "setPolygon", &HotSpotPolygon::setPolygon )
                .def( "getPolygon", &HotSpotPolygon::getPolygon )
                .def( "clearPoints", &HotSpotPolygon::clearPoints )
                .def_proxy_static( "getLocalPolygonCenter", nodeScriptMethod, &NodeScriptMethod::HotSpotPolygon_getLocalPolygonCenter )
                .def_proxy_static( "getWorldPolygonCenter", nodeScriptMethod, &NodeScriptMethod::HotSpotPolygon_getWorldPolygonCenter )
                .def_proxy_static( "getScreenPolygonCenter", nodeScriptMethod, &NodeScriptMethod::HotSpotPolygon_getScreenPolygonCenter )
                .def_proxy_static( "getWorldPolygon", nodeScriptMethod, &NodeScriptMethod::HotSpotPolygon_getWorldPolygon )
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

            pybind::interface_<Shape, pybind::bases<Node>>( _kernel, "Shape", false )
                .def( "setSurface", &Shape::setSurface )
                .def( "getSurface", &Shape::getSurface )
                .def_proxy_static( "getSurfaceSize", nodeScriptMethod, &NodeScriptMethod::Shape_getSurfaceSize )
                .def_proxy_static( "getLocalImageCenter", nodeScriptMethod, &NodeScriptMethod::Shape_getLocalImageCenter )
                .def_proxy_static( "getWorldImageCenter", nodeScriptMethod, &NodeScriptMethod::Shape_getWorldImageCenter )
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
                .def( "setUV", &ShapeQuadFlex::setUV )
                .def( "getUV", &ShapeQuadFlex::getUV )
                .def( "setPercentVisibility", &ShapeQuadFlex::setPercentVisibility )
                .def( "getPercentVisibility", &ShapeQuadFlex::getPercentVisibility )
                .def( "setCustomSize", &ShapeQuadFlex::setCustomSize )
                .def( "getCustomSize", &ShapeQuadFlex::getCustomSize )
                .def( "removeCustomSize", &ShapeQuadFlex::removeCustomSize )
                .def( "hasCustomSize", &ShapeQuadFlex::hasCustomSize )
                .def_proxy_static_args( "setPercentVisibilityTo", nodeScriptMethod, &NodeScriptMethod::ShapeQuadFlex_setPercentVisibilityTo )
                .def_proxy_static( "setPercentVisibilityStop", nodeScriptMethod, &NodeScriptMethod::ShapeQuadFlex_setPercentVisibilityStop )
                .def( "setTextureUVOffset", &ShapeQuadFlex::setTextureUVOffset )
                .def( "getTextureUVOffset", &ShapeQuadFlex::getTextureUVOffset )
                .def( "setTextureUVScale", &ShapeQuadFlex::setTextureUVScale )
                .def( "getTextureUVScale", &ShapeQuadFlex::getTextureUVScale )
                ;

            pybind::interface_<ShapeQuadSize, pybind::bases<ShapeQuad>>( _kernel, "ShapeQuadSize", false )
                .def( "setSize", &ShapeQuadSize::setSize )
                .def( "getSize", &ShapeQuadSize::getSize )
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


            pybind::interface_<Meshget, pybind::bases<Node>>( _kernel, "Meshget", false )
                .def( "setSurface", &Meshget::setSurface )
                .def( "getSurface", &Meshget::getSurface )
                .def( "setVertices", &Meshget::setVertices )
                .def_proxy_native_kernel( "setEventListener", nodeScriptMethod, &NodeScriptMethod::Meshget_setEventListener )
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

        Helper::registerScriptWrappingEx<Node>( _kernel, Node::getFactorableType(), MENGINE_DOCUMENT_FACTORABLE );
        Helper::registerScriptWrappingEx<Layer>( _kernel, Layer::getFactorableType(), MENGINE_DOCUMENT_FACTORABLE );
        Helper::registerScriptWrappingEx<HotSpot>( _kernel, HotSpot::getFactorableType(), MENGINE_DOCUMENT_FACTORABLE );
        Helper::registerScriptWrappingEx<Surface>( _kernel, Surface::getFactorableType(), MENGINE_DOCUMENT_FACTORABLE );

#define SCRIPT_CLASS_WRAPPING( Class )\
    Helper::registerScriptWrapping<Class>(_kernel, MENGINE_DOCUMENT_FACTORABLE)

        SCRIPT_CLASS_WRAPPING( Layer2D );
        
        SCRIPT_CLASS_WRAPPING( HotSpotPolygon );
        SCRIPT_CLASS_WRAPPING( HotSpotGlobal );
        SCRIPT_CLASS_WRAPPING( HotSpotCircle );
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
        SCRIPT_CLASS_WRAPPING( ShapeQuadSize );
        SCRIPT_CLASS_WRAPPING( ShapeQuadFixed );
        SCRIPT_CLASS_WRAPPING( ShapeQuadFlex );

        SCRIPT_CLASS_WRAPPING( Window );

        SCRIPT_CLASS_WRAPPING( RenderViewport );
        SCRIPT_CLASS_WRAPPING( RenderScissor );
        SCRIPT_CLASS_WRAPPING( RenderCameraOrthogonal );
        SCRIPT_CLASS_WRAPPING( RenderCameraProjection );
        SCRIPT_CLASS_WRAPPING( RenderCameraOrthogonalTarget );

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
    void NodeScriptEmbedding::eject( pybind::kernel_interface * _kernel )
    {
        MENGINE_UNUSED( _kernel );

        Helper::unregisterScriptWrappingEx( Node::getFactorableType() );
        Helper::unregisterScriptWrappingEx( Resource::getFactorableType() );
        Helper::unregisterScriptWrappingEx( Layer::getFactorableType() );
        Helper::unregisterScriptWrappingEx( HotSpot::getFactorableType() );
        Helper::unregisterScriptWrappingEx( Surface::getFactorableType() );

#define UNSCRIPT_CLASS_WRAPPING( Class )\
    Helper::unregisterScriptWrapping<Class>()

        UNSCRIPT_CLASS_WRAPPING( Layer2D );
        UNSCRIPT_CLASS_WRAPPING( HotSpotPolygon );
        UNSCRIPT_CLASS_WRAPPING( HotSpotGlobal );
        UNSCRIPT_CLASS_WRAPPING( HotSpotCircle );
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
        UNSCRIPT_CLASS_WRAPPING( ShapeQuadSize );
        UNSCRIPT_CLASS_WRAPPING( ShapeQuadFixed );
        UNSCRIPT_CLASS_WRAPPING( ShapeQuadFlex );

        UNSCRIPT_CLASS_WRAPPING( Window );

        UNSCRIPT_CLASS_WRAPPING( RenderViewport );
        UNSCRIPT_CLASS_WRAPPING( RenderScissor );
        UNSCRIPT_CLASS_WRAPPING( RenderCameraOrthogonal );
        UNSCRIPT_CLASS_WRAPPING( RenderCameraProjection );
        UNSCRIPT_CLASS_WRAPPING( RenderCameraOrthogonalTarget );

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
    //////////////////////////////////////////////////////////////////////////
}
