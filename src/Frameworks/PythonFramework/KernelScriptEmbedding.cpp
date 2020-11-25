#include "KernelScriptEmbedding.h"

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
#include "Interface/AnimationInterface.h"
#include "Interface/RenderSystemInterface.h"

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

#include "ScriptHolder.h"

#include "Environment/Python/PythonAnimatableEventReceiver.h"
#include "Environment/Python/PythonEventReceiver.h"
#include "Environment/Python/PythonDocumentTraceback.h"
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
#include "Kernel/Rect.h"
#include "Kernel/Polygon.h"
#include "Kernel/ValueFollower.h"
#include "Kernel/ValueInterpolatorParabolic.h"
#include "Kernel/FactoryPool.h"
#include "Kernel/AssertionFactory.h"
#include "Kernel/Identity.h"
#include "Kernel/Affector.h"
#include "Kernel/AffectorHelper.h"
#include "Kernel/ThreadTask.h"
#include "Kernel/Interender.h"
#include "Kernel/RenderViewport.h"
#include "Kernel/RenderScissor.h"
#include "Kernel/RenderCameraOrthogonal.h"
#include "Kernel/RenderCameraProjection.h"
#include "Kernel/RenderCameraOrthogonalTarget.h"
#include "Kernel/ResourceImage.h"
#include "Kernel/Shape.h"
#include "Kernel/Entity.h"
#include "Kernel/Logger.h"
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
#include "Kernel/Soundable.h"
#include "Kernel/Materialable.h"
#include "Kernel/Surface.h"

#include "Config/StdIO.h"

#include "math/angle.h"
#include "math/vec4.h"
#include "math/mat3.h"
#include "math/mat4.h"
#include "math/quat.h"
#include "math/utils.h"

#include "utf8.h"

namespace Mengine
{
    namespace
    {
        class KernelScriptMethod
            : public Factorable
        {
        public:
            KernelScriptMethod()
            {
            }

            ~KernelScriptMethod() override
            {
            }

        public:
            bool initialize()
            {
                m_factoryPythonScheduleEvent = Helper::makeFactoryPool<PythonScheduleEvent, 16>( MENGINE_DOCUMENT_FACTORABLE );
                m_factoryDelaySchedulePipe = Helper::makeFactoryPool<DelaySchedulePipe, 16>( MENGINE_DOCUMENT_FACTORABLE );
                m_factoryPythonScheduleTiming = Helper::makeFactoryPool<PythonScheduleTiming, 16>( MENGINE_DOCUMENT_FACTORABLE );
                m_factoryPythonSchedulePipe = Helper::makeFactoryPool<PythonSchedulePipe, 16>( MENGINE_DOCUMENT_FACTORABLE );
                m_factoryNodeAffectorCallback = Helper::makeFactoryPool<ScriptableAffectorCallback, 4>( MENGINE_DOCUMENT_FACTORABLE );

                m_factoryAffectorVelocity2 = Helper::makeFactorableUnique<FactoryAffectorVelocity2>( MENGINE_DOCUMENT_FACTORABLE );
                m_factoryAffectorVelocity2->initialize();

                m_nodeAffectorCreatorInterpolateParabolic = Helper::makeFactorableUnique<FactoryAffectorInterpolateParabolic>( MENGINE_DOCUMENT_FACTORABLE );
                m_nodeAffectorCreatorInterpolateParabolic->initialize();

                m_nodeAffectorCreatorFollowTo = Helper::makeFactorableUnique<FactoryAffectorFollowTo>( MENGINE_DOCUMENT_FACTORABLE );
                m_nodeAffectorCreatorFollowTo->initialize();

                m_nodeAffectorCreatorFollowToW = Helper::makeFactorableUnique<FactoryAffectorFollowToW>( MENGINE_DOCUMENT_FACTORABLE );
                m_nodeAffectorCreatorFollowToW->initialize();

                m_nodeAffectorCreatorInterpolateLinearFloat = Helper::makeFactorableUnique<NodeAffectorCreator::NodeAffectorCreatorInterpolateLinear<float>>( MENGINE_DOCUMENT_FACTORABLE );
                m_nodeAffectorCreatorInterpolateLinearFloat->initialize();

                m_nodeAffectorCreatorInterpolateLinear = Helper::makeFactorableUnique<NodeAffectorCreator::NodeAffectorCreatorInterpolateLinear<mt::vec3f>>( MENGINE_DOCUMENT_FACTORABLE );
                m_nodeAffectorCreatorInterpolateLinear->initialize();

                m_nodeAffectorCreatorInterpolateLinearVec4 = Helper::makeFactorableUnique<NodeAffectorCreator::NodeAffectorCreatorInterpolateLinear<mt::vec4f>>( MENGINE_DOCUMENT_FACTORABLE );
                m_nodeAffectorCreatorInterpolateLinearVec4->initialize();

                m_nodeAffectorCreatorAccumulateLinear = Helper::makeFactorableUnique<NodeAffectorCreator::NodeAffectorCreatorAccumulateLinear<mt::vec3f>>( MENGINE_DOCUMENT_FACTORABLE );
                m_nodeAffectorCreatorAccumulateLinear->initialize();

                m_nodeAffectorCreatorInterpolateQuadraticBezier = Helper::makeFactorableUnique<NodeAffectorCreator::NodeAffectorCreatorInterpolateBezier<mt::vec3f, 1>>( MENGINE_DOCUMENT_FACTORABLE );
                m_nodeAffectorCreatorInterpolateQuadraticBezier->initialize();

                m_nodeAffectorCreatorInterpolateQuadratic = Helper::makeFactorableUnique<NodeAffectorCreator::NodeAffectorCreatorInterpolateQuadratic<mt::vec3f>>( MENGINE_DOCUMENT_FACTORABLE );
                m_nodeAffectorCreatorInterpolateQuadratic->initialize();

                m_nodeAffectorCreatorInterpolateCubicBezier = Helper::makeFactorableUnique<NodeAffectorCreator::NodeAffectorCreatorInterpolateBezier<mt::vec3f, 2>>( MENGINE_DOCUMENT_FACTORABLE );
                m_nodeAffectorCreatorInterpolateCubicBezier->initialize();

                m_nodeAffectorCreatorInterpolateQuarticBezier = Helper::makeFactorableUnique<NodeAffectorCreator::NodeAffectorCreatorInterpolateBezier<mt::vec3f, 3>>( MENGINE_DOCUMENT_FACTORABLE );
                m_nodeAffectorCreatorInterpolateQuarticBezier->initialize();

                m_nodeAffectorCreatorInterpolateQuadraticFloat = Helper::makeFactorableUnique<NodeAffectorCreator::NodeAffectorCreatorInterpolateQuadratic<float>>( MENGINE_DOCUMENT_FACTORABLE );
                m_nodeAffectorCreatorInterpolateQuadraticFloat->initialize();

                m_nodeAffectorCreatorInterpolateLinearColor = Helper::makeFactorableUnique<NodeAffectorCreator::NodeAffectorCreatorInterpolateLinear<Color>>( MENGINE_DOCUMENT_FACTORABLE );
                m_nodeAffectorCreatorInterpolateLinearColor->initialize();

                return true;
            }

            void finalize()
            {
                m_factoryAffectorVelocity2->finalize();
                m_factoryAffectorVelocity2 = nullptr;

                m_nodeAffectorCreatorInterpolateParabolic->finalize();
                m_nodeAffectorCreatorInterpolateParabolic = nullptr;

                m_nodeAffectorCreatorFollowTo->finalize();
                m_nodeAffectorCreatorFollowTo = nullptr;

                m_nodeAffectorCreatorFollowToW->finalize();
                m_nodeAffectorCreatorFollowToW = nullptr;

                m_nodeAffectorCreatorInterpolateLinearFloat->finalize();
                m_nodeAffectorCreatorInterpolateLinearFloat = nullptr;

                m_nodeAffectorCreatorInterpolateLinear->finalize();
                m_nodeAffectorCreatorInterpolateLinear = nullptr;

                m_nodeAffectorCreatorInterpolateLinearVec4->finalize();
                m_nodeAffectorCreatorInterpolateLinearVec4 = nullptr;

                m_nodeAffectorCreatorAccumulateLinear->finalize();
                m_nodeAffectorCreatorAccumulateLinear = nullptr;

                m_nodeAffectorCreatorInterpolateQuadraticBezier->finalize();
                m_nodeAffectorCreatorInterpolateQuadraticBezier = nullptr;

                m_nodeAffectorCreatorInterpolateQuadratic->finalize();
                m_nodeAffectorCreatorInterpolateQuadratic = nullptr;

                m_nodeAffectorCreatorInterpolateCubicBezier->finalize();
                m_nodeAffectorCreatorInterpolateCubicBezier = nullptr;

                m_nodeAffectorCreatorInterpolateQuarticBezier->finalize();
                m_nodeAffectorCreatorInterpolateQuarticBezier = nullptr;

                m_nodeAffectorCreatorInterpolateQuadraticFloat->finalize();
                m_nodeAffectorCreatorInterpolateQuadraticFloat = nullptr;

                m_nodeAffectorCreatorInterpolateLinearColor->finalize();
                m_nodeAffectorCreatorInterpolateLinearColor = nullptr;

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
            FactoryPtr m_factoryPythonScheduleEvent;
            //////////////////////////////////////////////////////////////////////////
            uint32_t ScheduleInterface_schedule( SchedulerInterface * _scheduleManager, float _timing, const pybind::object & _script, const pybind::args & _args )
            {
                PythonScheduleEventPtr sl = m_factoryPythonScheduleEvent->createObject( MENGINE_DOCUMENT_PYBIND );

                sl->initialize( _script, _args );

                uint32_t id = _scheduleManager->event( _timing, sl, MENGINE_DOCUMENT_PYBIND );

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

                uint32_t id = _scheduleManager->timing( pipe, py_timing, py_event, MENGINE_DOCUMENT_PYBIND );

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

                uint32_t id = _scheduleManager->timing( py_pipe, py_timing, py_event, MENGINE_DOCUMENT_PYBIND );

                return id;
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
            UniqueId s_Affectorable_addAffector( Affectorable * _affectorable, const AffectorPtr & _affector )
            {
                const AffectorHubInterfacePtr & affectorHub = _affectorable->getAffectorHub();

                UniqueId id = affectorHub->addAffector( _affector );

                return id;
            }
            //////////////////////////////////////////////////////////////////////////
            bool s_Affectorable_stopAffector( Affectorable * _affectorable, UniqueId _id )
            {
                const AffectorHubInterfacePtr & affectorHub = _affectorable->getAffectorHub();

                bool result = affectorHub->stopAffector( _id );

                return result;
            }
            //////////////////////////////////////////////////////////////////////////
            void s_Affectorable_stopAllAffectors( Affectorable * _affectorable )
            {
                const AffectorHubInterfacePtr & affectorHub = _affectorable->getAffectorHub();

                affectorHub->stopAllAffectors();
            }
            //////////////////////////////////////////////////////////////////////////
            void s_Affectorable_setLinearSpeed( Affectorable * _affectorable, const mt::vec3f & _linearSpeed )
            {
                const AffectorHubInterfacePtr & affectorHub = _affectorable->getAffectorHub();

                affectorHub->setLinearSpeed( _linearSpeed );
            }
            //////////////////////////////////////////////////////////////////////////
            const mt::vec3f & s_Affectorable_getLinearSpeed( Affectorable * _affectorable )
            {
                const AffectorHubInterfacePtr & affectorHub = _affectorable->getAffectorHub();

                const mt::vec3f & linearSpeed = affectorHub->getLinearSpeed();

                return linearSpeed;
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
                _node->removeChildren( []( const NodePtr & _child )
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
            String s_Node_getDebugId( Node * _node )
            {
                Char debugId[256] = {'\0'};
                MENGINE_SNPRINTF( debugId, 255, "%p"
                    , _node 
                );

                return String( debugId );
            }
            //////////////////////////////////////////////////////////////////////////
            NodePtr s_Node_createChildren( Node * _node, const ConstString & _type )
            {
                NodePtr node = PROTOTYPE_SERVICE()
                    ->generatePrototype( STRINGIZE_STRING_LOCAL( "Node" ), _type, MENGINE_DOCUMENT_PYBIND );

                MENGINE_ASSERTION_MEMORY_PANIC( node );

                node->disable();

                _node->addChild( node );

                return node;
            }
            //////////////////////////////////////////////////////////////////////////
            pybind::list s_Node_getAllChildren( pybind::kernel_interface * _kernel, Node * _node )
            {
                uint32_t size = _node->getChildrenCount();

                pybind::list py_children( _kernel, size );

                uint32_t index = 0;

                _node->foreachChildrenSlug( [&py_children, &index]( const NodePtr & _node )
                {
                    py_children[index++] = _node;
                } );

                return py_children;
            }
            //////////////////////////////////////////////////////////////////////////
            void s_Node_moveStop( Node * _node )
            {
                const AffectorHubInterfacePtr & affectorHub = _node->getAffectorHub();

                MENGINE_ASSERTION_MEMORY_PANIC( affectorHub );

                affectorHub->stopAffectors( ETA_POSITION );
            }
            //////////////////////////////////////////////////////////////////////////
            IntrusivePtr<NodeAffectorCreator::NodeAffectorCreatorAccumulateLinear<mt::vec3f>> m_nodeAffectorCreatorAccumulateLinear;
            //////////////////////////////////////////////////////////////////////////
            uint32_t s_Node_velocityTo( Node * _node, float _speed, const mt::vec3f & _dir, const ConstString & _easingType, const pybind::object & _cb, const pybind::args & _args )
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

                AffectorPtr affector = m_nodeAffectorCreatorAccumulateLinear->create( ETA_POSITION
                    , easing
                    , callback
                    , [_node]( const mt::vec3f & _v )
                {
                    _node->setLocalPosition( _v );
                }
                    , _node->getLocalPosition(), _dir, _speed
                    , MENGINE_DOCUMENT_PYBIND
                    );

                MENGINE_ASSERTION_MEMORY_PANIC( affector, "invalid create affector" );

                s_Node_moveStop( _node );

                if( _node->isActivate() == false )
                {
                    return 0;
                }

                mt::vec3f linearSpeed = _dir * _speed;

                const AffectorHubInterfacePtr & affectorHub = _node->getAffectorHub();

                affectorHub->setLinearSpeed( linearSpeed );

                UniqueId id = affectorHub->addAffector( affector );

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

                ~AffectorVelocity2() override
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
                bool _affect( const UpdateContext * _context, float * const _used ) override
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
                : public Factorable
            {
            public:
                FactoryAffectorVelocity2()
                {
                }

                ~FactoryAffectorVelocity2() override
                {
                }

            public:
                bool initialize()
                {
                    m_affectorFactory = Helper::makeFactoryPool<AffectorVelocity2, 4>( MENGINE_DOCUMENT_FACTORABLE );

                    return true;
                }

                void finalize()
                {
                    m_affectorFactory = nullptr;
                }

            public:
                AffectorPtr create( EAffectorType _type
                    , const EasingInterfacePtr & _easing
                    , const AffectorCallbackInterfacePtr & _cb
                    , const NodePtr & _node, const mt::vec3f & _velocity, float _time, const DocumentPtr & _doc )
                {
                    AffectorVelocity2Ptr affector = m_affectorFactory->createObject( _doc );

                    affector->setAffectorType( _type );
                    affector->setEasing( _easing );
                    affector->setCallback( _cb );
                    affector->setNode( _node );

                    affector->initialize( _velocity, _time );

                    return affector;
                }

            protected:
                FactoryPtr m_affectorFactory;
            };
            //////////////////////////////////////////////////////////////////////////
            typedef IntrusivePtr<FactoryAffectorVelocity2> FactoryAffectorVelocity2Ptr;
            //////////////////////////////////////////////////////////////////////////
            FactoryAffectorVelocity2Ptr m_factoryAffectorVelocity2;
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

                AffectorPtr affector = m_factoryAffectorVelocity2->create( ETA_POSITION
                    , easing
                    , callback
                    , NodePtr( _node ), _velocity, _time, MENGINE_DOCUMENT_PYBIND
                    );

                MENGINE_ASSERTION_MEMORY_PANIC( affector, "invalid create affector" );

                s_Node_moveStop( _node );

                if( _node->isActivate() == false )
                {
                    return 0;
                }

                const AffectorHubInterfacePtr & affectorHub = _node->getAffectorHub();

                affectorHub->setLinearSpeed( _velocity );

                UniqueId id = affectorHub->addAffector( affector );

                return id;
            }
            //////////////////////////////////////////////////////////////////////////
            IntrusivePtr<NodeAffectorCreator::NodeAffectorCreatorInterpolateLinear<mt::vec3f>> m_nodeAffectorCreatorInterpolateLinear;
            //////////////////////////////////////////////////////////////////////////
            uint32_t s_Node_moveTo( Node * _node, float _time, const mt::vec3f & _point, const ConstString & _easingType, const pybind::object & _cb, const pybind::args & _args )
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
                    m_nodeAffectorCreatorInterpolateLinear->create( ETA_POSITION
                        , easing
                        , callback
                        , [_node]( const mt::vec3f & _v )
                {
                    _node->setLocalPosition( _v );
                }
                        , _node->getLocalPosition(), _point, _time
                    , MENGINE_DOCUMENT_PYBIND
                    );

                MENGINE_ASSERTION_MEMORY_PANIC( affector, "invalid create affector" );

                s_Node_moveStop( _node );

                if( _node->isActivate() == false )
                {
                    return 0;
                }

                float invTime = 1.f / _time;
                const mt::vec3f & pos = _node->getLocalPosition();
                mt::vec3f linearSpeed = (_point - pos) * invTime;

                const AffectorHubInterfacePtr & affectorHub = _node->getAffectorHub();

                affectorHub->setLinearSpeed( linearSpeed );

                UniqueId id = affectorHub->addAffector( affector );

                return id;
            }
            //////////////////////////////////////////////////////////////////////////
            IntrusivePtr<NodeAffectorCreator::NodeAffectorCreatorInterpolateQuadratic<mt::vec3f>> m_nodeAffectorCreatorInterpolateQuadratic;
            //////////////////////////////////////////////////////////////////////////
            uint32_t s_Node_accMoveTo( Node * _node, float _time, const mt::vec3f & _point, const ConstString & _easingType, const pybind::object & _cb, const pybind::args & _args )
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

                const AffectorHubInterfacePtr & affectorHub = _node->getAffectorHub();

                const mt::vec3f & linearSpeed = affectorHub->getLinearSpeed();

                ScriptableAffectorCallbackPtr callback = createNodeAffectorCallback( _node, _cb, _args );

                AffectorPtr affector = m_nodeAffectorCreatorInterpolateQuadratic->create( ETA_POSITION
                    , easing
                    , callback
                    , [_node]( const mt::vec3f & _v )
                {
                    _node->setLocalPosition( _v );
                }
                    , _node->getLocalPosition(), _point, linearSpeed, _time
                    , MENGINE_DOCUMENT_PYBIND
                    );

                MENGINE_ASSERTION_MEMORY_PANIC( affector, "invalid create affector" );

                s_Node_moveStop( _node );

                if( _node->isActivate() == false )
                {
                    return 0;
                }

                UniqueId id = affectorHub->addAffector( affector );

                return id;
            }
            //////////////////////////////////////////////////////////////////////////
            IntrusivePtr<NodeAffectorCreator::NodeAffectorCreatorInterpolateBezier<mt::vec3f, 1>> m_nodeAffectorCreatorInterpolateQuadraticBezier;
            //////////////////////////////////////////////////////////////////////////
            uint32_t s_Node_bezier2To( Node * _node
                , float _time
                , const mt::vec3f & _to
                , const mt::vec3f & _v0
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

                AffectorPtr affector = m_nodeAffectorCreatorInterpolateQuadraticBezier->create( ETA_POSITION
                    , easing
                    , callback
                    , [_node]( const mt::vec3f & _v )
                {
                    _node->setLocalPosition( _v );
                }
                    , [from]()
                {
                    return from;
                }
                    , [_to]()
                {
                    return _to;
                }
                    , [_v0]( mt::vec3f * _v )
                {
                    _v[0] = _v0;
                }
                    , _time
                    , MENGINE_DOCUMENT_PYBIND
                    );

                MENGINE_ASSERTION_MEMORY_PANIC( affector, "invalid create affector" );

                s_Node_moveStop( _node );

                if( _node->isActivate() == false )
                {
                    return 0;
                }

                const AffectorHubInterfacePtr & affectorHub = _node->getAffectorHub();

                UniqueId id = affectorHub->addAffector( affector );

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
                MENGINE_ASSERTION_MEMORY_PANIC( _follow, "invalid create follower" );

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

                AffectorPtr affector = m_nodeAffectorCreatorInterpolateQuadraticBezier->create( ETA_POSITION
                    , easing
                    , callback
                    , [_node]( const mt::vec3f & _v )
                {
                    _node->setWorldPosition( _v );
                }
                    , [node_pos]()
                {
                    return node_pos;
                }
                    , [_follow, _offset]()
                {
                    return _follow->getWorldPosition() + _offset;
                }
                    , [node_pos, _follow, _offset]( mt::vec3f * _v )
                {
                    float x = _follow->getWorldPosition().x + _offset.x;

                    _v[0] = mt::vec3f( x, node_pos.y, 0.f );
                }
                    , _time
                    , MENGINE_DOCUMENT_PYBIND
                    );

                MENGINE_ASSERTION_MEMORY_PANIC( affector, "invalid create affector" );

                s_Node_moveStop( _node );

                if( _node->isActivate() == false )
                {
                    return 0;
                }

                const AffectorHubInterfacePtr & affectorHub = _node->getAffectorHub();

                UniqueId id = affectorHub->addAffector( affector );

                return id;
            }
            //////////////////////////////////////////////////////////////////////////
            IntrusivePtr<NodeAffectorCreator::NodeAffectorCreatorInterpolateBezier<mt::vec3f, 2>> m_nodeAffectorCreatorInterpolateCubicBezier;
            //////////////////////////////////////////////////////////////////////////
            uint32_t s_Node_bezier3To( Node * _node
                , float _time
                , const mt::vec3f & _to
                , const mt::vec3f & _v0
                , const mt::vec3f & _v1
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
                    m_nodeAffectorCreatorInterpolateCubicBezier->create( ETA_POSITION
                        , easing
                        , callback
                        , [_node]( const mt::vec3f & _v )
                {
                    _node->setLocalPosition( _v );
                }
                        , [_node]()
                {
                    return _node->getLocalPosition();
                }
                    , [_to]()
                {
                    return _to;
                }
                    , [_v0, _v1]( mt::vec3f * _v )
                {
                    _v[0] = _v0; _v[1] = _v1;
                }
                    , _time
                    , MENGINE_DOCUMENT_PYBIND
                    );

                MENGINE_ASSERTION_MEMORY_PANIC( affector, "invalid create affector" );

                s_Node_moveStop( _node );

                if( _node->isActivate() == false )
                {
                    return 0;
                }

                const AffectorHubInterfacePtr & affectorHub = _node->getAffectorHub();

                UniqueId id = affectorHub->addAffector( affector );

                return id;
            }
            //////////////////////////////////////////////////////////////////////////
            IntrusivePtr<NodeAffectorCreator::NodeAffectorCreatorInterpolateBezier<mt::vec3f, 3>> m_nodeAffectorCreatorInterpolateQuarticBezier;
            //////////////////////////////////////////////////////////////////////////
            uint32_t s_Node_bezier4To( Node * _node
                , float _time
                , const mt::vec3f & _to
                , const mt::vec3f & _v0
                , const mt::vec3f & _v1
                , const mt::vec3f & _v2
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
                    m_nodeAffectorCreatorInterpolateQuarticBezier->create( ETA_POSITION
                        , easing
                        , callback
                        , [_node]( const mt::vec3f & _v )
                {
                    _node->setLocalPosition( _v );
                }
                        , [_node]()
                {
                    return _node->getLocalPosition();
                }
                    , [_to]()
                {
                    return _to;
                }
                    , [_v0, _v1, _v2]( mt::vec3f * _v )
                {
                    _v[0] = _v0; _v[1] = _v1; _v[2] = _v2;
                }
                    , _time
                    , MENGINE_DOCUMENT_PYBIND
                    );

                MENGINE_ASSERTION_MEMORY_PANIC( affector, "invalid create affector" );

                s_Node_moveStop( _node );

                if( _node->isActivate() == false )
                {
                    return 0;
                }

                const AffectorHubInterfacePtr & affectorHub = _node->getAffectorHub();

                UniqueId id = affectorHub->addAffector( affector );

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

                ~AffectorCreatorInterpolateParabolic() override
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
                bool _affect( const UpdateContext * _context, float * const _used ) override
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
                : public Factorable
            {
            public:
                FactoryAffectorInterpolateParabolic()
                {
                }

                ~FactoryAffectorInterpolateParabolic() override
                {
                }

            public:
                bool initialize()
                {
                    m_affectorFactory = Helper::makeFactoryPool<AffectorCreatorInterpolateParabolic, 4>( MENGINE_DOCUMENT_FACTORABLE );

                    return true;
                }

                void finalize()
                {
                    m_affectorFactory = nullptr;
                }

            public:
                AffectorPtr create( EAffectorType _type
                    , const EasingInterfacePtr & _easing
                    , const AffectorCallbackInterfacePtr & _cb
                    , const NodePtr & _node, const mt::vec3f & _end, const mt::vec3f & _v0, float _time, const DocumentPtr & _doc )
                {
                    AffectorCreatorInterpolateParabolicPtr affector = m_affectorFactory->createObject( _doc );

                    affector->setAffectorType( _type );
                    affector->setEasing( _easing );
                    affector->setCallback( _cb );
                    affector->setNode( _node );

                    affector->initialize( _end, _v0, _time );

                    return affector;
                }

            protected:
                FactoryPtr m_affectorFactory;
            };
            //////////////////////////////////////////////////////////////////////////
            typedef IntrusivePtr<FactoryAffectorInterpolateParabolic> FactoryAffectorInterpolateParabolicPtr;
            //////////////////////////////////////////////////////////////////////////
            FactoryAffectorInterpolateParabolicPtr m_nodeAffectorCreatorInterpolateParabolic;
            //////////////////////////////////////////////////////////////////////////
            uint32_t s_Node_parabolaTo( Node * _node
                , float _time
                , const mt::vec3f & _end
                , const mt::vec3f & _v0
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

                AffectorPtr affector = m_nodeAffectorCreatorInterpolateParabolic->create( ETA_POSITION
                    , easing
                    , callback
                    , NodePtr( _node ), _end, _v0, _time
                    , MENGINE_DOCUMENT_PYBIND
                    );

                MENGINE_ASSERTION_MEMORY_PANIC( affector, "invalid create affector" );

                s_Node_moveStop( _node );

                if( _node->isActivate() == false )
                {
                    return 0;
                }

                const AffectorHubInterfacePtr & affectorHub = _node->getAffectorHub();

                UniqueId id = affectorHub->addAffector( affector );

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

                ~AffectorCreatorFollowTo() override
                {
                }

            public:
                bool initialize( const NodePtr & _node, const NodePtr & _target, const mt::vec3f & _offset, float _distance, float _moveSpeed, float _moveAcceleration, float _moveLimit, bool _rotate, float _rotationSpeed, float _rotationAcceleration, float _rotationLimit )
                {
                    MENGINE_ASSERTION_MEMORY_PANIC( _node, "invalid create followTo" );
                    MENGINE_ASSERTION_MEMORY_PANIC( _target, "invalid create followTo" );

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
                bool _affect( const UpdateContext * _context, float * const _used ) override
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
                : public Factorable
            {
            public:
                FactoryAffectorFollowTo()
                {
                }

                ~FactoryAffectorFollowTo() override
                {
                }

            public:
                bool initialize()
                {
                    m_affectorFactory = Helper::makeFactoryPool<AffectorCreatorFollowTo, 4>( MENGINE_DOCUMENT_FACTORABLE );

                    return true;
                }

                void finalize()
                {
                    m_affectorFactory = nullptr;
                }

            public:
                AffectorPtr create( EAffectorType _type
                    , const EasingInterfacePtr & _easing
                    , const AffectorCallbackInterfacePtr & _cb
                    , const NodePtr & _node, const NodePtr & _target, const mt::vec3f & _offset, float _distance, float _moveSpeed, float _moveAcceleration, float _moveLimit
                    , bool _rotate
                    , float _rotationSpeed, float _rotationAcceleration, float _rotationLimit, const DocumentPtr & _doc )
                {
                    AffectorCreatorFollowToPtr affector = m_affectorFactory->createObject( _doc );

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
                FactoryPtr m_affectorFactory;
            };
            //////////////////////////////////////////////////////////////////////////
            typedef IntrusivePtr<FactoryAffectorFollowTo> FactoryAffectorFollowToPtr;
            //////////////////////////////////////////////////////////////////////////
            FactoryAffectorFollowToPtr m_nodeAffectorCreatorFollowTo;
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

                AffectorPtr affector = m_nodeAffectorCreatorFollowTo->create( ETA_POSITION
                    , easing
                    , callback
                    , NodePtr( _node ), _target, _offset, _distance
                    , _moveSpeed, _moveAcceleration, _moveLimit
                    , _rotate
                    , _rotationSpeed, _rotationAcceleration, _rotationLimit
                    , MENGINE_DOCUMENT_PYBIND
                    );

                MENGINE_ASSERTION_MEMORY_PANIC( affector );

                s_Node_moveStop( _node );

                if( _node->isActivate() == false )
                {
                    return 0;
                }

                const AffectorHubInterfacePtr & affectorHub = _node->getAffectorHub();

                UniqueId id = affectorHub->addAffector( affector );

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

                ~AffectorCreatorFollowToW() override
                {
                }

            public:
                bool initialize( const NodePtr & _node, const NodePtr & _target, const mt::vec3f & _offset, float _distance, float _moveSpeed, float _moveAcceleration, float _moveLimit )
                {
                    MENGINE_ASSERTION_MEMORY_PANIC( _node, "invalid create followTo" );
                    MENGINE_ASSERTION_MEMORY_PANIC( _target, "invalid create followTo" );

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
                bool _affect( const UpdateContext * _context, float * const _used ) override
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
                : public Factorable
            {
            public:
                FactoryAffectorFollowToW()
                {
                }

                ~FactoryAffectorFollowToW() override
                {
                }

            public:
                bool initialize()
                {
                    m_affectorFactory = Helper::makeFactoryPool<AffectorCreatorFollowToW, 4>( MENGINE_DOCUMENT_FACTORABLE );

                    return true;
                }

                void finalize()
                {
                    MENGINE_ASSERTION_FACTORY_EMPTY( m_affectorFactory );
                    m_affectorFactory = nullptr;
                }

            public:
                AffectorPtr create( EAffectorType _type
                    , const EasingInterfacePtr & _easing
                    , const AffectorCallbackInterfacePtr & _cb
                    , const NodePtr & _node, const NodePtr & _target, const mt::vec3f & _offset
                    , float _distance, float _moveSpeed, float _moveAcceleration, float _moveLimit, const DocumentPtr & _doc )
                {
                    AffectorCreatorFollowToWPtr affector = m_affectorFactory->createObject( _doc );

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
                FactoryPtr m_affectorFactory;
            };
            //////////////////////////////////////////////////////////////////////////
            typedef IntrusivePtr<FactoryAffectorFollowToW> FactoryAffectorFollowToWPtr;
            //////////////////////////////////////////////////////////////////////////
            FactoryAffectorFollowToWPtr m_nodeAffectorCreatorFollowToW;
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

                AffectorPtr affector = m_nodeAffectorCreatorFollowToW->create( ETA_POSITION
                    , easing
                    , callback
                    , NodePtr( _node ), _target, _offset, _distance
                    , _moveSpeed, _moveAcceleration, _moveLimit
                    , MENGINE_DOCUMENT_PYBIND
                    );

                MENGINE_ASSERTION_MEMORY_PANIC( affector );

                s_Node_moveStop( _node );

                if( _node->isActivate() == false )
                {
                    return 0;
                }

                const AffectorHubInterfacePtr & affectorHub = _node->getAffectorHub();

                UniqueId id = affectorHub->addAffector( affector );

                return id;
            }
            //////////////////////////////////////////////////////////////////////////
            void s_Node_angleStop( Node * _node )
            {
                const AffectorHubInterfacePtr & affectorHub = _node->getAffectorHub();

                MENGINE_ASSERTION_MEMORY_PANIC( affectorHub );

                affectorHub->stopAffectors( ETA_ANGLE );                
            }
            //////////////////////////////////////////////////////////////////////////
            IntrusivePtr<NodeAffectorCreator::NodeAffectorCreatorInterpolateLinear<float>> m_nodeAffectorCreatorInterpolateLinearFloat;
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

                AffectorPtr affector = m_nodeAffectorCreatorInterpolateLinearFloat->create( ETA_ANGLE
                    , easing
                    , callback
                    , [_node]( float _v )
                {
                    _node->setLocalOrientationX( _v );
                }
                    , correct_angle_from, correct_angle_to, _time
                    , MENGINE_DOCUMENT_PYBIND
                    );

                MENGINE_ASSERTION_MEMORY_PANIC( affector, "invalid create affector" );

                s_Node_angleStop( _node );

                if( _node->isActivate() == false )
                {
                    return 0;
                }

                float invTime = 1.f / _time;
                float angularSpeed = MT_fabsf( correct_angle_from - correct_angle_to ) * invTime;

                const AffectorHubInterfacePtr & affectorHub = _node->getAffectorHub();

                affectorHub->setAngularSpeed( angularSpeed );
                UniqueId id = affectorHub->addAffector( affector );

                return id;
            }
            //////////////////////////////////////////////////////////////////////////
            IntrusivePtr<NodeAffectorCreator::NodeAffectorCreatorInterpolateQuadratic<float>> m_nodeAffectorCreatorInterpolateQuadraticFloat;
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

                const AffectorHubInterfacePtr & affectorHub = _node->getAffectorHub();
                float angularSpeed = affectorHub->getAngularSpeed();

                float angle = _node->getLocalOrientationX();

                float correct_angle_from = angle;
                float correct_angle_to = _angle;
                //mt::angle_correct_interpolate_from_to( angle, _angle, correct_angle_from, correct_angle_to );

                EasingInterfacePtr easing = VOCABULARY_GET( STRINGIZE_STRING_LOCAL( "Easing" ), _easingType );

                ScriptableAffectorCallbackPtr callback = createNodeAffectorCallback( _node, _cb, _args );

                AffectorPtr affector =
                    m_nodeAffectorCreatorInterpolateQuadraticFloat->create( ETA_ANGLE
                        , easing
                        , callback
                        , [_node]( float _v )
                {
                    _node->setLocalOrientationX( _v );
                }
                        , correct_angle_from, correct_angle_to, angularSpeed, _time
                    , MENGINE_DOCUMENT_PYBIND
                    );

                MENGINE_ASSERTION_MEMORY_PANIC( affector, "invalid create affector" );

                s_Node_angleStop( _node );

                if( _node->isActivate() == false )
                {
                    return 0;
                }

                UniqueId id = affectorHub->addAffector( affector );

                return id;
            }
            //////////////////////////////////////////////////////////////////////////
            void s_Node_scaleStop( Node * _node )
            {
                const AffectorHubInterfacePtr & affectorHub = _node->getAffectorHub();

                MENGINE_ASSERTION_MEMORY_PANIC( affectorHub );

                affectorHub->stopAffectors( ETA_SCALE );
            }
            //////////////////////////////////////////////////////////////////////////
            uint32_t s_Node_scaleTo( Node * _node, float _time, const mt::vec3f & _scale, const ConstString & _easingType, const pybind::object & _cb, const pybind::args & _args )
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

                AffectorPtr affector = m_nodeAffectorCreatorInterpolateLinear->create( ETA_SCALE
                    , easing
                    , callback
                    , [_node]( const mt::vec3f & _v )
                {
                    _node->setLocalScale( _v );
                }
                    , _node->getLocalScale(), _scale, _time
                    , MENGINE_DOCUMENT_PYBIND
                    );

                MENGINE_ASSERTION_MEMORY_PANIC( affector, "invalid create affector" );

                s_Node_scaleStop( _node );

                if( _node->isActivate() == false )
                {
                    return 0;
                }

                const AffectorHubInterfacePtr & affectorHub = _node->getAffectorHub();

                UniqueId id = affectorHub->addAffector( affector );

                return id;
            }
            //////////////////////////////////////////////////////////////////////////
            void s_Node_colorStop( Node * _node )
            {
                const AffectorHubInterfacePtr & affectorHub = _node->getAffectorHub();

                MENGINE_ASSERTION_MEMORY_PANIC( affectorHub );

                affectorHub->stopAffectors( ETA_COLOR );
            }
            //////////////////////////////////////////////////////////////////////////
            IntrusivePtr<NodeAffectorCreator::NodeAffectorCreatorInterpolateLinear<Color>> m_nodeAffectorCreatorInterpolateLinearColor;
            //////////////////////////////////////////////////////////////////////////
            uint32_t s_Node_colorTo( Node * _node, float _time, const Color & _color, const ConstString & _easingType, const pybind::object & _cb, const pybind::args & _args )
            {
                MENGINE_ASSERTION_MEMORY_PANIC( _node, "_node is None" );

                if( _node->isActivate() == false )
                {
                    LOGGER_ERROR( "node '%s' is not activate"
                        , _node->getName().c_str()
                        );

                    return 0;
                }

                if( _node->isAfterActive() == false )
                {
                    LOGGER_ERROR( "node '%s' is not after activate"
                        , _node->getName().c_str()
                        );

                    return 0;
                }

                RenderInterface * render = _node->getRender();

                MENGINE_ASSERTION_MEMORY_PANIC( render, "node '%s' is not renderable"
                    , _node->getName().c_str()
                    );

                EasingInterfacePtr easing = VOCABULARY_GET( STRINGIZE_STRING_LOCAL( "Easing" ), _easingType );

                MENGINE_ASSERTION_MEMORY_PANIC( easing, "node '%s' not found easing '%s'"
                    , _node->getName().c_str()
                    , _easingType.c_str()
                    );

                ScriptableAffectorCallbackPtr callback = createNodeAffectorCallback( _node, _cb, _args );

                AffectorPtr affector =
                    m_nodeAffectorCreatorInterpolateLinearColor->create( ETA_COLOR
                        , easing
                        , callback
                        , [render]( const Color & _v )
                {
                    render->setLocalColor( _v );
                }
                        , render->getLocalColor(), _color, _time
                    , MENGINE_DOCUMENT_PYBIND
                    );

                MENGINE_ASSERTION_MEMORY_PANIC( affector, "node '%s' invalid create affector"
                    , _node->getName().c_str()
                    );

                s_Node_colorStop( _node );

                if( _node->isActivate() == false )
                {
                    LOGGER_ERROR( "node '%s' after color stop is inactivate"
                        , _node->getName().c_str()
                        );

                    return 0;
                }

                const AffectorHubInterfacePtr & affectorHub = _node->getAffectorHub();

                UniqueId id = affectorHub->addAffector( affector );

                if( id == 0 )
                {
                    LOGGER_ERROR( "node '%s' invalid add affector"
                        , _node->getName().c_str()
                        );

                    return 0;
                }

                return id;
            }
            //////////////////////////////////////////////////////////////////////////
            uint32_t s_Node_alphaTo( Node * _node, float _time, float _alpha, const ConstString & _easingType, const pybind::object & _cb, const pybind::args & _args )
            {
                MENGINE_ASSERTION_MEMORY_PANIC( _node, "_node is None" );

                if( _node->isActivate() == false )
                {
                    LOGGER_ERROR( "node '%s' is not activate"
                        , _node->getName().c_str()
                        );

                    return 0;
                }

                if( _node->isAfterActive() == false )
                {
                    LOGGER_ERROR( "node '%s' is not after activate"
                        , _node->getName().c_str()
                        );

                    return 0;
                }

                RenderInterface * render = _node->getRender();

                MENGINE_ASSERTION_MEMORY_PANIC( render, "node '%s' type '%s' is not renderable"
                    , _node->getName().c_str()
                    , _node->getType().c_str()
                    );

                EasingInterfacePtr easing = VOCABULARY_GET( STRINGIZE_STRING_LOCAL( "Easing" ), _easingType );

                MENGINE_ASSERTION_MEMORY_PANIC( easing, "node '%s' type '%s' not found easing '%s'"
                    , _node->getName().c_str()
                    , _node->getType().c_str()
                    , _easingType.c_str()
                    );

                ScriptableAffectorCallbackPtr callback = createNodeAffectorCallback( _node, _cb, _args );

                AffectorPtr affector =
                    m_nodeAffectorCreatorInterpolateLinearFloat->create( ETA_COLOR
                        , easing
                        , callback
                        , [render]( float _v )
                {
                    render->setLocalAlpha( _v );
                }
                        , render->getLocalAlpha(), _alpha, _time
                    , MENGINE_DOCUMENT_PYBIND
                    );

                MENGINE_ASSERTION_MEMORY_PANIC( affector, "node '%s' invalid create affector"
                    , _node->getName().c_str()
                    );

                s_Node_colorStop( _node );

                if( _node->isActivate() == false )
                {
                    LOGGER_ERROR( "node '%s' after color stop is inactivate"
                        , _node->getName().c_str()
                        );

                    return 0;
                }

                const AffectorHubInterfacePtr & affectorHub = _node->getAffectorHub();

                UniqueId id = affectorHub->addAffector( affector );

                if( id == 0 )
                {
                    LOGGER_ERROR( "node '%s' invalid add affector"
                        , _node->getName().c_str()
                        );

                    return 0;
                }

                return id;
            }
        };
        //////////////////////////////////////////////////////////////////////////
        typedef IntrusivePtr<KernelScriptMethod> KernelScriptMethodPtr;
        //////////////////////////////////////////////////////////////////////////
    }
    KernelScriptEmbedding::KernelScriptEmbedding()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    KernelScriptEmbedding::~KernelScriptEmbedding()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool KernelScriptEmbedding::embedding( pybind::kernel_interface * _kernel )
    {
        KernelScriptMethodPtr scriptMethod = Helper::makeFactorableUnique<KernelScriptMethod>( MENGINE_DOCUMENT_FACTORABLE );

        if( scriptMethod->initialize() == false )
        {
            return false;
        }

        pybind::interface_<Mixin>( _kernel, "Mixin", true )
            .def_smart_pointer()
            ;

        pybind::interface_<Scriptable, pybind::bases<Mixin>>( _kernel, "Scriptable" )
            .def_bindable()
            ;

        pybind::interface_<Identity, pybind::bases<Mixin>>( _kernel, "Identity" )
            .def( "setName", &Identity::setName )
            .def( "getName", &Identity::getName )
            .def( "getType", &Identity::getType )
            ;

        pybind::interface_<BoundingBox, pybind::bases<Mixin>>( _kernel, "BoundingBox" )
            .def_proxy_static( "getBoundingBox", scriptMethod, &KernelScriptMethod::s_BoundingBox_getBoundingBox )
            ;

        pybind::interface_<Transformation, pybind::bases<Mixin>>( _kernel, "Transformation" )
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
            .def_proxy_static( "setAngleDeg", scriptMethod, &KernelScriptMethod::s_Transformation_setAngleDeg )

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
            .def_proxy_static( "removeRelationTransformation", scriptMethod, &KernelScriptMethod::s_Transformation_removeRelationTransformation )
            ;

        pybind::interface_<Compilable, pybind::bases<Mixin>>( _kernel, "Compilable" )
            .def( "compile", &Compilable::compile )
            .def( "release", &Compilable::release )
            .def( "isCompile", &Compilable::isCompile )
            ;

        pybind::interface_<Reference, pybind::bases<Mixin>>( _kernel, "Reference" )
            .def( "incrementReference", &Reference::incrementReference )
            .def( "decrementReference", &Reference::decrementReference )
            .def( "countReference", &Reference::countReference )
            ;

        pybind::interface_<FileGroupInterface, pybind::bases<Mixin>>( _kernel, "FileGroupInterface" )
            .def( "getName", &FileGroupInterface::getName )
            .def_deprecated( "getCategory", &FileGroupInterface::getBaseFileGroup, "use getBaseFileGroup" )
            .def( "getBaseFileGroup", &FileGroupInterface::getBaseFileGroup )
            .def( "getParentFileGroup", &FileGroupInterface::getParentFileGroup )
            .def( "isPacked", &FileGroupInterface::isPacked )
            .def( "getRelationPath", &FileGroupInterface::getRelationPath )
            .def( "existFile", &FileGroupInterface::existFile )
            .def( "existDirectory", &FileGroupInterface::existDirectory )
            ;

        pybind::interface_<ContentInterface, pybind::bases<Mixin>>( _kernel, "ContentInterface" )
            .def( "setFileGroup", &ContentInterface::setFileGroup )
            .def( "getFileGroup", &ContentInterface::getFileGroup )
            .def( "setFilePath", &ContentInterface::setFilePath )
            .def( "getFilePath", &ContentInterface::getFilePath )
            .def( "setCodecType", &ContentInterface::setCodecType )
            .def( "getCodecType", &ContentInterface::getCodecType )
            .def( "setConverterType", &ContentInterface::setConverterType )
            .def( "getConverterType", &ContentInterface::getConverterType )
            .def( "setValidNoExist", &ContentInterface::setValidNoExist )
            .def( "isValidNoExist", &ContentInterface::isValidNoExist )
            ;

        pybind::interface_<Contentable, pybind::bases<Mixin>>( _kernel, "Contentable" )
            .def_mutable( "getContent", &Contentable::getContent )
            ;

        pybind::interface_<Resource, pybind::bases<Contentable, Scriptable, Compilable, Identity>>( _kernel, "Resource", false )
            .def( "setLocale", &Resource::setLocale )
            .def( "getLocale", &Resource::getLocale )
            .def( "setGroupName", &Resource::setGroupName )
            .def( "getGroupName", &Resource::getGroupName )
            .def( "cache", &Resource::cache )
            .def( "uncache", &Resource::uncache )
            ;

        pybind::interface_<UpdationInterface, pybind::bases<Mixin>>( _kernel, "Updation" )
            ;

        pybind::interface_<Updatable, pybind::bases<Mixin>>( _kernel, "Updatable" )
            .def_mutable( "getUpdation", &Updatable::getUpdation )
            ;

        pybind::interface_<Colorable, pybind::bases<Mixin>>( _kernel, "Colorable" )
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

        pybind::interface_<RenderInterface, pybind::bases<Colorable, BoundingBox>>( _kernel, "RenderInterface" )
            .def_deprecated( "setRelationRender", &RenderInterface::setRelationRender, "use setRelationRenderBack" )
            .def( "setRelationRenderBack", &RenderInterface::setRelationRender )
            .def( "removeRelationRender", &RenderInterface::removeRelationRender )
            .def( "getRelationRender", &RenderInterface::getRelationRender )
            .def( "setExtraRelationRenderBack", &RenderInterface::setExtraRelationRender )
            .def( "removeExtraRelationRender", &RenderInterface::removeExtraRelationRender )
            .def( "getExtraRelationRender", &RenderInterface::getExtraRelationRender )
            .def( "getTotalRelationRender", &RenderInterface::getTotalRelationRender )
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

        pybind::interface_<Renderable, pybind::bases<Mixin>>( _kernel, "Renderable" )
            .def_mutable( "getRender", &Renderable::getRender )
            ;

        pybind::interface_<PickerInterface, pybind::bases<Mixin>>( _kernel, "PickerInterface" )
            .def( "isPickerEnable", &PickerInterface::isPickerEnable )
            .def( "isPickerPicked", &PickerInterface::isPickerPicked )
            .def( "isPickerPressed", &PickerInterface::isPickerPressed )
            .def( "isPickerHandle", &PickerInterface::isPickerHandle )
            .def( "isPickerExclusive", &PickerInterface::isPickerExclusive )
            .def( "pick", &PickerInterface::pick )
            ;

        pybind::interface_<Pickerable, pybind::bases<Mixin>>( _kernel, "Pickerable" )
            .def_const( "getPicker", &Pickerable::getPicker )
            ;

        pybind::interface_<AnimationInterface, pybind::bases<Mixin>>( _kernel, "Animation" )
            .def_proxy_static( "play", scriptMethod, &KernelScriptMethod::s_Animation_play )
            .def( "stop", &AnimationInterface::stop )
            .def( "pause", &AnimationInterface::pause )
            .def_proxy_static( "resume", scriptMethod, &KernelScriptMethod::s_Animation_resume )
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

        pybind::interface_<Animatable, pybind::bases<Mixin>>( _kernel, "Animatable" )
            .def_mutable( "getAnimation", &Animatable::getAnimation )
            ;

        pybind::interface_<EventationInterface, pybind::bases<Mixin>>( _kernel, "Eventation" )
            .def( "removeEvents", &EventationInterface::removeEvents )
            ;

        pybind::interface_<Eventable, pybind::bases<Mixin>>( _kernel, "Eventable" )
            .def_mutable( "getEventation", &Eventable::getEventation )
            ;

        pybind::interface_<Soundable, pybind::bases<Mixin>>( _kernel, "Soundable" )
            .def( "setVolume", &Soundable::setVolume )
            .def( "getVolume", &Soundable::getVolume )
            ;

        pybind::interface_<Affectorable, pybind::bases<Mixin>>( _kernel, "Affectorable" )
            .def_proxy_static( "addAffector", scriptMethod, &KernelScriptMethod::s_Affectorable_addAffector )
            .def_proxy_static( "stopAffector", scriptMethod, &KernelScriptMethod::s_Affectorable_stopAffector )
            .def_proxy_static( "stopAllAffectors", scriptMethod, &KernelScriptMethod::s_Affectorable_stopAllAffectors )
            .def_proxy_static( "setLinearSpeed", scriptMethod, &KernelScriptMethod::s_Affectorable_setLinearSpeed )
            .def_proxy_static( "getLinearSpeed", scriptMethod, &KernelScriptMethod::s_Affectorable_getLinearSpeed )
            ;

        pybind::interface_<Materialable, pybind::bases<Mixin>>( _kernel, "Materialable" )
            .def( "setMaterialName", &Materialable::setMaterialName )
            .def( "getMaterialName", &Materialable::getMaterialName )
            .def( "setDisableTextureColor", &Materialable::setDisableTextureColor )
            .def( "getDisableTextureColor", &Materialable::getDisableTextureColor )
            .def( "setBlendMode", &Materialable::setBlendMode )
            .def( "getBlendMode", &Materialable::getBlendMode )
            ;

        pybind::interface_<Hierarchy, pybind::bases<Mixin>>( _kernel, "Hierarchy" )
            .def( "addChildren", &Hierarchy::addChild )
            .def( "addChildrenFront", &Hierarchy::addChildFront )
            .def( "addChildrenAfter", &Hierarchy::addChildAfter )
            .def( "findChildren", &Hierarchy::findChild )
            .def( "getSiblingPrev", &Hierarchy::getSiblingPrev )
            .def( "getSiblingNext", &Hierarchy::getSiblingNext )
            .def( "emptyChild", &Hierarchy::emptyChildren )
            .def( "hasChildren", &Hierarchy::hasChild )
            .def( "getParent", &Hierarchy::getParent )
            .def( "hasParent", &Hierarchy::hasParent )
            ;

        pybind::interface_<Node, pybind::bases<Scriptable, Eventable, Animatable, Identity, Transformation, Compilable, Renderable, Pickerable, Affectorable, Hierarchy>>( _kernel, "Node", false )
            .def( "enable", &Node::enable )
            .def( "disable", &Node::disable )
            .def( "isEnable", &Node::isEnable )
            .def( "isActivate", &Node::isActivate )
            .def( "freeze", &Node::freeze )
            .def( "isFreeze", &Node::isFreeze )
            .def( "getUniqueIdentity", &Node::getUniqueIdentity )
            .def( "findUniqueChild", &Node::findUniqueChild )
            .def_proxy_static( "removeChildren", scriptMethod, &KernelScriptMethod::s_Node_removeChild )
            .def_proxy_static( "removeAllChild", scriptMethod, &KernelScriptMethod::s_Node_removeAllChild )
            .def_proxy_static( "removeFromParent", scriptMethod, &KernelScriptMethod::s_Node_removeFromParent )
            .def_proxy_static( "destroyAllChild", scriptMethod, &KernelScriptMethod::s_Node_destroyAllChild )
            .def_proxy_static( "isHomeless", scriptMethod, &KernelScriptMethod::s_Node_isHomeless )

            .def( "getWorldPosition", &Node::getWorldPosition )
            .def( "setWorldPosition", &Node::setWorldPosition )
            //.def( "getWorldDirection", &Node::getWorldDirection )
            .def_proxy_static( "getWorldOffsetPosition", scriptMethod, &KernelScriptMethod::s_Node_getWorldOffsetPosition )
            .def_proxy_static( "getLengthTo", scriptMethod, &KernelScriptMethod::s_Node_getLengthTo )

            .def_proxy_static( "getDebugId", scriptMethod, &KernelScriptMethod::s_Node_getDebugId )

            .def_proxy_static( "createChildren", scriptMethod, &KernelScriptMethod::s_Node_createChildren )
            .def_proxy_static_kernel( "getAllChildren", scriptMethod, &KernelScriptMethod::s_Node_getAllChildren )

            .def_proxy_static_args( "colorTo", scriptMethod, &KernelScriptMethod::s_Node_colorTo )
            .def_proxy_static_args( "alphaTo", scriptMethod, &KernelScriptMethod::s_Node_alphaTo )
            .def_proxy_static( "colorStop", scriptMethod, &KernelScriptMethod::s_Node_colorStop )

            .def_proxy_static_args( "velocityTo", scriptMethod, &KernelScriptMethod::s_Node_velocityTo )
            .def_proxy_static_args( "velocityTo2", scriptMethod, &KernelScriptMethod::s_Node_velocityTo2 )

            .def_proxy_static_args( "moveTo", scriptMethod, &KernelScriptMethod::s_Node_moveTo )
            .def_proxy_static_args( "bezier2To", scriptMethod, &KernelScriptMethod::s_Node_bezier2To )
            .def_proxy_static_args( "bezier3To", scriptMethod, &KernelScriptMethod::s_Node_bezier3To )
            .def_proxy_static_args( "bezier4To", scriptMethod, &KernelScriptMethod::s_Node_bezier4To )
            .def_proxy_static_args( "parabolaTo", scriptMethod, &KernelScriptMethod::s_Node_parabolaTo )
            .def_proxy_static_args( "followTo", scriptMethod, &KernelScriptMethod::s_Node_followTo )
            .def_proxy_static_args( "followToW", scriptMethod, &KernelScriptMethod::s_Node_followToW )
            .def_proxy_static_args( "bezier2Follower", scriptMethod, &KernelScriptMethod::s_Node_bezier2Follower )
            .def_proxy_static( "moveStop", scriptMethod, &KernelScriptMethod::s_Node_moveStop )

            .def_proxy_static_args( "angleTo", scriptMethod, &KernelScriptMethod::s_Node_angleTo )
            .def_proxy_static( "angleStop", scriptMethod, &KernelScriptMethod::s_Node_angleStop )
            .def_proxy_static_args( "scaleTo", scriptMethod, &KernelScriptMethod::s_Node_scaleTo )
            .def_proxy_static( "scaleStop", scriptMethod, &KernelScriptMethod::s_Node_scaleStop )

            .def_proxy_static_args( "accMoveTo", scriptMethod, &KernelScriptMethod::s_Node_accMoveTo )
            .def_proxy_static_args( "accAngleTo", scriptMethod, &KernelScriptMethod::s_Node_accAngleTo )
            ;

        pybind::interface_<Affector, pybind::bases<Updatable>>( _kernel, "Affector", true )
            .def( "stop", &Affector::stop )
            .def( "getId", &Affector::getId )
            .def( "setFreeze", &Affector::setFreeze )
            .def( "getFreeze", &Affector::getFreeze )
            .def( "setSpeedFactor", &Affector::setSpeedFactor )
            .def( "getSpeedFactor", &Affector::getSpeedFactor )
            ;

        pybind::interface_<Surface, pybind::bases<Scriptable, Eventable, Animatable, Identity, Materialable, Compilable>>( _kernel, "Surface", false )
            .def( "setAnchor", &Surface::setAnchor )
            .def( "getAnchor", &Surface::getAnchor )
            .def( "getMaxSize", &Surface::getMaxSize )
            .def( "getSize", &Surface::getSize )
            .def( "getOffset", &Surface::getOffset )
            .def( "getUVCount", &Surface::getUVCount )
            .def( "getUV", &Surface::getUV )
            .def( "getColor", &Surface::getColor )
            .def_deprecated( "getColour", &Surface::getColor, "use getColor" )
            ;

        pybind::interface_<Visitable, pybind::bases<Mixin>>( _kernel, "Visitable" )
            ;

        pybind::interface_<ThreadTask, pybind::bases<Visitable>>( _kernel, "Task" )
            ;

        pybind::interface_<RenderViewportInterface, pybind::bases<Mixin>>( _kernel, "RenderViewportInterface" )
            .def( "getViewport", &RenderViewportInterface::getViewport )
            ;

        pybind::interface_<RenderScissorInterface, pybind::bases<Mixin>>( _kernel, "RenderScissorInterface" )
            .def( "getScissorViewport", &RenderScissorInterface::getScissorViewport )
            ;

        pybind::interface_<RenderCameraInterface, pybind::bases<Mixin>>( _kernel, "RenderCameraInterface" )
            ;

        pybind::interface_<RenderScissorInterface, pybind::bases<Mixin>>( _kernel, "RenderScissorInterface", false )
            .def( "getScissorRect", &RenderScissorInterface::getScissorViewport )
            ;

        pybind::interface_<RenderTargetInterface, pybind::bases<Mixin>>( _kernel, "RenderTargetInterface", false )
            .def_deprecated( "getWidth", &RenderTargetInterface::getHWWidth, "use getHWWidth" )
            .def_deprecated( "getHeight", &RenderTargetInterface::getHWHeight, "use getHWHeight" )
            .def( "getHWWidth", &RenderTargetInterface::getHWWidth )
            .def( "getHWHeight", &RenderTargetInterface::getHWHeight )
            ;

        pybind::interface_<SchedulerInterface, pybind::bases<Mixin>>( _kernel, "SchedulerInterface", true )
            .def_proxy_static_args( "timing", scriptMethod, &KernelScriptMethod::ScheduleInterface_timing )
            .def_proxy_static_args( "schedule", scriptMethod, &KernelScriptMethod::ScheduleInterface_schedule )
            .def_proxy_static_args( "pipe", scriptMethod, &KernelScriptMethod::ScheduleInterface_pipe )
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

        m_implement = scriptMethod;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void KernelScriptEmbedding::ejecting( pybind::kernel_interface * _kernel )
    {
        MENGINE_UNUSED( _kernel );

        KernelScriptMethodPtr scriptMethod = m_implement;
        scriptMethod->finalize();

        m_implement = nullptr;
    }
}
