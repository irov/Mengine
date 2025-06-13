#include "EngineScriptEmbedding.h"

#include "Interface/PlatformServiceInterface.h"
#include "Interface/EnvironmentServiceInterface.h"
#include "Interface/ApplicationInterface.h"
#include "Interface/FileServiceInterface.h"
#include "Interface/TimelineServiceInterface.h"
#include "Interface/StringizeServiceInterface.h"
#include "Interface/ThreadServiceInterface.h"
#include "Interface/InputServiceInterface.h"
#include "Interface/MemoryServiceInterface.h"
#include "Interface/PrefetcherServiceInterface.h"
#include "Interface/PackageServiceInterface.h"
#include "Interface/RandomizerInterface.h"
#include "Interface/PluginServiceInterface.h"
#include "Interface/CodecServiceInterface.h"
#include "Interface/ScriptServiceInterface.h"
#include "Interface/SchedulerInterface.h"
#include "Interface/TextServiceInterface.h"
#include "Interface/FontServiceInterface.h"
#include "Interface/AccountInterface.h"
#include "Interface/ResourceServiceInterface.h"
#include "Interface/ModuleServiceInterface.h"
#include "Interface/SceneServiceInterface.h"
#include "Interface/PlayerServiceInterface.h"
#include "Interface/ApplicationInterface.h"
#include "Interface/PickerServiceInterface.h"
#include "Interface/RenderServiceInterface.h"
#include "Interface/AnalyticsServiceInterface.h"
#include "Interface/TimerServiceInterface.h"
#include "Interface/ArrowServiceInterface.h"
#include "Interface/SettingsServiceInterface.h"

#include "Environment/Python/PythonIncluder.h"
#include "Environment/Python/PythonEventReceiver.h"
#include "Environment/Python/PythonScriptWrapper.h"
#include "Environment/Python/PythonDocumentTraceback.h"

#include "Plugins/ResourceValidatePlugin/ResourceValidateServiceInterface.h"

#include "Services/SecureService/SecureUnsignedValue.h"
#include "Services/SecureService/SecureStringValue.h"

#include "Services/AnalyticsService/AnalyticsContext.h"

#include "ScriptHolder.h"

#include "PythonValueFollowerLinear.h"
#include "PythonValueFollowerAcceleration.h"

#include "PythonEntityBehavior.h"
#include "PythonScheduleTiming.h"
#include "PythonSchedulePipe.h"
#include "PythonScheduleEvent.h"
#include "DelaySchedulePipe.h"
#include "PythonFileLogger.h"

#include "Engine/ResourceFile.h"
#include "Engine/ResourceTestPick.h"
#include "Engine/ResourceHIT.h"
#include "Engine/ResourceShape.h"
#include "Engine/HotSpotPolygon.h"
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
#include "Engine/Meshget.h"
#include "Engine/Isometric.h"
#include "Engine/Window.h"

#include "Kernel/ShapeQuadFixed.h"
#include "Kernel/ShapeQuadFlex.h"
#include "Kernel/ConfigHelper.h"
#include "Kernel/ThreadTask.h"
#include "Kernel/Scene.h"
#include "Kernel/MT19937Randomizer.h"
#include "Kernel/InputServiceHelper.h"
#include "Kernel/FileStreamHelper.h"
#include "Kernel/UnicodeHelper.h"
#include "Kernel/Assertion.h"
#include "Kernel/AssertionResourceType.h"
#include "Kernel/ConstStringHelper.h"
#include "Kernel/PolygonHelper.h"
#include "Kernel/ResourceSound.h"
#include "Kernel/ResourceImageDefault.h"
#include "Kernel/ResourceImageSubstractRGBAndAlpha.h"
#include "Kernel/ResourceImageSubstract.h"
#include "Kernel/ResourceImageSequence.h"
#include "Kernel/ResourceImageSolid.h"
#include "Kernel/ResourceImageData.h"
#include "Kernel/ResourceImage.h"
#include "Kernel/RenderViewport.h"
#include "Kernel/RenderScissor.h"
#include "Kernel/RenderCameraOrthogonal.h"
#include "Kernel/RenderCameraProjection.h"
#include "Kernel/RenderCameraOrthogonalTarget.h"
#include "Kernel/SurfaceSound.h"
#include "Kernel/SurfaceImage.h"
#include "Kernel/SurfaceImageSequence.h"
#include "Kernel/SurfaceSolidColor.h"
#include "Kernel/Shape.h"
#include "Kernel/Entity.h"
#include "Kernel/Layer.h"
#include "Kernel/Logger.h"
#include "Kernel/Identity.h"
#include "Kernel/Interender.h"
#include "Kernel/Affector.h"
#include "Kernel/DefaultPrototypeGenerator.h"
#include "Kernel/ScriptablePrototypeGenerator.h"
#include "Kernel/AssertionMemoryPanic.h"
#include "Kernel/Polygon.h"
#include "Kernel/MemoryStreamHelper.h"
#include "Kernel/ValueFollower.h"
#include "Kernel/Rect.h"
#include "Kernel/RenderContextHelper.h"
#include "Kernel/FactoryPool.h"
#include "Kernel/AssertionFactory.h"
#include "Kernel/FileGroupHelper.h"
#include "Kernel/ColorHelper.h"
#include "Kernel/ResourcePacket.h"
#include "Kernel/ContentHelper.h"
#include "Kernel/PrototypeHelper.h"
#include "Kernel/NodeScreenPosition.h"

#include "Config/StdString.h"
#include "Config/Lambda.h"
#include "Config/StdMath.h"
#include "Config/DynamicCast.h"
#include "Config/Path.h"

#include "math/angle.h"
#include "math/vec4.h"
#include "math/mat3.h"
#include "math/mat4.h"
#include "math/quat.h"
#include "math/utils.h"

#include "xmlsax/xmlsax.hpp"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    namespace
    {
        //////////////////////////////////////////////////////////////////////////
        typedef Vector<HotSpotPolygonPtr> VectorHotSpotPolygons;
        //////////////////////////////////////////////////////////////////////////
        class EngineScriptMethod
            : public Factorable
        {
        public:
            EngineScriptMethod()
            {
            }

            ~EngineScriptMethod() override
            {
            }

        public:
            bool initialize()
            {
                m_factoryPythonScheduleTiming = Helper::makeFactoryPool<PythonScheduleTiming, 8>( MENGINE_DOCUMENT_FACTORABLE );
                m_factoryPythonSchedulePipe = Helper::makeFactoryPool<PythonSchedulePipe, 8>( MENGINE_DOCUMENT_FACTORABLE );
                m_factoryDelaySchedulePipe = Helper::makeFactoryPool<DelaySchedulePipe, 8>( MENGINE_DOCUMENT_FACTORABLE );
                m_factoryPythonScheduleEvent = Helper::makeFactoryPool<PythonScheduleEvent, 8>( MENGINE_DOCUMENT_FACTORABLE );
                m_factoryPythonSceneChangeCallback = Helper::makeFactoryPool<PythonSceneChangeCallback, 8>( MENGINE_DOCUMENT_FACTORABLE );
                m_factoryAffectorUIWheelOfFortune = Helper::makeFactoryPool<AffectorUIWheelOfFortune, 8>( MENGINE_DOCUMENT_FACTORABLE );
                m_factoryAffectorGridBurnTransparency = Helper::makeFactoryPool<AffectorGridBurnTransparency, 4>( MENGINE_DOCUMENT_FACTORABLE );
                m_factoryAffectorUser = Helper::makeFactoryPool<AffectorUser, 4>( MENGINE_DOCUMENT_FACTORABLE );
                m_factoryPyGlobalMouseLeaveHandlers = Helper::makeFactoryPool<PyGlobalMouseLeaveHandler, 32>( MENGINE_DOCUMENT_FACTORABLE );
                m_factoryPyGlobalMouseMoveHandlers = Helper::makeFactoryPool<PyGlobalMouseMoveHandler, 32>( MENGINE_DOCUMENT_FACTORABLE );
                m_factoryPyGlobalMouseHandlerButtons = Helper::makeFactoryPool<PyGlobalMouseHandlerButton, 32>( MENGINE_DOCUMENT_FACTORABLE );
                m_factoryPyGlobalMouseHandlerButtonEnds = Helper::makeFactoryPool<PyGlobalMouseHandlerButtonEnd, 32>( MENGINE_DOCUMENT_FACTORABLE );
                m_factoryPyGlobalMouseHandlerWheels = Helper::makeFactoryPool<PyGlobalMouseHandlerWheel, 32>( MENGINE_DOCUMENT_FACTORABLE );
                m_factoryPyGlobalMouseHandlerButtonBegins = Helper::makeFactoryPool<PyGlobalMouseHandlerButtonBegin, 32>( MENGINE_DOCUMENT_FACTORABLE );
                m_factoryPyGlobalKeyHandler = Helper::makeFactoryPool<PyGlobalKeyHandler, 32>( MENGINE_DOCUMENT_FACTORABLE );
                m_factoryPyGlobalTextHandler = Helper::makeFactoryPool<PyGlobalTextHandler, 32>( MENGINE_DOCUMENT_FACTORABLE );
                m_factoryPyGlobalAccelerometerHandler = Helper::makeFactoryPool<PyGlobalAccelerometerHandler, 8>( MENGINE_DOCUMENT_FACTORABLE );
                m_factoryPyInputMousePositionProvider = Helper::makeFactoryPool<PyInputMousePositionProvider, 8>( MENGINE_DOCUMENT_FACTORABLE );

                m_creatorAffectorNodeFollowerLocalAlpha = Helper::makeFactorableUnique<AffectorNodeFollowerCreator<Node, float>>( MENGINE_DOCUMENT_FACTORABLE );
                m_creatorAffectorNodeFollowerLocalAlpha->initialize();

                m_creatorAffectorNodeFollowerCustomSize = Helper::makeFactorableUnique<AffectorNodeFollowerCreator<ShapeQuadFlex, mt::vec2f>>( MENGINE_DOCUMENT_FACTORABLE );
                m_creatorAffectorNodeFollowerCustomSize->initialize();

                m_creatorAffectorNodeFollowerTextureUVScale = Helper::makeFactorableUnique<AffectorNodeFollowerCreator<ShapeQuadFlex, mt::vec2f>>( MENGINE_DOCUMENT_FACTORABLE );
                m_creatorAffectorNodeFollowerTextureUVScale->initialize();

                return true;
            }

            void finalize()
            {
                m_creatorAffectorNodeFollowerLocalAlpha->finalize();
                m_creatorAffectorNodeFollowerLocalAlpha = nullptr;

                m_creatorAffectorNodeFollowerCustomSize->finalize();
                m_creatorAffectorNodeFollowerCustomSize = nullptr;

                m_creatorAffectorNodeFollowerTextureUVScale->finalize();
                m_creatorAffectorNodeFollowerTextureUVScale = nullptr;

                MENGINE_ASSERTION_FACTORY_EMPTY( m_factoryPythonScheduleTiming );
                MENGINE_ASSERTION_FACTORY_EMPTY( m_factoryPythonSchedulePipe );
                MENGINE_ASSERTION_FACTORY_EMPTY( m_factoryDelaySchedulePipe );
                MENGINE_ASSERTION_FACTORY_EMPTY( m_factoryPythonScheduleEvent );
                MENGINE_ASSERTION_FACTORY_EMPTY( m_factoryPythonSceneChangeCallback );
                MENGINE_ASSERTION_FACTORY_EMPTY( m_factoryAffectorUIWheelOfFortune );
                MENGINE_ASSERTION_FACTORY_EMPTY( m_factoryAffectorGridBurnTransparency );
                MENGINE_ASSERTION_FACTORY_EMPTY( m_factoryAffectorUser );
                MENGINE_ASSERTION_FACTORY_EMPTY( m_factoryPyGlobalMouseLeaveHandlers );
                MENGINE_ASSERTION_FACTORY_EMPTY( m_factoryPyGlobalMouseMoveHandlers );
                MENGINE_ASSERTION_FACTORY_EMPTY( m_factoryPyGlobalMouseHandlerButtons );
                MENGINE_ASSERTION_FACTORY_EMPTY( m_factoryPyGlobalMouseHandlerButtonEnds );
                MENGINE_ASSERTION_FACTORY_EMPTY( m_factoryPyGlobalMouseHandlerWheels );
                MENGINE_ASSERTION_FACTORY_EMPTY( m_factoryPyGlobalMouseHandlerButtonBegins );
                MENGINE_ASSERTION_FACTORY_EMPTY( m_factoryPyGlobalKeyHandler );
                MENGINE_ASSERTION_FACTORY_EMPTY( m_factoryPyGlobalTextHandler );
                MENGINE_ASSERTION_FACTORY_EMPTY( m_factoryPyInputMousePositionProvider );

                m_factoryPythonScheduleTiming = nullptr;
                m_factoryPythonSchedulePipe = nullptr;
                m_factoryDelaySchedulePipe = nullptr;
                m_factoryPythonScheduleEvent = nullptr;
                m_factoryPythonSceneChangeCallback = nullptr;
                m_factoryAffectorUIWheelOfFortune = nullptr;
                m_factoryAffectorGridBurnTransparency = nullptr;
                m_factoryAffectorUser = nullptr;
                m_factoryPyGlobalMouseLeaveHandlers = nullptr;
                m_factoryPyGlobalMouseMoveHandlers = nullptr;
                m_factoryPyGlobalMouseHandlerButtons = nullptr;
                m_factoryPyGlobalMouseHandlerButtonEnds = nullptr;
                m_factoryPyGlobalMouseHandlerWheels = nullptr;
                m_factoryPyGlobalMouseHandlerButtonBegins = nullptr;
                m_factoryPyGlobalKeyHandler = nullptr;
                m_factoryPyGlobalTextHandler = nullptr;
                m_factoryPyGlobalAccelerometerHandler = nullptr;
                m_factoryPyInputMousePositionProvider = nullptr;
            }

        public:
            //////////////////////////////////////////////////////////////////////////
            Polygon s_polygon_wm( Node * _node, const Polygon & _polygon )
            {
                MENGINE_ASSERTION_MEMORY_PANIC( _node, "Menge.polygon_wm send None object" );

                const TransformationInterface * transformation = _node->getTransformation();

                const mt::mat4f & wm = transformation->getWorldMatrix();

                Polygon polygon;
                _polygon.mul_wm( &polygon, wm );

                return polygon;
            }
            //////////////////////////////////////////////////////////////////////////
            Polygon s_polygon_anchor( const Polygon & _polygon, const mt::vec2f & _anchor )
            {
                Polygon polygon;
                _polygon.transpose( &polygon, _anchor );

                return polygon;
            }
            //////////////////////////////////////////////////////////////////////////
            bool s_testHotspot( const HotSpotPolygon * _left, const HotSpotPolygon * _right )
            {
                MENGINE_ASSERTION_MEMORY_PANIC( _left, "invalid left pointer" );
                MENGINE_ASSERTION_MEMORY_PANIC( _right, "invalid right pointer" );

                const Polygon & left_poligon = _left->getPolygon();
                const mt::mat4f & left_wm = _left->getTransformation()->getWorldMatrix();

                const Polygon & right_poligon = _right->getPolygon();
                const mt::mat4f & right_wm = _right->getTransformation()->getWorldMatrix();

                Polygon left_polygon_wm;
                left_poligon.mul_wm( &left_polygon_wm, left_wm );

                Polygon right_polygon_wm;
                right_poligon.mul_wm( &right_polygon_wm, right_wm );

                bool result = Helper::intersects( left_polygon_wm, right_polygon_wm );

                return result;
            }
            //////////////////////////////////////////////////////////////////////////
            bool s_loadPlugin( const WString & _pluginName )
            {
                Path utf8_pluginName = {'\0'};
                if( Helper::unicodeToUtf8( _pluginName, utf8_pluginName, MENGINE_MAX_PATH ) == false )
                {
                    return false;
                }

                bool successful = PLUGIN_SERVICE()
                    ->loadPlugin( utf8_pluginName, MENGINE_DOCUMENT_PYBIND );

                return successful;
            }
            //////////////////////////////////////////////////////////////////////////
            void s_setMousePickerBlockInput( bool _value )
            {
                PICKER_SERVICE()
                    ->setBlock( _value );
            }
            //////////////////////////////////////////////////////////////////////////
            void s_setMousePickerHandleValue( bool _value )
            {
                PICKER_SERVICE()
                    ->setHandleValue( _value );
            }
            //////////////////////////////////////////////////////////////////////////
            void s_setInputMouseButtonEventBlock( bool _value )
            {
                APPLICATION_SERVICE()
                    ->setInputMouseButtonEventBlock( _value );
            }
            //////////////////////////////////////////////////////////////////////////
            bool s_getInputMouseButtonEventBlock()
            {
                return APPLICATION_SERVICE()
                    ->getInputMouseButtonEventBlock();
            }
            //////////////////////////////////////////////////////////////////////////
            FactoryInterfacePtr m_factoryPythonScheduleTiming;
            //////////////////////////////////////////////////////////////////////////
            FactoryInterfacePtr m_factoryPythonSchedulePipe;
            //////////////////////////////////////////////////////////////////////////
            FactoryInterfacePtr m_factoryDelaySchedulePipe;
            //////////////////////////////////////////////////////////////////////////
            FactoryInterfacePtr m_factoryPythonScheduleEvent;
            //////////////////////////////////////////////////////////////////////////
            uint32_t s_timing( float _delay, const pybind::object & _timing, const pybind::object & _event, const pybind::args & _args )
            {
                const SchedulerInterfacePtr & tm = PLAYER_SERVICE()
                    ->getLocalScheduler();

                DelaySchedulePipePtr py_pipe = m_factoryDelaySchedulePipe->createObject( MENGINE_DOCUMENT_PYBIND );
                py_pipe->initialize( _delay );

                PythonScheduleTimingPtr py_timing = m_factoryPythonScheduleTiming->createObject( MENGINE_DOCUMENT_PYBIND );
                py_timing->initialize( _timing, _args );

                PythonScheduleEventPtr py_event = m_factoryPythonScheduleEvent->createObject( MENGINE_DOCUMENT_PYBIND );
                py_event->initialize( _event, _args );

                uint32_t id = tm->timing( py_pipe, py_timing, py_event, MENGINE_DOCUMENT_PYBIND );

                return id;
            }
            //////////////////////////////////////////////////////////////////////////
            bool s_timingRemove( UniqueId _id )
            {
                const SchedulerInterfacePtr & tm = PLAYER_SERVICE()
                    ->getLocalScheduler();

                bool successful = tm->remove( _id );

                return successful;
            }
            //////////////////////////////////////////////////////////////////////////
            SchedulerInterfacePtr s_createScheduler()
            {
                SchedulerInterfacePtr sm = PLAYER_SERVICE()
                    ->createScheduler( MENGINE_DOCUMENT_PYBIND );

                return sm;
            }
            //////////////////////////////////////////////////////////////////////////
            void s_destroyScheduler( const SchedulerInterfacePtr & _sm )
            {
                MENGINE_ASSERTION_MEMORY_PANIC( _sm, "destroy scheduler is nullptr" );

                PLAYER_SERVICE()
                    ->destroyScheduler( _sm );
            }
            //////////////////////////////////////////////////////////////////////////
            LayoutInterfacePtr s_createLayout()
            {
                LayoutInterfacePtr layout = PLAYER_SERVICE()
                    ->createLayout( MENGINE_DOCUMENT_PYBIND );

                return layout;
            }
            //////////////////////////////////////////////////////////////////////////
            void s_destroyLayout( const LayoutInterfacePtr & _layout )
            {
                MENGINE_ASSERTION_MEMORY_PANIC( _layout, "destroy layout is nullptr" );

                PLAYER_SERVICE()
                    ->destroyLayout( _layout );
            }
            //////////////////////////////////////////////////////////////////////////
            uint32_t s_pipe( const pybind::object & _pipe, const pybind::object & _timing, const pybind::object & _event, const pybind::args & _args )
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

                const SchedulerInterfacePtr & sm = PLAYER_SERVICE()
                    ->getLocalScheduler();

                uint32_t id = sm->timing( py_pipe, py_timing, py_event, MENGINE_DOCUMENT_PYBIND );

                return id;
            }
            //////////////////////////////////////////////////////////////////////////
            uint32_t s_schedule( float _timing, const pybind::object & _script, const pybind::args & _args )
            {
                const SchedulerInterfacePtr & sm = PLAYER_SERVICE()
                    ->getLocalScheduler();

                PythonScheduleEventPtr sl = m_factoryPythonScheduleEvent->createObject( MENGINE_DOCUMENT_PYBIND );
                sl->initialize( _script, _args );

                uint32_t id = sm->event( _timing, sl, MENGINE_DOCUMENT_PYBIND );

                return id;
            }
            //////////////////////////////////////////////////////////////////////////
            bool s_scheduleRemove( UniqueId _id )
            {
                const SchedulerInterfacePtr & sm = PLAYER_SERVICE()
                    ->getLocalScheduler();

                if( sm == nullptr )
                {
                    return false;
                }

                bool successful = sm->remove( _id );

                return successful;
            }
            //////////////////////////////////////////////////////////////////////////
            void s_scheduleRemoveAll()
            {
                const SchedulerInterfacePtr & sm = PLAYER_SERVICE()
                    ->getLocalScheduler();

                if( sm == nullptr )
                {
                    return;
                }

                sm->removeAll();
            }
            //////////////////////////////////////////////////////////////////////////
            bool s_scheduleFreeze( UniqueId _id, bool _freeze )
            {
                const SchedulerInterfacePtr & sm = PLAYER_SERVICE()
                    ->getLocalScheduler();

                if( sm == nullptr )
                {
                    return false;
                }

                bool successful = sm->freeze( _id, _freeze );

                return successful;
            }
            //////////////////////////////////////////////////////////////////////////
            void s_scheduleFreezeAll()
            {
                const SchedulerInterfacePtr & sm = PLAYER_SERVICE()
                    ->getLocalScheduler();

                if( sm == nullptr )
                {
                    return;
                }

                sm->freezeAll( true );
            }
            //////////////////////////////////////////////////////////////////////////
            void s_scheduleResumeAll()
            {
                const SchedulerInterfacePtr & sm = PLAYER_SERVICE()
                    ->getLocalScheduler();

                if( sm == nullptr )
                {
                    return;
                }

                sm->freezeAll( false );
            }
            //////////////////////////////////////////////////////////////////////////
            bool s_scheduleIsFreeze( UniqueId _id )
            {
                const SchedulerInterfacePtr & sm = PLAYER_SERVICE()
                    ->getLocalScheduler();

                if( sm == nullptr )
                {
                    return false;
                }

                bool result = sm->isFreeze( _id );

                return result;
            }
            //////////////////////////////////////////////////////////////////////////
            float s_scheduleTime( UniqueId _id )
            {
                const SchedulerInterfacePtr & sm = PLAYER_SERVICE()
                    ->getLocalScheduler();

                if( sm == nullptr )
                {
                    return 0.f;
                }

                float time = sm->getTimePassed( _id );

                return time;
            }
            //////////////////////////////////////////////////////////////////////////
            uint32_t s_scheduleGlobal( float _timing, const pybind::object & _cb, const pybind::args & _args )
            {
                const SchedulerInterfacePtr & sm = PLAYER_SERVICE()
                    ->getGlobalScheduler();

                MENGINE_ASSERTION_MEMORY_PANIC( sm, "scheduler global is nullptr" );

                PythonScheduleEventPtr sl = m_factoryPythonScheduleEvent->createObject( MENGINE_DOCUMENT_PYBIND );
                sl->initialize( _cb, _args );

                uint32_t id = sm->event( _timing, sl, MENGINE_DOCUMENT_PYBIND );

                return id;
            }
            //////////////////////////////////////////////////////////////////////////
            bool s_scheduleGlobalRemove( UniqueId _id )
            {
                const SchedulerInterfacePtr & sm = PLAYER_SERVICE()
                    ->getGlobalScheduler();

                if( sm == nullptr )
                {
                    return false;
                }

                bool successful = sm->remove( _id );

                return successful;
            }
            //////////////////////////////////////////////////////////////////////////
            void s_scheduleGlobalRemoveAll()
            {
                const SchedulerInterfacePtr & sm = PLAYER_SERVICE()
                    ->getGlobalScheduler();

                if( sm == nullptr )
                {
                    return;
                }

                sm->removeAll();
            }
            //////////////////////////////////////////////////////////////////////////
            bool s_scheduleGlobalFreeze( UniqueId _id, bool _freeze )
            {
                const SchedulerInterfacePtr & sm = PLAYER_SERVICE()
                    ->getGlobalScheduler();

                if( sm == nullptr )
                {
                    return false;
                }

                bool successful = sm->freeze( _id, _freeze );

                return successful;
            }
            //////////////////////////////////////////////////////////////////////////
            void s_scheduleGlobalFreezeAll()
            {
                const SchedulerInterfacePtr & sm = PLAYER_SERVICE()
                    ->getGlobalScheduler();

                if( sm == nullptr )
                {
                    return;
                }

                sm->freezeAll( true );
            }
            //////////////////////////////////////////////////////////////////////////
            void s_scheduleGlobalResumeAll()
            {
                const SchedulerInterfacePtr & sm = PLAYER_SERVICE()
                    ->getGlobalScheduler();

                if( sm == nullptr )
                {
                    return;
                }

                sm->freezeAll( false );
            }
            //////////////////////////////////////////////////////////////////////////
            bool s_scheduleGlobalIsFreeze( UniqueId _id )
            {
                const SchedulerInterfacePtr & sm = PLAYER_SERVICE()
                    ->getGlobalScheduler();

                if( sm == nullptr )
                {
                    return false;
                }

                bool freeze = sm->isFreeze( _id );

                return freeze;
            }
            //////////////////////////////////////////////////////////////////////////
            float s_scheduleGlobalPassed( UniqueId _id )
            {
                const SchedulerInterfacePtr & sm = PLAYER_SERVICE()
                    ->getGlobalScheduler();

                if( sm == nullptr )
                {
                    return 0.f;
                }

                float time = sm->getTimePassed( _id );

                return time;
            }
            //////////////////////////////////////////////////////////////////////////
            float s_scheduleGlobalLeft( UniqueId _id )
            {
                const SchedulerInterfacePtr & sm = PLAYER_SERVICE()
                    ->getGlobalScheduler();

                if( sm == nullptr )
                {
                    return 0.f;
                }

                float time = sm->getTimeLeft( _id );

                return time;
            }
            //////////////////////////////////////////////////////////////////////////
            void s_addHomeless( Node * _node )
            {
                MENGINE_ASSERTION_MEMORY_PANIC( _node, "Menge.addHomeless send None object" );

                _node->removeFromParent();
                _node->disable();
            }
            //////////////////////////////////////////////////////////////////////////
            bool s_isHomeless( Node * _node )
            {
                MENGINE_ASSERTION_MEMORY_PANIC( _node, "Menge.isHomeless send None object" );

                return _node->hasParent() == false;
            }
            //////////////////////////////////////////////////////////////////////////
            class PythonSceneChangeCallback
                : public SceneChangeCallbackInterface
                , public PythonCallbackProvider
                , public Factorable
            {
                DECLARE_FACTORABLE( PythonSceneChangeCallback );

            public:
                PythonSceneChangeCallback()
                {
                }

                ~PythonSceneChangeCallback() override
                {
                }

            public:
                void onSceneChange( const ScenePtr & _scene, bool _enable, bool _remove, bool _error ) override
                {
                    MENGINE_UNUSED( _error );

                    if( _remove == false )
                    {
                        if( _scene == nullptr )
                        {
                            this->call_cb( nullptr, _enable, _error );
                        }
                        else
                        {
                            const PythonEntityBehaviorPtr & behavior = _scene->getBehavior();
                            const pybind::object & py_scene = behavior->getScriptObject();

                            this->call_cb( py_scene, _enable, _error );
                        }
                    }
                    else
                    {
                        this->call_cb();
                    }
                }
            };
            //////////////////////////////////////////////////////////////////////////
            typedef IntrusivePtr<PythonSceneChangeCallback> PythonSceneChangeCallbackPtr;
            //////////////////////////////////////////////////////////////////////////
            FactoryInterfacePtr m_factoryPythonSceneChangeCallback;
            //////////////////////////////////////////////////////////////////////////
            bool createCurrentScene( const ConstString & _prototype, const ConstString & _name, bool _immediately, bool _destroyOld, const pybind::object & _cb, const pybind::args & _args )
            {
                if( SERVICE_IS_INITIALIZE( SceneServiceInterface ) == false )
                {
                    return false;
                }

                PythonSceneChangeCallbackPtr py_cb = m_factoryPythonSceneChangeCallback->createObject( MENGINE_DOCUMENT_PYBIND );
                py_cb->initialize( _cb, _args );

                ScenePtr currentScene = SCENE_SERVICE()
                    ->getCurrentScene();

                if( currentScene != nullptr && currentScene->getName() == _name )
                {
                    if( SCENE_SERVICE()
                        ->restartCurrentScene( _immediately, py_cb ) == false )
                    {
                        return false;
                    }
                }
                else
                {
                    ScenePtr scene = PROTOTYPE_SERVICE()
                        ->generatePrototype( Scene::getFactorableType(), _prototype, MENGINE_DOCUMENT_PYBIND );

                    MENGINE_ASSERTION_MEMORY_PANIC( scene, "invalid create scene '%s'"
                        , _name.c_str()
                    );

                    scene->setName( _name );

                    if( SCENE_SERVICE()
                        ->setCurrentScene( scene, _immediately, _destroyOld, py_cb ) == false )
                    {
                        scene = nullptr;

                        return false;
                    }
                }

                return true;
            }
            //////////////////////////////////////////////////////////////////////////
            bool setCurrentScene( const ScenePtr & _scene, bool _immediately, bool _destroyOld, const pybind::object & _cb, const pybind::args & _args )
            {
                if( SERVICE_IS_INITIALIZE( SceneServiceInterface ) == false )
                {
                    return false;
                }

                PythonSceneChangeCallbackPtr py_cb = m_factoryPythonSceneChangeCallback->createObject( MENGINE_DOCUMENT_PYBIND );
                py_cb->initialize( _cb, _args );

                ScenePtr currentScene = SCENE_SERVICE()
                    ->getCurrentScene();

                if( currentScene != nullptr && currentScene->getName() == _scene->getName() )
                {
                    if( SCENE_SERVICE()
                        ->restartCurrentScene( _immediately, py_cb ) == false )
                    {
                        return false;
                    }
                }
                else
                {
                    if( SCENE_SERVICE()
                        ->setCurrentScene( _scene, _immediately, _destroyOld, py_cb ) == false )
                    {
                        return false;
                    }
                }

                return true;
            }
            //////////////////////////////////////////////////////////////////////////
            bool restartCurrentScene( bool _immediately, const pybind::object & _cb, const pybind::args & _args )
            {
                if( SERVICE_IS_INITIALIZE( SceneServiceInterface ) == false )
                {
                    return false;
                }

                PythonSceneChangeCallbackPtr py_cb = m_factoryPythonSceneChangeCallback->createObject( MENGINE_DOCUMENT_PYBIND );
                py_cb->initialize( _cb, _args );

                if( SCENE_SERVICE()
                    ->restartCurrentScene( _immediately, py_cb ) == false )
                {
                    return false;
                }

                return true;
            }
            //////////////////////////////////////////////////////////////////////////
            const ScenePtr & getCurrentScene()
            {
                if( SERVICE_IS_INITIALIZE( SceneServiceInterface ) == false )
                {
                    return ScenePtr::none();
                }

                const ScenePtr & scene = SCENE_SERVICE()
                    ->getCurrentScene();

                return scene;
            }
            //////////////////////////////////////////////////////////////////////////
            ScenePtr s_createScene( const ConstString & _name, const pybind::object & _type )
            {
                ScenePtr scene = Helper::generateNodeFactorable<Scene>( MENGINE_DOCUMENT_PYBIND );

                MENGINE_ASSERTION_MEMORY_PANIC( scene, "invalid create scene '%s'"
                    , _name.c_str()
                );

                scene->setName( _name );

                if( _type.is_none() == false )
                {
                    EventablePtr eventable = SCRIPT_SERVICE()
                        ->eventableEntity( _type );

                    MENGINE_ASSERTION_MEMORY_PANIC( eventable, "scene '%s' invalid eventable '%s'"
                        , _name.c_str()
                        , _type.repr().c_str()
                    );

                    scene->setBehaviorEventable( eventable );

                    pybind::object py_scene = _type.call();

                    if( py_scene.is_invalid() == true )
                    {
                        LOGGER_ERROR( "scene '%s' invalid create type '%s'"
                            , _name.c_str()
                            , _type.repr().c_str()
                        );

                        return nullptr;
                    }

                    PythonEntityBehaviorPtr behavior = Helper::makeFactorableUnique<PythonEntityBehavior>( MENGINE_DOCUMENT_PYBIND );
                    behavior->setScriptObject( py_scene );

                    scene->setBehavior( behavior );
                }

                if( scene->create() == false )
                {
                    LOGGER_ERROR( "invalid create scene '%s'"
                        , _name.c_str()
                    );

                    return nullptr;
                }

                return scene;
            }
            //////////////////////////////////////////////////////////////////////////
            bool createGlobalScene()
            {
                if( SERVICE_IS_INITIALIZE( SceneServiceInterface ) == false )
                {
                    return false;
                }

                bool successful = SCENE_SERVICE()
                    ->createGlobalScene();

                return successful;
            }
            //////////////////////////////////////////////////////////////////////////
            void removeGlobalScene()
            {
                if( SERVICE_IS_INITIALIZE( SceneServiceInterface ) == false )
                {
                    return;
                }

                SCENE_SERVICE()
                    ->removeGlobalScene();
            }
            //////////////////////////////////////////////////////////////////////////
            const ScenePtr & getGlobalScene()
            {
                if( SERVICE_IS_INITIALIZE( SceneServiceInterface ) == false )
                {
                    return ScenePtr::none();
                }

                const ScenePtr & scene = SCENE_SERVICE()
                    ->getGlobalScene();

                return scene;
            }
            //////////////////////////////////////////////////////////////////////////
            ArrowServiceInterface * s_getArrowService()
            {
                ArrowServiceInterface * arrowService = ARROW_SERVICE();

                return arrowService;
            }
            //////////////////////////////////////////////////////////////////////////
            void s_hideArrow( bool _hide )
            {
                const NodePtr & node = ARROW_SERVICE()
                    ->getNode();

                RenderInterface * render = node->getRender();

                render->setLocalHide( _hide );
            }
            //////////////////////////////////////////////////////////////////////////
            const Resolution & s_getCurrentResolution()
            {
                const Resolution & resolution = APPLICATION_SERVICE()
                    ->getCurrentWindowResolution();

                return resolution;
            }
            //////////////////////////////////////////////////////////////////////////
            const Resolution & s_getContentResolution()
            {
                const Resolution & resolution = APPLICATION_SERVICE()
                    ->getContentResolution();

                return resolution;
            }
            //////////////////////////////////////////////////////////////////////////
            void s_setNopause( bool _value )
            {
                return APPLICATION_SERVICE()
                    ->setNopause( _value );
            }
            //////////////////////////////////////////////////////////////////////////
            bool s_getNopause()
            {
                return APPLICATION_SERVICE()
                    ->getNopause();
            }
            //////////////////////////////////////////////////////////////////////////
            bool s_setArrowLayer( const LayerPtr & _layer )
            {
                if( _layer == nullptr )
                {
                    return false;
                }

                const NodePtr & node = ARROW_SERVICE()
                    ->getNode();

                _layer->addChild( node );

                return true;
            }
            //////////////////////////////////////////////////////////////////////////
            mt::vec2f s_getHotSpotImageSize( const HotSpotImagePtr & _hotspotImage )
            {
                if( _hotspotImage == nullptr || _hotspotImage->isCompile() == false )
                {
                    return mt::vec2f( 0.f, 0.f );
                }

                const ResourceTestPickPtr & resourceHIT = _hotspotImage->getResourceTestPick();

                mt::vec2f size;
                size.x = resourceHIT->getImageWidthF();
                size.y = resourceHIT->getImageHeightF();

                return size;
            }
            //////////////////////////////////////////////////////////////////////////
            const mt::vec2f & s_getCamera2DPosition()
            {
                const RenderViewportInterfacePtr & rv = PLAYER_SERVICE()
                    ->getRenderViewport();

                const Viewport & vp = rv->getViewportWM();

                return vp.begin;
            }
            //////////////////////////////////////////////////////////////////////////
            void s_destroyNode( const NodePtr & _node )
            {
                MENGINE_ASSERTION_MEMORY_PANIC( _node, "invalid take None object" );

                _node->dispose();
            }
            //////////////////////////////////////////////////////////////////////////
            NodePtr s_createNode( const ConstString & _type )
            {
                ConstString correct_type = _type;

                if( correct_type == Node::getFactorableType() )
                {
                    LOGGER_WARNING( "type 'Node' is old deprecated type, use 'Interender' or other\ntraceback:\n%s"
                        , MENGINE_PYBIND_STATETRACE()
                    );

                    correct_type = Interender::getFactorableType();
                }

                NodePtr node = PROTOTYPE_SERVICE()
                    ->generatePrototype( Node::getFactorableType(), correct_type, MENGINE_DOCUMENT_PYBIND );

                MENGINE_ASSERTION_MEMORY_PANIC( node, "invalid create node '%s'"
                    , correct_type.c_str()
                );

                return node;
            }
            //////////////////////////////////////////////////////////////////////////
            SurfacePtr s_createSurface( const ConstString & _type )
            {
                SurfacePtr surface = PROTOTYPE_SERVICE()
                    ->generatePrototype( Surface::getFactorableType(), _type, MENGINE_DOCUMENT_PYBIND );

                MENGINE_ASSERTION_MEMORY_PANIC( surface, "invalid create surface '%s'"
                    , _type.c_str()
                );

                return surface;
            }
            //////////////////////////////////////////////////////////////////////////
            UniqueId s_addTimer( float _delay, const pybind::object & _timer, const pybind::args & _args )
            {
                UniqueId id = TIMER_SERVICE()
                    ->addTimer( _delay, [_timer, _args]( UniqueId _id )
                {
                    MENGINE_UNUSED( _id );

                    _timer.call_args( _id, _args );
                }, MENGINE_DOCUMENT_PYBIND );

                return id;
            }
            //////////////////////////////////////////////////////////////////////////
            void s_removeTimer( UniqueId _id )
            {
                TIMER_SERVICE()
                    ->removeTimer( _id );
            }
            //////////////////////////////////////////////////////////////////////////
            RandomizerInterfacePtr s_generateRandomizer( const ConstString & _prototype )
            {
                RandomizerInterfacePtr randomizer = PROTOTYPE_SERVICE()
                    ->generatePrototype( STRINGIZE_STRING_LOCAL( "Randomizer" ), _prototype, MENGINE_DOCUMENT_PYBIND );

                MENGINE_ASSERTION_MEMORY_PANIC( randomizer, "invalid create randomizer '%s'"
                    , _prototype.c_str()
                );

                return randomizer;
            }
            //////////////////////////////////////////////////////////////////////////
            ShapeQuadFixedPtr s_createSprite( const ConstString & _name, const ResourceImagePtr & _resource )
            {
                MENGINE_ASSERTION_MEMORY_PANIC( _resource, "'%s' resource is nullptr"
                    , _name.c_str()
                );

                SurfaceImagePtr surface = Helper::generateSurfaceFactorable<SurfaceImage>( MENGINE_DOCUMENT_PYBIND );

                MENGINE_ASSERTION_MEMORY_PANIC( surface, "invalid create surface '%s'"
                    , _name.c_str()
                );

                surface->setName( _name );
                surface->setResourceImage( _resource );

                ShapeQuadFixedPtr shape = Helper::generateNodeFactorable<ShapeQuadFixed>( MENGINE_DOCUMENT_PYBIND );

                MENGINE_ASSERTION_MEMORY_PANIC( shape, "invalid create shape '%s'"
                    , _name.c_str()
                );  

                shape->setName( _name );
                shape->setSurface( surface );

                return shape;
            }
            //////////////////////////////////////////////////////////////////////////
            void s_quitApplication()
            {
                APPLICATION_SERVICE()
                    ->quit();
            }
            //////////////////////////////////////////////////////////////////////////
            ResourcePtr s_createResource( const ConstString & _type )
            {
                ResourceCook cook;
                cook.type = _type;

                ResourcePtr resource = RESOURCE_SERVICE()
                    ->createResource( cook, MENGINE_DOCUMENT_PYBIND );

                MENGINE_ASSERTION_MEMORY_PANIC( resource, "invalid create resource '%s'"
                    , _type.c_str()
                );

                return resource;
            }
            //////////////////////////////////////////////////////////////////////////
            void s_removeResource( const ResourcePtr & _resource )
            {
                RESOURCE_SERVICE()
                    ->removeResource( _resource );
            }
            //////////////////////////////////////////////////////////////////////////
            bool s_directResourceCompile( const ConstString & _nameResource )
            {
                ResourcePtr resource;
                if( RESOURCE_SERVICE()
                    ->hasResource( ConstString::none(), _nameResource, false, &resource ) == false )
                {
                    LOGGER_ERROR( "not found resource '%s'"
                        , _nameResource.c_str()
                    );

                    return false;
                }

                if( resource->compile() == false )
                {
                    LOGGER_ERROR( "resource '%s' type '%s' invalid compile"
                        , _nameResource.c_str()
                        , resource->getType().c_str()
                    );

                    return false;
                }

                return true;
            }
            //////////////////////////////////////////////////////////////////////////
            bool s_directResourceRelease( const ConstString & _nameResource )
            {
                ResourcePtr resource;
                if( RESOURCE_SERVICE()
                    ->hasResource( ConstString::none(), _nameResource, false, &resource ) == false )
                {
                    LOGGER_ERROR( "not found resource '%s'"
                        , _nameResource.c_str()
                    );

                    return false;
                }

                resource->release();

                return true;
            }
            //////////////////////////////////////////////////////////////////////////
            bool s_directFontCompile( const ConstString & _fontName )
            {
                bool successful = FONT_SERVICE()
                    ->directFontCompile( _fontName );

                return successful;
            }
            //////////////////////////////////////////////////////////////////////////
            bool s_directFontRelease( const ConstString & _fontName )
            {
                bool successful = FONT_SERVICE()
                    ->directFontRelease( _fontName );

                return successful;
            }
            //////////////////////////////////////////////////////////////////////////
            const ResourcePtr & s_getResource( const ConstString & _name )
            {
                const ResourcePtr & resource = RESOURCE_SERVICE()
                    ->getResourceReference( ConstString::none(), _name );

                MENGINE_ASSERTION_MEMORY_PANIC( resource, "not exist resource '%s'"
                    , _name.c_str()
                );

                if( resource->compile() == false )
                {
                    LOGGER_ERROR( "resource '%s' type '%s' invalid compile"
                        , resource->getName().c_str()
                        , resource->getType().c_str()
                    );

                    return ResourcePtr::none();
                }

                return resource;
            }
            //////////////////////////////////////////////////////////////////////////
            const ResourcePtr & s_getResourceReference( const ConstString & _name )
            {
                const ResourcePtr & resource = RESOURCE_SERVICE()
                    ->getResourceReference( ConstString::none(), _name );

                MENGINE_ASSERTION_MEMORY_PANIC( resource, "not exist resource '%s'"
                    , _name.c_str()
                );

                return resource;
            }
            //////////////////////////////////////////////////////////////////////////
            void s_cancelTask( const ThreadTaskPtr & _task )
            {
                _task->cancel();
            }
            //////////////////////////////////////////////////////////////////////////
            bool s_joinTask( const ThreadTaskPtr & _task )
            {
                return THREAD_SERVICE()
                    ->joinTask( _task );
            }
            //////////////////////////////////////////////////////////////////////////
            void s_setFullscreenMode( bool _fullscreen )
            {
                APPLICATION_SERVICE()
                    ->setFullscreenMode( _fullscreen );
            }
            //////////////////////////////////////////////////////////////////////////
            bool s_getFullscreenMode()
            {
                return APPLICATION_SERVICE()
                    ->getFullscreenMode();
            }
            //////////////////////////////////////////////////////////////////////////
            void s_setFixedContentResolution( bool _fixedContentResolution )
            {
                APPLICATION_SERVICE()
                    ->setFixedContentResolution( _fixedContentResolution );
            }
            //////////////////////////////////////////////////////////////////////////
            bool s_getFixedContentResolution()
            {
                return APPLICATION_SERVICE()
                    ->getFixedContentResolution();
            }
            //////////////////////////////////////////////////////////////////////////
            void s_setFixedDisplayResolution( bool _fixedDisplayResolution )
            {
                APPLICATION_SERVICE()
                    ->setFixedDisplayResolution( _fixedDisplayResolution );
            }
            //////////////////////////////////////////////////////////////////////////
            bool s_getFixedDisplayResolution()
            {
                return APPLICATION_SERVICE()
                    ->getFixedDisplayResolution();
            }
            //////////////////////////////////////////////////////////////////////////
            void s_setFixedViewportResolution( bool _fixedViewportResolution )
            {
                APPLICATION_SERVICE()
                    ->setFixedViewportResolution( _fixedViewportResolution );
            }
            //////////////////////////////////////////////////////////////////////////
            bool s_getFixedViewportResolution()
            {
                return APPLICATION_SERVICE()
                    ->getFixedViewportResolution();
            }
            //////////////////////////////////////////////////////////////////////////
            void s_foreachSceneHotspot( const ScenePtr & _scene, const pybind::object & _cb, const pybind::args & _args )
            {
                class HotSpotVisitor
                    : public Visitor
                    , public ConcreteVisitor<HotSpot>
                    , public PythonCallbackProvider
                    , public Factorable
                {
                public:
                    HotSpotVisitor( const pybind::object & _cb, const pybind::args & _args )
                        : PythonCallbackProvider( _cb, _args )
                    {
                    }

                public:
                    void accept( HotSpot * _hotspot ) override
                    {
                        this->call_cb( _hotspot );
                    }
                };

                typedef IntrusivePtr<HotSpotVisitor> HotSpotVisitorPtr;

                HotSpotVisitorPtr visitor = Helper::makeFactorableUnique<HotSpotVisitor>( MENGINE_DOCUMENT_PYBIND, _cb, _args );

                _scene->visitThree( visitor );
            }
            //////////////////////////////////////////////////////////////////////////
            void s_renderOneFrame()
            {
            }
            //////////////////////////////////////////////////////////////////////////
            void s_writeImageToFile( const ConstString & _resource, const FilePath & _filePath )
            {
                LOGGER_WARNING( "write image to file '%s' path '%s'"
                    , _resource.c_str()
                    , _filePath.c_str()
                );

                const ResourceImagePtr & resource = RESOURCE_SERVICE()
                    ->getResourceReference( ConstString::none(), _resource );

                MENGINE_ASSERTION_MEMORY_PANIC( resource, "image resource not getting '%s'"
                    , _resource.c_str()
                );

                if( resource->compile() == false )
                {
                    LOGGER_ERROR( "resource '%s' type '%s' invalid compile"
                        , resource->getName().c_str()
                        , resource->getType().c_str()
                    );

                    return;
                }

                const FileGroupInterfacePtr & fileGroup = FILE_SERVICE()
                    ->getFileGroup( STRINGIZE_STRING_LOCAL( "user" ) );

                const RenderTextureInterfacePtr & texture = resource->getTexture( 0 );

                ContentInterfacePtr content = Helper::makeFileContent( fileGroup, _filePath, MENGINE_DOCUMENT_PYBIND );

                MENGINE_ASSERTION_MEMORY_PANIC( content, "invalid create content '%s'"
                    , _filePath.c_str()
                );

                content->setCodecType( STRINGIZE_STRING_LOCAL( "pngImage" ) );

                RENDERTEXTURE_SERVICE()
                    ->saveImage( texture, content );

                resource->release();
            }
            //////////////////////////////////////////////////////////////////////////
            void setParticlesEnabled( bool _enabled )
            {
                APPLICATION_SERVICE()
                    ->setParticleEnable( _enabled );
            }
            //////////////////////////////////////////////////////////////////////////
            ResourceImageDefaultPtr s_createImageResource( const ConstString & _resourceName, const ConstString & _fileGroupName, const FilePath & _filePath, const mt::vec2f & _maxSize )
            {
                const FileGroupInterfacePtr & fileGroup = FILE_SERVICE()
                    ->getFileGroup( _fileGroupName );

#if defined(MENGINE_DEBUG)
                {
                    InputStreamInterfacePtr stream = Helper::openInputStreamFile( fileGroup, _filePath, false, false, MENGINE_DOCUMENT_PYBIND );

                    MENGINE_ASSERTION_MEMORY_PANIC( stream, "image file '%s' was not found"
                        , Helper::getFileGroupFullPath( fileGroup, _filePath ).c_str()
                    );

                    MENGINE_ASSERTION_FATAL( stream->size() != 0, "empty stream '%s'"
                        , Helper::getFileGroupFullPath( fileGroup, _filePath ).c_str()
                    );

                    uint8_t Magic_PNGBytes[] = {0x89, 0x50, 0x4E, 0x47, 0x0D, 0x0A, 0x1A, 0x0A};
                    uint8_t Magic_BMPBytes[] = {0x42, 0x4D};
                    uint8_t Magic_JPGBytes[] = {0xFF, 0xD8, 0xFF};
                    uint8_t Magic_JPEGBytes[] = {0x00, 0x00, 0x00, 0x0C, 0x6A, 0x50, 0x20, 0x20};
                    uint8_t Magic_WEBPBytes[] = {0x52, 0x49, 0x46, 0x46};

                    uint8_t magic_image[8];
                    if( stream->read( magic_image, 8 ) != 8 )
                    {
                        return nullptr;
                    }

                    ConstString codecTypeMagic;

                    if( memcmp( magic_image, Magic_PNGBytes, 8 ) == 0 )
                    {
                        codecTypeMagic = STRINGIZE_STRING_LOCAL( "pngImage" );
                    }
                    else if( memcmp( magic_image, Magic_BMPBytes, 2 ) == 0 )
                    {
                        codecTypeMagic = STRINGIZE_STRING_LOCAL( "bmpImage" );
                    }
                    else if( memcmp( magic_image, Magic_JPGBytes, 3 ) == 0 )
                    {
                        codecTypeMagic = STRINGIZE_STRING_LOCAL( "jpegImage" );
                    }
                    else if( memcmp( magic_image, Magic_JPEGBytes, 8 ) == 0 )
                    {
                        codecTypeMagic = STRINGIZE_STRING_LOCAL( "jpegImage" );
                    }
                    else if( memcmp( magic_image, Magic_WEBPBytes, 4 ) == 0 )
                    {
                        codecTypeMagic = STRINGIZE_STRING_LOCAL( "webpImage" );
                    }
                    else
                    {
                        LOGGER_ERROR( "invalid get image '%s' type from magic bytes"
                            , Helper::getFileGroupFullPath( fileGroup, _filePath ).c_str()
                        );

                        return nullptr;
                    }

                    ConstString codecType = CODEC_SERVICE()
                        ->findCodecType( _filePath );

                    if( codecTypeMagic != codecType )
                    {
                        LOGGER_ERROR( "image '%s' ext not magic bytes types '%s' != '%s'"
                            , Helper::getFileGroupFullPath( fileGroup, _filePath ).c_str()
                            , codecType.c_str()
                            , codecTypeMagic.c_str()
                        );

                        return nullptr;
                    }
                }
#endif

                mt::vec2f maxSize;

                if( _maxSize.x < 0.f || _maxSize.y < 0.f )
                {
                    InputStreamInterfacePtr stream = Helper::openInputStreamFile( fileGroup, _filePath, false, false, MENGINE_DOCUMENT_PYBIND );

                    MENGINE_ASSERTION_MEMORY_PANIC( stream, "image file '%s' was not found"
                        , Helper::getFileGroupFullPath( fileGroup, _filePath ).c_str()
                    );

                    MENGINE_ASSERTION_FATAL( stream->size() != 0, "empty stream '%s'"
                        , Helper::getFileGroupFullPath( fileGroup, _filePath ).c_str()
                    );

                    ConstString codecType = CODEC_SERVICE()
                        ->findCodecType( _filePath );

                    ImageDecoderInterfacePtr imageDecoder = CODEC_SERVICE()
                        ->createDecoder( codecType, MENGINE_DOCUMENT_PYBIND );

                    MENGINE_ASSERTION_MEMORY_PANIC( imageDecoder, "invalid create decoder '%s' for '%s'"
                        , codecType.c_str()
                        , Helper::getFileGroupFullPath( fileGroup, _filePath ).c_str()
                    );

                    if( imageDecoder->prepareData( stream ) == false )
                    {
                        return nullptr;
                    }

                    const ImageCodecDataInfo * dataInfo = imageDecoder->getCodecDataInfo();

                    maxSize.x = (float)dataInfo->width;
                    maxSize.y = (float)dataInfo->height;
                }
                else
                {
                    maxSize = _maxSize;
                }

                ResourceCook cook;
                cook.type = ResourceImageDefault::getFactorableType();

                ResourceImageDefaultPtr resource = RESOURCE_SERVICE()
                    ->createResource( cook, MENGINE_DOCUMENT_PYBIND );

                MENGINE_ASSERTION_MEMORY_PANIC( resource, "invalid create resource '%s'"
                    , _resourceName.c_str()
                );

                resource->setName( _resourceName );

                const ConstString & newCodecType = CODEC_SERVICE()
                    ->findCodecType( _filePath );

                if( newCodecType.empty() == true )
                {
                    return nullptr;
                }

                ContentInterfacePtr content = Helper::makeFileContent( fileGroup, _filePath, MENGINE_DOCUMENT_PYBIND );

                content->setCodecType( newCodecType );

                resource->setContent( content );

                resource->setMaxSize( maxSize );
                resource->setSize( maxSize );
                resource->setOffset( mt::vec2f( 0.f, 0.f ) );

                resource->setAlpha( true );

                if( resource->initialize() == false )
                {
                    LOGGER_ERROR( "invalid initialize image '%s'"
                        , Helper::getFileGroupFullPath( fileGroup, _filePath ).c_str()
                    );

                    return nullptr;
                }

                return resource;
            }
            //////////////////////////////////////////////////////////////////////////
            ResourceImageSolidPtr s_createImageSolidResource( const ConstString & _resourceName, const Color & _color, const mt::vec2f & _maxSize )
            {
                ResourceCook cook;
                cook.type = ResourceImageSolid::getFactorableType();

                ResourceImageSolidPtr resource = RESOURCE_SERVICE()
                    ->createResource( cook, MENGINE_DOCUMENT_PYBIND );

                MENGINE_ASSERTION_MEMORY_PANIC( resource, "invalid create resource '%s'"
                    , _resourceName.c_str()
                );

                resource->setName( _resourceName );

                resource->setColor( _color );

                resource->setMaxSize( _maxSize );
                resource->setSize( _maxSize );

                if( resource->initialize() == false )
                {
                    LOGGER_ERROR( "invalid initialize image solid '%s'"
                        , _resourceName.c_str()
                    );

                    return nullptr;
                }

                return resource;
            }
            //////////////////////////////////////////////////////////////////////////
            void s_minimizeWindow()
            {
                APPLICATION_SERVICE()
                    ->minimizeWindow();
            }
            //////////////////////////////////////////////////////////////////////////
            bool s_calcMouseScreenPosition( const mt::vec2f & _pos, mt::vec2f * const _screen )
            {
                const NodePtr & node = ARROW_SERVICE()
                    ->getNode();

                const RenderInterface * render = node->getRender();

                const RenderCameraInterfacePtr & camera = render->getRenderCamera();
                const RenderViewportInterfacePtr & viewport = render->getRenderViewport();

                MENGINE_ASSERTION_MEMORY_PANIC( camera, "invalid get arrow render camera inheritance" );
                MENGINE_ASSERTION_MEMORY_PANIC( viewport, "invalid get arrow render viewport inheritance" );

                RenderContext context;
                Helper::clearRenderContext( &context );

                context.camera = camera.get();
                context.viewport = viewport.get();

                ARROW_SERVICE()
                    ->calcMouseScreenPosition( &context, _pos, _screen );

                return true;
            }
            //////////////////////////////////////////////////////////////////////////
            void s_pushMouseMove( ETouchCode _touchId, const mt::vec2f & _position )
            {
                const mt::vec2f & cursorPosition = INPUT_SERVICE()
                    ->getCursorPosition( _touchId );

                mt::vec2f screenPosition;
                if( this->s_calcMouseScreenPosition( _position, &screenPosition ) == false )
                {
                    return;
                }

                mt::vec2f mp = screenPosition - cursorPosition;

                Helper::pushMouseMoveEvent( _touchId, cursorPosition.x, cursorPosition.y, mp.x, mp.y, 0.f, 0.f );
            }
            //////////////////////////////////////////////////////////////////////////
            void s_pushMouseButtonEvent( ETouchCode _touchId, const mt::vec2f & _pos, EMouseButtonCode _button, bool _isDown )
            {
                mt::vec2f pos_screen;
                if( this->s_calcMouseScreenPosition( _pos, &pos_screen ) == false )
                {
                    return;
                }

                Helper::pushMouseButtonEvent( _touchId, pos_screen.x, pos_screen.y, _button, 0.f, _isDown );
            }
            //////////////////////////////////////////////////////////////////////////
            void s_platformEvent( const ConstString & _event, const Params & _params )
            {
                PLATFORM_SERVICE()
                    ->onEvent( _event, _params );
            }
            //////////////////////////////////////////////////////////////////////////
            PyObject * s_getCompanyName( pybind::kernel_interface * _kernel )
            {
                Char companyName[MENGINE_APPLICATION_COMPANY_MAXNAME + 1] = {'\0'};
                APPLICATION_SERVICE()
                    ->getCompanyName( companyName );

                return _kernel->string_from_char( companyName );
            }
            //////////////////////////////////////////////////////////////////////////
            PyObject * s_getProjectName( pybind::kernel_interface * _kernel )
            {
                Char projectName[MENGINE_APPLICATION_PROJECT_MAXNAME + 1] = {'\0'};
                APPLICATION_SERVICE()
                    ->getProjectName( projectName );

                return _kernel->string_from_char( projectName );
            }
            //////////////////////////////////////////////////////////////////////////
            const ConstString & s_getProjectCodename()
            {
                const ConstString & codename = APPLICATION_SERVICE()
                    ->getProjectCodename();

                return codename;
            }
            //////////////////////////////////////////////////////////////////////////
            PyObject * s_getDeviceLanguage( pybind::kernel_interface * _kernel )
            {
                Char deviceLanguage[MENGINE_ENVIRONMENT_DEVICE_LANGUAGE_MAXNAME + 1] = {'\0'};
                ENVIRONMENT_SERVICE()
                    ->getDeviceLanguage( deviceLanguage );

                return _kernel->string_from_char( deviceLanguage );
            }
            //////////////////////////////////////////////////////////////////////////
            const RenderCameraOrthogonalPtr & s_getDefaultSceneRenderCamera2D()
            {
                const RenderCameraOrthogonalPtr & camera = PLAYER_SERVICE()
                    ->getDefaultSceneRenderCamera2D();

                return camera;
            }
            //////////////////////////////////////////////////////////////////////////
            const RenderViewportPtr & s_getDefaultRenderViewport2D()
            {
                const RenderViewportPtr & viewport = PLAYER_SERVICE()
                    ->getDefaultRenderViewport2D();

                return viewport;
            }
            //////////////////////////////////////////////////////////////////////////
            const RenderCameraOrthogonalPtr & s_getDefaultArrowRenderCamera2D()
            {
                const RenderCameraOrthogonalPtr & camera = PLAYER_SERVICE()
                    ->getDefaultArrowRenderCamera2D();

                return camera;
            }
            //////////////////////////////////////////////////////////////////////////
            void s_AnalyticsContext_addParameterBoolean( AnalyticsContextInterface * _context, const ConstString & _name, bool _value )
            {
                _context->addParameterBoolean( _name, _value, MENGINE_DOCUMENT_PYBIND );
            }
            //////////////////////////////////////////////////////////////////////////
            void s_AnalyticsContext_addParameterInteger( AnalyticsContextInterface * _context, const ConstString & _name, int64_t _value )
            {
                _context->addParameterInteger( _name, _value, MENGINE_DOCUMENT_PYBIND );
            }
            //////////////////////////////////////////////////////////////////////////
            void s_AnalyticsContext_addParameterDouble( AnalyticsContextInterface * _context, const ConstString & _name, double _value )
            {
                _context->addParameterDouble( _name, _value, MENGINE_DOCUMENT_PYBIND );
            }
            //////////////////////////////////////////////////////////////////////////
            void s_AnalyticsContext_addParameterString( AnalyticsContextInterface * _context, const ConstString & _name, const ConstString & _value )
            {
                _context->addParameterConstString( _name, _value, MENGINE_DOCUMENT_PYBIND );
            }
            //////////////////////////////////////////////////////////////////////////
            void s_AnalyticsContext_addParameterGetterBoolean( AnalyticsContextInterface * _context, const ConstString & _name, const pybind::object & _cb, const pybind::args & _args )
            {
                _context->addParameterGetterBoolean( _name, [_cb, _args]()
                {
                    bool value = _cb.call( _args );

                    return value;
                }, MENGINE_DOCUMENT_PYBIND );
            }
            //////////////////////////////////////////////////////////////////////////
            void s_AnalyticsContext_addParameterGetterInteger( AnalyticsContextInterface * _context, const ConstString & _name, const pybind::object & _cb, const pybind::args & _args )
            {
                _context->addParameterGetterInteger( _name, [_cb, _args]()
                {
                    int64_t value = _cb.call( _args );

                    return value;
                }, MENGINE_DOCUMENT_PYBIND );
            }
            //////////////////////////////////////////////////////////////////////////
            void s_AnalyticsContext_addParameterGetterDouble( AnalyticsContextInterface * _context, const ConstString & _name, const pybind::object & _cb, const pybind::args & _args )
            {
                _context->addParameterGetterDouble( _name, [_cb, _args]()
                {
                    double value = _cb.call( _args );

                    return value;
                }, MENGINE_DOCUMENT_PYBIND );
            }
            //////////////////////////////////////////////////////////////////////////
            void s_AnalyticsContext_addParameterGetterString( AnalyticsContextInterface * _context, const ConstString & _name, const pybind::object & _cb, const pybind::args & _args )
            {
                _context->addParameterGetterConstString( _name, [_cb, _args]()
                {
                    ConstString value = _cb.call( _args );

                    return value;
                }, MENGINE_DOCUMENT_PYBIND );
            }
            //////////////////////////////////////////////////////////////////////////
            AnalyticsContextInterfacePtr s_analyticsMakeContext()
            {
                AnalyticsContextInterfacePtr context = ANALYTICS_SERVICE()
                    ->makeContext( MENGINE_DOCUMENT_PYBIND );

                return context;
            }
            //////////////////////////////////////////////////////////////////////////
            void s_addAnalyticsGlobalContextParameterBoolean( const ConstString & _name, bool _value )
            {
                const AnalyticsContextInterfacePtr & analyticsGlobalContext = ANALYTICS_SERVICE()
                    ->getGlobalContext();

                analyticsGlobalContext->addParameterBoolean( _name, _value, MENGINE_DOCUMENT_PYBIND );
            }
            //////////////////////////////////////////////////////////////////////////
            void s_addAnalyticsGlobalContextParameterString( const ConstString & _name, const ConstString & _value )
            {
                const AnalyticsContextInterfacePtr & analyticsGlobalContext = ANALYTICS_SERVICE()
                    ->getGlobalContext();

                analyticsGlobalContext->addParameterConstString( _name, _value, MENGINE_DOCUMENT_PYBIND );
            }
            //////////////////////////////////////////////////////////////////////////
            void s_addAnalyticsGlobalContextParameterInteger( const ConstString & _name, int64_t _value )
            {
                const AnalyticsContextInterfacePtr & analyticsGlobalContext = ANALYTICS_SERVICE()
                    ->getGlobalContext();

                analyticsGlobalContext->addParameterInteger( _name, _value, MENGINE_DOCUMENT_PYBIND );
            }
            //////////////////////////////////////////////////////////////////////////
            void s_addAnalyticsGlobalContextParameterDouble( const ConstString & _name, double _value )
            {
                const AnalyticsContextInterfacePtr & analyticsGlobalContext = ANALYTICS_SERVICE()
                    ->getGlobalContext();

                analyticsGlobalContext->addParameterDouble( _name, _value, MENGINE_DOCUMENT_PYBIND );
            }
            //////////////////////////////////////////////////////////////////////////
            void s_analyticsEvent( const ConstString & _eventName, const AnalyticsContextInterfacePtr & _context, const pybind::dict & _parameters )
            {
                AnalyticsEventBuilderInterfacePtr builder = ANALYTICS_SERVICE()
                    ->buildEvent( _eventName, MENGINE_DOCUMENT_PYBIND );

                builder->setLocalContext( _context );

                for( pybind::dict::iterator
                    it = _parameters.begin(),
                    it_end = _parameters.end();
                    it != it_end;
                    ++it )
                {
                    ConstString k = it.key();
                    pybind::object v = it.value();

                    if( v.is_bool() == true )
                    {
                        bool value = v.extract();

                        builder->addParameterBoolean( k, value );
                    }
                    else if( v.is_integer() == true )
                    {
                        int64_t value = v.extract();

                        builder->addParameterInteger( k, value );
                    }
                    else if( v.is_long() == true )
                    {
                        int64_t value = v.extract();

                        builder->addParameterInteger( k, value );
                    }
                    else if( v.is_float() == true )
                    {
                        double value = v.extract();

                        builder->addParameterDouble( k, value );
                    }
                    else if( v.is_string() == true )
                    {
                        String value = v.extract();

                        builder->addParameterString( k, value );
                    }
                    else if( v.is_unicode() == true )
                    {
                        WString value = v.extract();

                        String utf8_value;
                        if( Helper::unicodeToUtf8( value, &utf8_value ) == false )
                        {
                            LOGGER_ERROR( "invalid convert to utf8 pybind analytics parameter '%s' value '%s' type '%s'"
                                , k.c_str()
                                , v.repr().c_str()
                                , v.repr_type().c_str()
                            );

                            return;
                        }

                        builder->addParameterString( k, utf8_value );
                    }
                    else if( v.is_embedded_type<ConstString>() == true )
                    {
                        ConstString value = v.extract();

                        builder->addParameterConstString( k, value );
                    }
                    else
                    {
                        LOGGER_ERROR( "unsupport pybind analytics parameter '%s' value '%s' type '%s'"
                            , k.c_str()
                            , v.repr().c_str()
                            , v.repr_type().c_str()
                        );

                        return;
                    }
                }

                builder->log();
            }
            //////////////////////////////////////////////////////////////////////////
            void s_analyticsScreenView( const ConstString & _screenType, const ConstString & _screenName )
            {
                ANALYTICS_SERVICE()
                    ->logScreenView( _screenType, _screenName );
            }
            //////////////////////////////////////////////////////////////////////////
            void s_analyticsEventFlush()
            {
                ANALYTICS_SERVICE()
                    ->logFlush();
            }
            //////////////////////////////////////////////////////////////////////////
            bool s_mountResourcePackage( const ConstString & _fileGroupName
                , const ConstString & _name
                , const ConstString & _type
                , const ConstString & _category
                , const FilePath & _path
                , const FilePath & _descriptionPath )
            {
                FileGroupInterfacePtr fileGroup;
                if( FILE_SERVICE()
                    ->hasFileGroup( _fileGroupName, &fileGroup ) == false )
                {
                    LOGGER_ERROR( "invalid found file group '%s'"
                        , _fileGroupName.c_str()
                    );

                    return false;
                }

                LOGGER_INFO( "system", "mount resource package name '%s' type '%s' category '%s' path '%s'"
                    , _name.c_str()
                    , _type.c_str()
                    , _category.c_str()
                    , _path.c_str()
                );

                PackageDesc desc;
                desc.dev = false;
                desc.immediately = true;
                desc.preload = false;

                desc.name = _name;
                desc.type = _type;
                desc.fileGroupName = _category;
                desc.path = _path;
                desc.descriptionPath = _descriptionPath;

                bool result = PACKAGE_SERVICE()
                    ->addPackage( desc, MENGINE_DOCUMENT_PYBIND );

                return result;
            }
            //////////////////////////////////////////////////////////////////////////
            bool s_hasPackage( const ConstString & _name )
            {
                bool result = PACKAGE_SERVICE()
                    ->hasPackage( _name );

                return result;
            }
            //////////////////////////////////////////////////////////////////////////
            bool s_unmountResourcePackage( const ConstString & _name )
            {
                bool result = PACKAGE_SERVICE()
                    ->removePackage( _name );

                return result;
            }
            //////////////////////////////////////////////////////////////////////////
            bool s_existFile( const ConstString & _fileGroupName, const FilePath & _filePath )
            {
                const FileGroupInterfacePtr & fileGroup = FILE_SERVICE()
                    ->getFileGroup( _fileGroupName );

                MENGINE_ASSERTION_MEMORY_PANIC( fileGroup, "not found file group '%s'"
                    , _fileGroupName.c_str()
                );

                bool result = fileGroup->existFile( _filePath, true );

                return result;
            }
            //////////////////////////////////////////////////////////////////////////
            bool s_removeFile( const ConstString & _fileGroupName, const FilePath & _filePath )
            {
                const FileGroupInterfacePtr & fileGroup = FILE_SERVICE()
                    ->getFileGroup( _fileGroupName );

                MENGINE_ASSERTION_MEMORY_PANIC( fileGroup, "not found file group '%s'"
                    , _fileGroupName.c_str()
                );

                bool result = fileGroup->removeFile( _filePath );

                return result;
            }
            //////////////////////////////////////////////////////////////////////////
            struct python_xmlsax_desc
            {
                pybind::kernel_interface * kernel;
                pybind::object cb;
            };
            //////////////////////////////////////////////////////////////////////////
            static void python_callback_begin_node( const xmlsax_char_t * _node, void * _userdata )
            {
                python_xmlsax_desc * desc = (python_xmlsax_desc *)_userdata;

                pybind::object py_cb_begin = desc->cb.get_attr( "begin" );

                py_cb_begin.call( _node );
            }
            //////////////////////////////////////////////////////////////////////////
            static void python_callback_node_attributes( const xmlsax_char_t * _node, uint32_t _count, const xmlsax_char_t ** _keys, const xmlsax_char_t ** _values, void * _userdata )
            {
                MENGINE_UNUSED( _node );

                python_xmlsax_desc * desc = (python_xmlsax_desc *)_userdata;

                pybind::dict py_attr( desc->kernel, _count );

                for( uint32_t i = 0; i != _count; ++i )
                {
                    const xmlsax_char_t * key = _keys[i];
                    const xmlsax_char_t * value = _values[i];

                    py_attr[key] = value;
                }

                pybind::object py_cb_attr = desc->cb.get_attr( "attr" );

                py_cb_attr.call( py_attr );
            }
            //////////////////////////////////////////////////////////////////////////
            static void python_callback_end_node( const xmlsax_char_t * _node, void * _userdata )
            {
                MENGINE_UNUSED( _node );

                python_xmlsax_desc * desc = (python_xmlsax_desc *)_userdata;

                pybind::object py_cb_end = desc->cb.get_attr( "end" );

                py_cb_end.call();
            }
            //////////////////////////////////////////////////////////////////////////
            bool s_parseXml( pybind::kernel_interface * _kernel, const ConstString & _fileGroupName, const FilePath & _filePath, const pybind::object & _cb )
            {
                const FileGroupInterfacePtr & fileGroup = FILE_SERVICE()
                    ->getFileGroup( _fileGroupName );

                MENGINE_ASSERTION_MEMORY_PANIC( fileGroup, "not found file group '%s'"
                    , _fileGroupName.c_str()
                );

                MemoryInterfacePtr binary_buffer = Helper::createMemoryCacheFileString( fileGroup, _filePath, false, false, MENGINE_DOCUMENT_PYBIND );

                MENGINE_ASSERTION_MEMORY_PANIC( binary_buffer, "not found file '%s'"
                    , _filePath.c_str()
                );

                Char * memory = binary_buffer->getBuffer();

                xmlsax_callbacks_t cb;
                cb.begin_node = &python_callback_begin_node;
                cb.node_attributes = &python_callback_node_attributes;
                cb.end_node = &python_callback_end_node;

                python_xmlsax_desc desc;
                desc.kernel = _kernel;
                desc.cb = _cb;

                if( xmlsax_parse( memory, &cb, &desc ) == false )
                {
                    return false;
                }

                return true;
            }
            //////////////////////////////////////////////////////////////////////////
            void s_visitFonts( const pybind::object & _cb )
            {
                FONT_SERVICE()
                    ->foreachFonts( [_cb]( const FontInterfacePtr & _font )
                {
                    const ConstString & name = _font->getName();

                    _cb.call( name );
                } );
            }
            //////////////////////////////////////////////////////////////////////////
            pybind::list s_getFonts( pybind::kernel_interface * _kernel )
            {
                pybind::list l( _kernel );

                FONT_SERVICE()
                    ->foreachFonts( [&l]( const FontInterfacePtr & _font )
                {
                    const ConstString & name = _font->getName();

                    l.append( name );
                } );

                return l;
            }
            //////////////////////////////////////////////////////////////////////////
            bool s_hasFont( const ConstString & _fontName )
            {
                FontInterfacePtr font;
                bool has = FONT_SERVICE()
                    ->existFont( _fontName, &font );

                return has;
            }
            //////////////////////////////////////////////////////////////////////////
            bool s_validateFont( const ConstString & _fontName, const String & _text )
            {
                FontInterfacePtr font;
                if( FONT_SERVICE()
                    ->existFont( _fontName, &font ) == false )
                {
                    return false;
                }

                if( font->validateText( ConstString::none(), _text.c_str(), _text.size() ) == false )
                {
                    return false;
                }

                return true;
            }
            //////////////////////////////////////////////////////////////////////////
            ResourcePacketPtr s_cacheResources( const ConstString & _groupName )
            {
                ResourcePacketPtr resourcePacket = Helper::makeFactorableUnique<ResourcePacket>( MENGINE_DOCUMENT_PYBIND );

                RESOURCE_SERVICE()
                    ->foreachGroupResources( _groupName, [resourcePacket]( const ResourcePtr & _resource )
                {
                    if( _resource->isCompile() == false )
                    {
                        return;
                    }

                    resourcePacket->addResource( _resource );
                } );

                resourcePacket->cache();

                return resourcePacket;
            }
            //////////////////////////////////////////////////////////////////////////
            void s_uncacheResources( const ResourcePacketPtr & _resourcePacket )
            {
                _resourcePacket->uncache();
            }
            //////////////////////////////////////////////////////////////////////////
            uint32_t s_rotateToTrimetric( const mt::vec2f & _dir, const mt::vec2f & _vx, const mt::vec2f & _vy )
            {
                float dir_angle = mt::signed_angle( _dir );

                float isometric_angle8[8] = {
                    mt::signed_angle( +_vx ),
                    mt::signed_angle( +_vx + _vy ),
                    mt::signed_angle( +_vy ),
                    mt::signed_angle( +_vy - _vx ),
                    mt::signed_angle( -_vx ),
                    mt::signed_angle( -_vx - _vy ),
                    mt::signed_angle( -_vy ),
                    mt::signed_angle( +_vx - _vy )
                };

                float min_angle = mt::constant::two_pi;
                uint32_t isometric_index = 0;

                for( uint32_t i = 0; i != 8; ++i )
                {
                    float isometric_angle = isometric_angle8[i];

                    float angle_length = mt::angle_length( isometric_angle, dir_angle );

                    float abs_angle_length = StdMath::fabsf( angle_length );

                    if( abs_angle_length < min_angle )
                    {
                        min_angle = abs_angle_length;
                        isometric_index = i;
                    }
                }

                return isometric_index;
            }
            //////////////////////////////////////////////////////////////////////////
            uint32_t s_rotateToTrimetric2( const mt::vec2f & _from, const mt::vec2f & _to, const mt::vec2f & _vx, const mt::vec2f & _vy )
            {
                uint32_t trimetric = s_rotateToTrimetric( _to - _from, _vx, _vy );

                return trimetric;
            }
            //////////////////////////////////////////////////////////////////////////
            bool s_copyFile_( const ConstString & _resourceFilePath, const MemoryBufferInterfacePtr & _memory )
            {
                MENGINE_ASSERTION_RESOURCE_TYPE_BY_NAME( _resourceFilePath, ResourceFilePtr, false, "resource '%s' type does not match 'ResourceFile'"
                    , _resourceFilePath.c_str()
                );

                const ResourceFilePtr & resource = RESOURCE_SERVICE()
                    ->getResourceReference( ConstString::none(), _resourceFilePath );

                MENGINE_ASSERTION_MEMORY_PANIC( resource, "resource '%s' not found"
                    , _resourceFilePath.c_str()
                );

                if( resource->compile() == false )
                {
                    LOGGER_ERROR( "resource '%s' type '%s' invalid compile"
                        , resource->getName().c_str()
                        , resource->getType().c_str()
                    );

                    return false;
                }

                const ContentInterfacePtr & content = resource->getContent();

                InputStreamInterfacePtr stream = content->openInputStreamFile( false, false, MENGINE_DOCUMENT_PYBIND );

                if( stream == nullptr )
                {
                    resource->release();

                    return false;
                }

                size_t size = stream->size();

                void * memory_buffer = _memory->newBuffer( size );

                if( stream->read( memory_buffer, size ) != size )
                {
                    resource->release();

                    return false;
                }

                resource->release();

                return true;
            }
            //////////////////////////////////////////////////////////////////////////
            bool s_updateUserWallpaper( const String & _filePath )
            {
                Char projectName[MENGINE_APPLICATION_PROJECT_MAXNAME + 1] = {'\0'};
                APPLICATION_SERVICE()
                    ->getProjectName( projectName );

                StdString::strchrcat( projectName, MENGINE_PATH_DELIM_BACKSLASH );

                if( PLATFORM_SERVICE()
                    ->updateDesktopWallpaper( projectName, _filePath.c_str() ) == false )
                {
                    return false;
                }

                return true;
            }
            //////////////////////////////////////////////////////////////////////////
            bool s_copyUserPicture( const ConstString & _resourceFilePath, const String & _filePath )
            {
                MemoryBufferInterfacePtr memory = MEMORY_SERVICE()
                    ->createMemoryBuffer( MENGINE_DOCUMENT_PYBIND );

                MENGINE_ASSERTION_MEMORY_PANIC( memory, "invalid create memory buffer" );

                if( this->s_copyFile_( _resourceFilePath, memory ) == false )
                {
                    return false;
                }

                Char projectName[MENGINE_APPLICATION_PROJECT_MAXNAME + 1] = {'\0'};
                APPLICATION_SERVICE()
                    ->getProjectName( projectName );

                StdString::strchrcat( projectName, MENGINE_PATH_DELIM_BACKSLASH );

                const void * memoryBuffer = memory->getBuffer();
                size_t memorySize = memory->getSize();

                if( PLATFORM_SERVICE()
                    ->createDirectoryUserPicture( projectName, _filePath.c_str(), memoryBuffer, memorySize ) == false )
                {
                    return false;
                }

                return true;
            }
            //////////////////////////////////////////////////////////////////////////
            bool s_copyUserMusic( const ConstString & _resourceFilePath, const String & _filePath )
            {
                MemoryBufferInterfacePtr memory = MEMORY_SERVICE()
                    ->createMemoryBuffer( MENGINE_DOCUMENT_PYBIND );

                MENGINE_ASSERTION_MEMORY_PANIC( memory, "invalid create memory buffer" );

                if( this->s_copyFile_( _resourceFilePath, memory ) == false )
                {
                    return false;
                }

                Char projectName[MENGINE_APPLICATION_PROJECT_MAXNAME + 1] = {'\0'};
                APPLICATION_SERVICE()
                    ->getProjectName( projectName );

                StdString::strchrcat( projectName, MENGINE_PATH_DELIM_BACKSLASH );

                const void * memoryBuffer = memory->getBuffer();
                size_t memorySize = memory->getSize();

                if( PLATFORM_SERVICE()
                    ->createDirectoryUserMusic( projectName, _filePath.c_str(), memoryBuffer, memorySize ) == false )
                {
                    return false;
                }

                return true;
            }
            //////////////////////////////////////////////////////////////////////////
            class PyInputMousePositionProvider
                : public InputMousePositionProviderInterface
                , public PythonCallbackProvider
                , public Factorable
            {
                DECLARE_FACTORABLE( PyInputMousePositionProvider );

            public:
                PyInputMousePositionProvider()
                    : m_renderCamera( nullptr )
                    , m_renderViewport( nullptr )
                {
                }

            public:
                void initialize( const RenderCameraInterfacePtr & _camera, const RenderViewportInterfacePtr & _viewport, const pybind::object & _cb, const pybind::args & _args )
                {
                    m_renderCamera = _camera;
                    m_renderViewport = _viewport;

                    PythonCallbackProvider::initialize( _cb, _args );
                }

            protected:
                void onMousePositionChange( ETouchCode _touchId, const mt::vec2f & _position, float _pressure ) override
                {
                    MENGINE_UNUSED( _pressure );

                    RenderContext context;
                    Helper::clearRenderContext( &context );

                    context.camera = m_renderCamera.get();
                    context.viewport = m_renderViewport.get();

                    mt::vec2f wp;
                    ARROW_SERVICE()
                        ->calcMouseWorldPosition( &context, _position, &wp );

                    mt::vec3f v3( wp.x, wp.y, 0.f );

                    this->call_cb( _touchId, v3 );
                }

            protected:
                RenderCameraInterfacePtr m_renderCamera;
                RenderViewportInterfacePtr m_renderViewport;
            };
            //////////////////////////////////////////////////////////////////////////
            typedef IntrusivePtr<PyInputMousePositionProvider, InputMousePositionProviderInterface> PyInputMousePositionProviderPtr;
            //////////////////////////////////////////////////////////////////////////
            FactoryInterfacePtr m_factoryPyInputMousePositionProvider;
            //////////////////////////////////////////////////////////////////////////
            InputMousePositionProviderInterfacePtr s_addMousePositionProvider( const RenderCameraInterfacePtr & _camera, const RenderViewportInterfacePtr & _viewport, const pybind::object & _cb, const pybind::args & _args )
            {
                RenderCameraInterfacePtr camera = _camera;
                if( camera == nullptr )
                {
                    camera = PLAYER_SERVICE()
                        ->getRenderCamera();
                }

                RenderViewportInterfacePtr viewport = _viewport;
                if( viewport == nullptr )
                {
                    viewport = PLAYER_SERVICE()
                        ->getRenderViewport();
                }

                PyInputMousePositionProviderPtr provider = m_factoryPyInputMousePositionProvider->createObject( MENGINE_DOCUMENT_PYBIND );
                provider->initialize( camera, viewport, _cb, _args );

                INPUT_SERVICE()
                    ->addMousePositionProvider( provider, MENGINE_DOCUMENT_PYBIND );

                return provider;
            }
            //////////////////////////////////////////////////////////////////////////
            void s_removeMousePositionProvider( const InputMousePositionProviderInterfacePtr & _provider )
            {
                INPUT_SERVICE()
                    ->removeMousePositionProvider( _provider );
            }
            //////////////////////////////////////////////////////////////////////////
            mt::vec2f s_screenToWorldPoint( const RenderCameraInterfacePtr & _camera, const RenderViewportInterfacePtr & _viewport, const mt::vec2f & _screenPoint )
            {
                RenderCameraInterfacePtr camera = _camera;

                if( camera == nullptr )
                {
                    camera = PLAYER_SERVICE()
                        ->getRenderCamera();
                }

                RenderViewportInterfacePtr viewport = _viewport;
                if( viewport == nullptr )
                {
                    viewport = PLAYER_SERVICE()
                        ->getRenderViewport();
                }

                RenderContext context;
                Helper::clearRenderContext( &context );

                context.camera = camera.get();
                context.viewport = viewport.get();

                mt::vec2f wp;
                ARROW_SERVICE()
                    ->calcMouseWorldPosition( &context, _screenPoint, &wp );

                return wp;
            }
            //////////////////////////////////////////////////////////////////////////
            mt::vec2f s_screenToWorldClick( const RenderCameraInterfacePtr & _camera, const RenderViewportInterfacePtr & _viewport, const mt::vec2f & _screenPoint )
            {
                RenderCameraInterfacePtr camera = _camera;
                if( camera == nullptr )
                {
                    camera = PLAYER_SERVICE()
                        ->getRenderCamera();
                }

                RenderViewportInterfacePtr viewport = _viewport;
                if( viewport == nullptr )
                {
                    viewport = PLAYER_SERVICE()
                        ->getRenderViewport();
                }

                RenderContext context;
                Helper::clearRenderContext( &context );

                context.camera = camera.get();
                context.viewport = viewport.get();

                mt::vec2f wp;
                ARROW_SERVICE()
                    ->calcPointClick( &context, _screenPoint, &wp );

                return wp;
            }
            //////////////////////////////////////////////////////////////////////////
            class AffectorUIWheelOfFortune
                : public Affector
            {
                DECLARE_FACTORABLE( AffectorUIWheelOfFortune );

            public:
                AffectorUIWheelOfFortune()
                    : m_coeff( 1.f )
                {
                }

            public:
                void initialize( const NodePtr & _node, const NodePtr & _target, float _coeff )
                {
                    m_node = _node;
                    m_target = _target;

                    m_coeff = _coeff;
                }

            protected:
                bool _affect( const UpdateContext * _context, float * const _used ) override
                {
                    *_used = _context->time;

                    mt::vec2f sp_node;
                    Helper::getNodeScreenPosition( m_node.get(), &sp_node );

                    mt::vec2f sp_target;
                    Helper::getNodeScreenPosition( m_target.get(), &sp_target );

                    float distance = mt::length_v2_v2( sp_target, sp_node );

                    float scale = (1.f - distance) * m_coeff;

                    if( scale <= 0.f )
                    {
                        scale = 0.f;
                    }

                    TransformationInterface * transformation = m_node->getTransformation();

                    transformation->setLocalScale( mt::vec3f( scale, scale, 1.f ) );

                    return false;
                }

            protected:
                NodePtr m_node;
                NodePtr m_target;

                float m_coeff;
            };
            //////////////////////////////////////////////////////////////////////////
            typedef IntrusivePtr<AffectorUIWheelOfFortune, Affector> AffectorUIWheelOfFortunePtr;
            //////////////////////////////////////////////////////////////////////////
            FactoryInterfacePtr m_factoryAffectorUIWheelOfFortune;
            //////////////////////////////////////////////////////////////////////////
            uint32_t s_affectorUIWheelOfFortune( const NodePtr & _node, const NodePtr & _targer, float _coeff )
            {
                AffectorUIWheelOfFortunePtr affector = m_factoryAffectorUIWheelOfFortune->createObject( MENGINE_DOCUMENT_PYBIND );

                affector->setAffectorType( EAFFECTORTYPE_SCALE );

                affector->initialize( _node, _targer, _coeff );

                const AffectorHubInterfacePtr & affectorHub = _node->getAffectorHub();

                uint32_t id = affectorHub->addAffector( affector );

                return id;
            }
            //////////////////////////////////////////////////////////////////////////
            class AffectorGridBurnTransparency
                : public Affector
            {
                DECLARE_FACTORABLE( AffectorGridBurnTransparency );

            public:
                AffectorGridBurnTransparency()
                    : m_position( 0.f, 0.f )
                    , m_duration( 0.f )
                    , m_radius( 0.f )
                    , m_ellipse( 0.f )
                    , m_penumbra( 0.f )
                    , m_time( 0.f )
                {
                }

            public:
                void initialize( const Grid2DPtr & _grid, const mt::vec2f & _position, float _duration, float _radius, float _ellipse, float _penumbra, const pybind::object & _cb )
                {
                    m_grid = _grid;
                    m_position = _position;
                    m_duration = _duration;
                    m_radius = _radius;
                    m_ellipse = _ellipse;
                    m_penumbra = _penumbra;
                    m_cb = _cb;
                }

            protected:
                bool _affect( const UpdateContext * _context, float * const _used ) override
                {
                    *_used = _context->time;

                    m_time += _context->time;

                    bool complete = false;
                    if( m_time + _context->time > m_duration )
                    {
                        m_time = m_duration;
                        complete = true;
                    }

                    float a1 = m_time / m_duration;

                    float radius = m_radius * a1;
                    float penumbra = m_penumbra * a1;

                    float width = m_grid->getWidth();
                    float height = m_grid->getHeight();

                    uint32_t countX = m_grid->getCountX();
                    uint32_t countY = m_grid->getCountY();

                    float position_step_x = width / float( countX - 1 );
                    float position_step_y = height / float( countY - 1 );

                    for( uint32_t j = 0; j != countY; ++j )
                    {
                        for( uint32_t i = 0; i != countX; ++i )
                        {
                            mt::vec2f grid_position( position_step_x * i, position_step_y * j );

                            mt::vec2f v = m_position - grid_position;

                            v.y /= m_ellipse;

                            float position_sqrdistance = v.sqrlength();

                            if( position_sqrdistance < radius * radius )
                            {
                                m_grid->setGridColor( i, j, Helper::makeColorARGB( 0x00FFFFFF ) );
                            }
                            else if( position_sqrdistance < (radius + penumbra) * (radius + penumbra) )
                            {
                                Color cv;
                                m_grid->getGridColor( i, j, &cv );

                                float cv_a = cv.getA();

                                float pos_distance = StdMath::sqrtf( position_sqrdistance );

                                float a0 = (pos_distance - radius) / penumbra;

                                if( cv_a < a0 )
                                {
                                    continue;
                                }

                                m_grid->setGridColor( i, j, Color( 1.f, 1.f, 1.f, a0 ) );
                            }
                        }
                    }

                    return complete;
                }

                void _complete( bool _isEnd ) override
                {
                    if( m_cb.is_callable() == true )
                    {
                        m_cb.call( _isEnd );
                    }
                }

            protected:
                Grid2DPtr m_grid;

                mt::vec2f m_position;
                float m_duration;
                float m_radius;
                float m_ellipse;
                float m_penumbra;

                pybind::object m_cb;

                float m_time;
            };
            //////////////////////////////////////////////////////////////////////////
            typedef IntrusivePtr<AffectorGridBurnTransparency, Affector> AffectorGridBurnTransparencyPtr;
            //////////////////////////////////////////////////////////////////////////
            FactoryInterfacePtr m_factoryAffectorGridBurnTransparency;
            //////////////////////////////////////////////////////////////////////////
            uint32_t s_gridBurnTransparency( const Grid2DPtr & _grid, const mt::vec2f & _pos, float _time, float _radius, float _ellipse, float _penumbra, const pybind::object & _cb )
            {
                AffectorGridBurnTransparencyPtr affector = m_factoryAffectorGridBurnTransparency->createObject( MENGINE_DOCUMENT_PYBIND );

                affector->setAffectorType( EAFFECTORTYPE_USER );

                affector->initialize( _grid, _pos, _time, _radius, _ellipse, _penumbra, _cb );

                const AffectorHubInterfacePtr & affectorHub = _grid->getAffectorHub();

                uint32_t id = affectorHub->addAffector( affector );

                return id;
            }
            //////////////////////////////////////////////////////////////////////////
            class AffectorUser
                : public Affector
                , public PythonCallbackProvider
            {
                DECLARE_FACTORABLE( AffectorUser );

            protected:
                bool _affect( const UpdateContext * _context, float * const _used ) override
                {
                    *_used = _context->time;

                    bool complete = this->call_cb( _context->time );

                    return complete;
                }
            };
            //////////////////////////////////////////////////////////////////////////
            typedef IntrusivePtr<AffectorUser, Affector> AffectorUserPtr;
            //////////////////////////////////////////////////////////////////////////
            FactoryInterfacePtr m_factoryAffectorUser;
            //////////////////////////////////////////////////////////////////////////
            AffectorPtr s_createAffector( const pybind::object & _cb, const pybind::args & _args )
            {
                AffectorUserPtr affector = m_factoryAffectorUser->createObject( MENGINE_DOCUMENT_PYBIND );

                affector->initialize( _cb, _args );

                return affector;
            }
            //////////////////////////////////////////////////////////////////////////
            UniqueId s_addAffector( const pybind::object & _cb, const pybind::args & _args )
            {
                AffectorUserPtr affector = m_factoryAffectorUser->createObject( MENGINE_DOCUMENT_PYBIND );

                affector->initialize( _cb, _args );

                const AffectorHubInterfacePtr & affectorHub = PLAYER_SERVICE()
                    ->getGlobalAffectorHub();

                UniqueId id = affectorHub->addAffector( affector );

                return id;
            }
            //////////////////////////////////////////////////////////////////////////
            bool s_removeAffector( UniqueId _id )
            {
                const AffectorHubInterfacePtr & affectorHub = PLAYER_SERVICE()
                    ->getGlobalAffectorHub();

                bool successful = affectorHub->stopAffector( _id );

                return successful;
            }
            //////////////////////////////////////////////////////////////////////////
            class AffectorFollower
                : public Affector
            {
            public:
                virtual void follow( const pybind::object & _target ) = 0;

            protected:
                PyObject * getEmbed( pybind::kernel_interface * _kernel )
                {
                    PyObject * py_obj = _kernel->scope_create_holder_t( this );

                    return py_obj;
                }
            };
            //////////////////////////////////////////////////////////////////////////
            typedef IntrusivePtr<AffectorFollower, Affector> AffectorFollowerPtr;
            //////////////////////////////////////////////////////////////////////////
            class AffectorNodeFollower
                : public AffectorFollower
            {
            public:
                AffectorNodeFollower()
                {
                }

                ~AffectorNodeFollower() override
                {
                }

            public:
                void setAffectorable( const AffectorablePtr & _affectorable )
                {
                    m_affectorable = _affectorable;
                }

                const AffectorablePtr & getAffectorable() const
                {
                    return m_affectorable;
                }

            protected:
                void _stop() override
                {
                    UniqueId id = this->getId();

                    const AffectorHubInterfacePtr & affectorHub = m_affectorable->getAffectorHub();

                    affectorHub->stopAffector( id );
                }

            protected:
                AffectorablePtr m_affectorable;
            };
            //////////////////////////////////////////////////////////////////////////
            template<class T_Value, class T_Setter, class T_Getter>
            class AffectorNodeFollowerMethod
                : public AffectorNodeFollower
            {
                DECLARE_FACTORABLE( AffectorNodeFollowerMethod );

            public:
                AffectorNodeFollowerMethod()
                {
                }

                ~AffectorNodeFollowerMethod() override
                {
                }

            public:
                bool initialize( const T_Setter & _setter, const T_Getter & _getter, const T_Value & _value, const T_Value & _target, float _speed )
                {
                    m_setter = _setter;
                    m_getter = _getter;

                    m_follower.setValue( _value );
                    m_follower.setSpeed( _speed );

                    m_follower.setFollow( _target );

                    m_setter( _value );

                    return true;
                }

            public:
                void follow( const pybind::object & _target ) override
                {
                    T_Value value_target = _target.extract();

                    m_follower.setFollow( value_target );
                }

            protected:
                bool _affect( const UpdateContext * _context, float * const _used ) override
                {
                    T_Value current_value = m_getter();

                    m_follower.setValue( current_value );

                    m_follower.update( _context, _used );

                    T_Value value = m_follower.getValue();

                    m_setter( value );

                    return false;
                }

            protected:
                ValueFollowerLinear<T_Value> m_follower;

                T_Setter m_setter;
                T_Getter m_getter;
            };
            //////////////////////////////////////////////////////////////////////////
            template<class T_Node, class T_Value>
            class AffectorNodeFollowerCreator
                : public Factorable
            {
                DECLARE_FACTORABLE( AffectorNodeFollowerCreator );

            protected:
                typedef Lambda<void( const T_Value & )> T_Setter;
                typedef Lambda<T_Value()> T_Getter;

                typedef AffectorNodeFollowerMethod<T_Value, T_Setter, T_Getter> TAffectorNodeFollowerMethod;
                typedef IntrusivePtr<TAffectorNodeFollowerMethod> TAffectorNodeFollowerMethodPtr;

            public:
                AffectorNodeFollowerCreator()
                {
                }

                ~AffectorNodeFollowerCreator() override
                {
                }

            public:
                bool initialize()
                {
                    m_affectorFactory = Helper::makeFactoryPool<TAffectorNodeFollowerMethod, 4>( MENGINE_DOCUMENT_FACTORABLE );

                    return true;
                }

                void finalize()
                {
                    MENGINE_ASSERTION_FACTORY_EMPTY( m_affectorFactory );
                    m_affectorFactory = nullptr;
                }

            public:
                AffectorFollowerPtr create( const AffectorablePtr & _affectorable, const T_Setter & _setter, const T_Getter & _getter, const T_Value & _value, const T_Value & _target, float _speed, const DocumentInterfacePtr & _doc )
                {
                    TAffectorNodeFollowerMethodPtr affector = m_affectorFactory->createObject( _doc );

                    affector->setAffectorable( _affectorable );

                    if( affector->initialize( _setter, _getter, _value, _target, _speed ) == false )
                    {
                        return nullptr;
                    }

                    const AffectorHubInterfacePtr & affectorHub = _affectorable->getAffectorHub();

                    if( affectorHub->addAffector( affector ) == INVALID_UNIQUE_ID )
                    {
                        return nullptr;
                    }

                    return affector;
                }

            protected:
                FactoryInterfacePtr m_affectorFactory;
            };
            //////////////////////////////////////////////////////////////////////////
            IntrusivePtr<AffectorNodeFollowerCreator<Node, float>> m_creatorAffectorNodeFollowerLocalAlpha;
            //////////////////////////////////////////////////////////////////////////
            AffectorFollowerPtr s_addNodeFollowerLocalAlpha( const NodePtr & _node, float _value, float _target, float _speed )
            {
                RenderInterface * render = _node->getRender();

                MENGINE_ASSERTION_MEMORY_PANIC( render, "node '%s' is not renderable"
                    , _node->getName().c_str()
                );

                AffectorFollowerPtr affector = m_creatorAffectorNodeFollowerLocalAlpha->create( _node
                    , [render]( float _alpha )
                {
                    render->setLocalAlpha( _alpha );
                }
                    , [render]()
                {
                    return render->getLocalAlpha();
                }
                    , _value, _target, _speed
                    , MENGINE_DOCUMENT_PYBIND );

                return affector;
            }
            //////////////////////////////////////////////////////////////////////////
            IntrusivePtr<AffectorNodeFollowerCreator<ShapeQuadFlex, mt::vec2f>> m_creatorAffectorNodeFollowerCustomSize;
            //////////////////////////////////////////////////////////////////////////
            AffectorFollowerPtr s_addShapeFollowerCustomSize( const ShapeQuadFlexPtr & _node, const mt::vec2f & _value, const mt::vec2f & _target, float _speed )
            {
                AffectorFollowerPtr affector = m_creatorAffectorNodeFollowerCustomSize->create( _node
                    , [_node]( const mt::vec2f & _value )
                {
                    _node->setCustomSize( _value );
                }
                    , [_node]()
                {
                    return _node->getCustomSize();
                }
                    , _value, _target, _speed
                    , MENGINE_DOCUMENT_PYBIND );

                return affector;
            }
            //////////////////////////////////////////////////////////////////////////
            IntrusivePtr<AffectorNodeFollowerCreator<ShapeQuadFlex, mt::vec2f>> m_creatorAffectorNodeFollowerTextureUVScale;
            //////////////////////////////////////////////////////////////////////////
            AffectorFollowerPtr s_addShapeFollowerTextureUVScale( const ShapeQuadFlexPtr & _node, const mt::vec2f & _value, const mt::vec2f & _target, float _speed )
            {
                AffectorFollowerPtr affector = m_creatorAffectorNodeFollowerTextureUVScale->create( _node
                    , [_node]( const mt::vec2f & _value )
                {
                    _node->setTextureUVScale( _value );
                }
                    , [_node]()
                {
                    return _node->getTextureUVScale();
                }
                    , _value, _target, _speed
                    , MENGINE_DOCUMENT_PYBIND );

                return affector;
            }
            //////////////////////////////////////////////////////////////////////////
            void s_removeNodeFollower( const AffectorFollowerPtr & _affector )
            {
                MENGINE_ASSERTION_MEMORY_PANIC( _affector, "take NULL affector" );

                _affector->stop();
            }
            //////////////////////////////////////////////////////////////////////////
            mt::vec3f s_getCameraPosition( const RenderCameraInterfacePtr & _camera, const NodePtr & _node )
            {
                const TransformationInterface * transformation = _node->getTransformation();

                const mt::vec3f & wp = transformation->getWorldPosition();

                const mt::mat4f & vm = _camera->getCameraViewMatrix();

                mt::vec3f wp_screen;
                mt::mul_v3_v3_m4( &wp_screen, wp, vm );

                return wp_screen;
            }
            //////////////////////////////////////////////////////////////////////////
            static WChar s_InputTextEvent_getSymbol( InputTextEvent * _event )
            {
                WChar symbol = _event->text[0];

                return symbol;
            }
            //////////////////////////////////////////////////////////////////////////
            static const WChar * s_InputTextEvent_getText( InputTextEvent * _event )
            {
                const WChar * text = _event->text;

                return text;
            }
            //////////////////////////////////////////////////////////////////////////
            PythonValueFollowerLinearPtr s_createValueFollowerLinear( float _value, float _speed, const pybind::object & _cb, const pybind::args & _args )
            {
                PythonValueFollowerLinearPtr follower = Helper::generateAffectorFactorable<PythonValueFollowerLinear>( MENGINE_DOCUMENT_PYBIND );

                follower->initialize( _value, _speed, _cb, _args );

                const AffectorHubInterfacePtr & affectorHub = PLAYER_SERVICE()
                    ->getGlobalAffectorHub();

                affectorHub->addAffector( follower );

                return follower;
            }
            //////////////////////////////////////////////////////////////////////////
            PythonValueFollowerAccelerationPtr s_createValueFollowerAcceleration( float _value, float _speed, float _acceleration, const pybind::object & _cb, const pybind::args & _args )
            {
                PythonValueFollowerAccelerationPtr follower = Helper::generateAffectorFactorable<PythonValueFollowerAcceleration>( MENGINE_DOCUMENT_PYBIND );

                follower->initialize( _value, _speed, _acceleration, _cb, _args );

                const AffectorHubInterfacePtr & affectorHub = PLAYER_SERVICE()
                    ->getGlobalAffectorHub();

                affectorHub->addAffector( follower );

                return follower;
            }
            //////////////////////////////////////////////////////////////////////////
            void s_destroyValueFollower( const PythonValueFollowerPtr & _follower )
            {
                UniqueId id = _follower->getId();

                const AffectorHubInterfacePtr & affectorHub = PLAYER_SERVICE()
                    ->getGlobalAffectorHub();

                affectorHub->stopAffector( id );
            }
            //////////////////////////////////////////////////////////////////////////
            String s_SecureValueInterface_saveHexadecimal( const SecureValueInterface * _secure )
            {
                String hexadecimal;
                _secure->saveHexadecimal( &hexadecimal );

                return hexadecimal;
            }
            //////////////////////////////////////////////////////////////////////////
            bool s_SecureValueInterface_loadHexadecimal( SecureValueInterface * _secure, const String & _hexadecimal )
            {
                bool result = _secure->loadHexadecimal( _hexadecimal );

                return result;
            }
            //////////////////////////////////////////////////////////////////////////
            pybind::tuple s_SecureUnsignedValue_getUnprotectedValue( pybind::kernel_interface * _kernel, const SecureUnsignedValue * _secure )
            {
                uint32_t unprotected_value;
                if( _secure->getUnprotectedValue( &unprotected_value ) == false )
                {
                    return pybind::make_tuple_t( _kernel, false, 0 );
                }

                return pybind::make_tuple_t( _kernel, true, unprotected_value );
            }
            //////////////////////////////////////////////////////////////////////////
            pybind::tuple s_SecureUnsignedValue_cmpSecureValue( pybind::kernel_interface * _kernel, const SecureUnsignedValue * _secure, const SecureUnsignedValuePtr & _test )
            {
                int32_t result;
                if( _secure->cmpSecureValue( _test, &result ) == false )
                {
                    return pybind::make_tuple_t( _kernel, false, 0 );
                }

                return pybind::make_tuple_t( _kernel, true, result );
            }
            //////////////////////////////////////////////////////////////////////////
            SecureUnsignedValuePtr s_makeSecureUnsignedValue( uint32_t _value )
            {
                SecureUnsignedValuePtr secureValue = PROTOTYPE_SERVICE()
                    ->generatePrototype( ConstString::none(), SecureUnsignedValue::getFactorableType(), MENGINE_DOCUMENT_PYBIND );

                MENGINE_ASSERTION_MEMORY_PANIC( secureValue, "invalid create secure value" );

                secureValue->setUnprotectedValue( _value );

                return secureValue;
            }
            //////////////////////////////////////////////////////////////////////////
            pybind::tuple s_SecureStringValue_getUnprotectedValue( pybind::kernel_interface * _kernel, const SecureStringValue * _secure )
            {
                String unprotected_value;
                if( _secure->getUnprotectedValue( &unprotected_value ) == false )
                {
                    return pybind::make_tuple_t( _kernel, false, "" );
                }

                return pybind::make_tuple_t( _kernel, true, unprotected_value );
            }
            //////////////////////////////////////////////////////////////////////////
            pybind::tuple s_SecureStringValue_cmpSecureValue( pybind::kernel_interface * _kernel, const SecureStringValue * _secure, const SecureStringValuePtr & _test )
            {
                int32_t result;
                if( _secure->cmpSecureValue( _test, &result ) == false )
                {
                    return pybind::make_tuple_t( _kernel, false, 0 );
                }

                return pybind::make_tuple_t( _kernel, true, result );
            }
            //////////////////////////////////////////////////////////////////////////
            SecureStringValuePtr s_makeSecureStringValue( const String & _value )
            {
                SecureStringValuePtr secureValue = PROTOTYPE_SERVICE()
                    ->generatePrototype( ConstString::none(), SecureStringValue::getFactorableType(), MENGINE_DOCUMENT_PYBIND );

                MENGINE_ASSERTION_MEMORY_PANIC( secureValue, "invalid create secure value" );

                secureValue->setUnprotectedValue( _value );

                return secureValue;
            }
            //////////////////////////////////////////////////////////////////////////
            PythonFileLoggerPtr s_makeFileLogger( const FilePath & _filePath )
            {
                PythonFileLoggerPtr logger = Helper::makeFactorableUnique<PythonFileLogger>( MENGINE_DOCUMENT_PYBIND );

                const FileGroupInterfacePtr & userFileGroup = FILE_SERVICE()
                    ->getFileGroup( STRINGIZE_STRING_LOCAL( "user" ) );

                MENGINE_ASSERTION_MEMORY_PANIC( userFileGroup, "invalid get user file group" );

                ContentInterfacePtr content = Helper::makeFileContent( userFileGroup, _filePath, MENGINE_DOCUMENT_PYBIND );

                MENGINE_ASSERTION_MEMORY_PANIC( content, "invalid make file content" );

                if( logger->initialize( content ) == false )
                {
                    return nullptr;
                }

                return logger;
            }
            //////////////////////////////////////////////////////////////////////////
            pybind::dict s_getSettings( pybind::kernel_interface * _kernel )
            {
                pybind::dict py_settings( _kernel );

                SETTINGS_SERVICE()
                    ->foreachSettings( [_kernel, &py_settings]( const ConstString & _name, const SettingInterfacePtr & _setting )
                {
                    pybind::dict py_setting( _kernel );

                    _setting->foreachKeys( [_kernel, _setting, &py_setting]( const Char * _key )
                    {
                        ESettingType type = _setting->getValueType( _key );

                        switch( type )
                        {
                        case EST_NONE:
                            {
                                py_setting[_key] = _kernel->ret_none();
                            }break;
                        case EST_BOOL:
                            {
                                bool value = _setting->getValueBoolean( _key, false );

                                py_setting[_key] = value;
                            }break;
                        case EST_INTEGER:
                            {
                                int64_t value = _setting->getValueInteger( _key, 0L );

                                py_setting[_key] = value;
                            }break;
                        case EST_REAL:
                            {
                                double value = _setting->getValueFloat( _key, 0.0 );

                                py_setting[_key] = value;
                            }break;
                        case EST_STRING:
                            {
                                const Char * value = _setting->getValueString( _key, "" );

                                py_setting[_key] = value;
                            }break;
                        case EST_VEC2F:
                            {
                                mt::vec2f value = _setting->getValueVec2f( _key, mt::vec2f::identity() );

                                py_setting[_key] = value;
                            }break;
                        case EST_VEC3F:
                            {
                                mt::vec3f value = _setting->getValueVec3f( _key, mt::vec3f::identity() );
                                py_setting[_key] = value;
                            }break;
                        case EST_COLOR:
                            {
                                Color value = _setting->getValueColor( _key, Color::identity() );

                                py_setting[_key] = value;
                            }break;
                        }
                    } );

                    py_settings[_name] = py_setting;
                } );

                return py_settings;
            }
            //////////////////////////////////////////////////////////////////////////
            void s_setAvailablePlugin( const ConstString & _pluginName, bool _available )
            {
                PLUGIN_SERVICE()
                    ->setAvailablePlugin( _pluginName, _available );
            }
            //////////////////////////////////////////////////////////////////////////
            bool s_isAvailablePlugin( const ConstString & _pluginName )
            {
                bool avalilable = PLUGIN_SERVICE()
                    ->isAvailablePlugin( _pluginName );

                return avalilable;
            }
            //////////////////////////////////////////////////////////////////////////
            Scene * s_findNodeScene( Node * _node )
            {
                Node * node_iterator = _node;

                while( node_iterator != nullptr )
                {
                    Scene * node_scene = Helper::dynamicCast<Scene *>( node_iterator );

                    if( node_scene != nullptr )
                    {
                        return node_scene;
                    }

                    node_iterator = node_iterator->getParent();
                }

                return nullptr;
            }
            //////////////////////////////////////////////////////////////////////////
            void s_setLocale( const ConstString & _locale )
            {
                APPLICATION_SERVICE()
                    ->setLocale( _locale );
            }
            //////////////////////////////////////////////////////////////////////////
            mt::vec2f s_getCursorPosition()
            {
                mt::vec2f wp = this->s_getTouchPosition( TC_TOUCH0 );

                return wp;
            }
            //////////////////////////////////////////////////////////////////////////
            mt::vec2f s_getMousePosition()
            {
                mt::vec2f wp = this->s_getTouchPosition( TC_TOUCH0 );

                return wp;
            }
            //////////////////////////////////////////////////////////////////////////
            mt::vec2f s_getTouchPosition( ETouchCode _touchId )
            {
                const mt::vec2f & pos = INPUT_SERVICE()
                    ->getCursorPosition( _touchId );

                mt::vec2f wp;
                PLAYER_SERVICE()
                    ->calcGlobalMouseWorldPosition( pos, &wp );

                return wp;
            }
            //////////////////////////////////////////////////////////////////////////
            bool s_isInViewport( const mt::vec2f & _pos )
            {
                const RenderViewportInterfacePtr & renderViewport = PLAYER_SERVICE()
                    ->getRenderViewport();

                const Viewport & vp = renderViewport->getViewportWM();

                bool test = vp.testPoint( _pos );

                return test;
            }
            //////////////////////////////////////////////////////////////////////////
            void s_enableTextureFiltering( bool _enable )
            {
                MENGINE_UNUSED( _enable );

                //Empty
            }
            //////////////////////////////////////////////////////////////////////////
            bool s_isTextureFilteringEnabled()
            {
                return true;
            }
            //////////////////////////////////////////////////////////////////////////
            bool s_existText( const ConstString & _key )
            {
                bool exist = TEXT_SERVICE()
                    ->hasTextEntry( _key, nullptr );

                return exist;
            }
            //////////////////////////////////////////////////////////////////////////
            pybind::list s_pickHotspot( pybind::kernel_interface * _kernel, const mt::vec2f & _point )
            {
                InputSpecialData special;
                INPUT_SERVICE()
                    ->getSpecial( &special );

                VectorPickers pickers;
                PICKER_SERVICE()
                    ->pickTraps( _point, TC_TOUCH0, 0.f, special, &pickers );

                pybind::list pyret( _kernel );

                bool onFocus = APPLICATION_SERVICE()
                    ->isFocus();

                if( onFocus == false )
                {
                    return pyret;
                }

                for( const PickerInterfacePtr & picker : pickers )
                {
                    Scriptable * scriptable = picker->getPickerScriptable();

                    if( scriptable == nullptr )
                    {
                        continue;
                    }

                    pyret.append( scriptable );
                }

                return pyret;
            }
            //////////////////////////////////////////////////////////////////////////
            pybind::list s_pickAllHotspot( pybind::kernel_interface * _kernel, const mt::vec2f & _point )
            {
                VectorPickers pickers;
                PICKER_SERVICE()
                    ->getTraps( _point, &pickers );

                pybind::list pyret( _kernel );

                bool onFocus = APPLICATION_SERVICE()
                    ->isFocus();

                if( onFocus == false )
                {
                    return pyret;
                }

                for( const PickerInterfacePtr & picker : pickers )
                {
                    Scriptable * scriptable = picker->getPickerScriptable();

                    if( scriptable == nullptr )
                    {
                        continue;
                    }

                    pyret.append( scriptable );
                }

                return pyret;
            }
            //////////////////////////////////////////////////////////////////////////
            bool s_testPlatformTags( const Tags & _tags )
            {
                const Tags & platformTags = PLATFORM_SERVICE()
                    ->getPlatformTags();

                bool successful = platformTags.hasTags( _tags );

                return successful;
            }
            //////////////////////////////////////////////////////////////////////////
            bool s_hasPlatformTag( const ConstString & _tag )
            {
                bool successful = PLATFORM_SERVICE()
                    ->hasPlatformTag( _tag );

                return successful;
            }
            //////////////////////////////////////////////////////////////////////////
            bool s_hasTouchpad()
            {
                bool touchpad = PLATFORM_SERVICE()
                    ->hasTouchpad();

                return touchpad;
            }
            //////////////////////////////////////////////////////////////////////////
            const RenderCameraInterfacePtr & s_getRenderCamera2D()
            {
                const RenderCameraInterfacePtr & renderCamera = PLAYER_SERVICE()
                    ->getRenderCamera();

                return renderCamera;
            }
            //////////////////////////////////////////////////////////////////////////
            void s_showKeyboard()
            {
                PLATFORM_SERVICE()
                    ->showKeyboard();
            }
            //////////////////////////////////////////////////////////////////////////
            void s_hideKeyboard()
            {
                PLATFORM_SERVICE()
                    ->hideKeyboard();
            }
            //////////////////////////////////////////////////////////////////////////
            bool s_isShowKeyboard()
            {
                bool result = PLATFORM_SERVICE()
                    ->isShowKeyboard();

                return result;
            }
            //////////////////////////////////////////////////////////////////////////
            bool s_hasResource( const ConstString & _name )
            {
                bool result = RESOURCE_SERVICE()
                    ->hasResource( ConstString::none(), _name, false, nullptr );

                return result;
            }
            //////////////////////////////////////////////////////////////////////////
            bool s_removeCurrentScene( bool _immediately, const pybind::object & _cb, const pybind::args & _args )
            {
                if( SERVICE_IS_INITIALIZE( SceneServiceInterface ) == false )
                {
                    return false;
                }

                PythonSceneChangeCallbackPtr py_cb = m_factoryPythonSceneChangeCallback
                    ->createObject( MENGINE_DOCUMENT_PYBIND );

                py_cb->initialize( _cb, _args );

                if( SCENE_SERVICE()
                    ->removeCurrentScene( _immediately, py_cb ) == false )
                {
                    return false;
                }

                return true;
            }
            //////////////////////////////////////////////////////////////////////////
            class PyGlobalBaseHandler
                : public InputHandlerInterface
                , public PythonCallbackProvider
                , public Factorable
            {
            public:
                PyGlobalBaseHandler()
                {
                }

                ~PyGlobalBaseHandler() override
                {
                }

            protected:
                bool handleKeyEvent( const InputKeyEvent & _event ) override
                {
                    MENGINE_UNUSED( _event );

                    //Empty

                    return false;
                }

                bool handleTextEvent( const InputTextEvent & _event ) override
                {
                    MENGINE_UNUSED( _event );

                    //Empty

                    return false;
                }

            protected:
                bool handleAccelerometerEvent( const InputAccelerometerEvent & _event ) override
                {
                    MENGINE_UNUSED( _event );

                    //Empty

                    return false;
                }

            protected:
                bool handleMouseButtonEvent( const InputMouseButtonEvent & _event ) override
                {
                    MENGINE_UNUSED( _event );

                    //Empty

                    return false;
                }

                bool handleMouseButtonEventBegin( const InputMouseButtonEvent & _event ) override
                {
                    MENGINE_UNUSED( _event );

                    //Empty

                    return false;
                }

                bool handleMouseButtonEventEnd( const InputMouseButtonEvent & _event ) override
                {
                    MENGINE_UNUSED( _event );

                    //Empty

                    return false;
                }

                bool handleMouseMove( const InputMouseMoveEvent & _event ) override
                {
                    MENGINE_UNUSED( _event );

                    //Empty

                    return false;
                }

                bool handleMouseWheel( const InputMouseWheelEvent & _event ) override
                {
                    MENGINE_UNUSED( _event );

                    //Empty

                    return false;
                }

                bool handleMouseEnter( const InputMouseEnterEvent & _event ) override
                {
                    MENGINE_UNUSED( _event );

                    //Empty

                    return false;
                }

                void handleMouseLeave( const InputMouseLeaveEvent & _event ) override
                {
                    MENGINE_UNUSED( _event );

                    //Empty
                }
            };
            //////////////////////////////////////////////////////////////////////////
            typedef IntrusivePtr<PyGlobalBaseHandler> PyGlobalBaseHandlerPtr;
            //////////////////////////////////////////////////////////////////////////
            class PyGlobalMouseLeaveHandler
                : public PyGlobalBaseHandler
            {
                DECLARE_FACTORABLE( PyGlobalMouseLeaveHandler );

            protected:
                void handleMouseLeave( const InputMouseLeaveEvent & _event ) override
                {
                    mt::vec2f point( _event.x, _event.y );

                    mt::vec2f wp;
                    PLAYER_SERVICE()
                        ->calcGlobalMouseWorldPosition( point, &wp );

                    InputMouseLeaveEvent ev = _event;
                    ev.x = wp.x;
                    ev.y = wp.y;

                    pybind::object py_result = this->call_cb( ev );

                    if( py_result.is_none() == false )
                    {
                        LOGGER_ERROR( "cb '%s' args '%s' return value '%s' type '%s' not None"
                            , this->get_cb().repr().c_str()
                            , this->get_args().repr().c_str()
                            , py_result.repr().c_str()
                            , py_result.repr_type().c_str()
                        );
                    }
                }
            };
            //////////////////////////////////////////////////////////////////////////
            typedef IntrusivePtr<PyGlobalMouseLeaveHandler> PyGlobalMouseLeaveHandlerPtr;
            //////////////////////////////////////////////////////////////////////////
            FactoryInterfacePtr m_factoryPyGlobalMouseLeaveHandlers;
            //////////////////////////////////////////////////////////////////////////
            UniqueId s_addMouseLeaveHandler( const pybind::object & _cb, const pybind::args & _args )
            {
                const GlobalInputHandlerInterfacePtr & globalHandleSystem = PLAYER_SERVICE()
                    ->getGlobalInputHandler();

                PyGlobalMouseLeaveHandlerPtr handler = m_factoryPyGlobalMouseLeaveHandlers
                    ->createObject( MENGINE_DOCUMENT_PYBIND );

                handler->initialize( _cb, _args );

                UniqueId id = globalHandleSystem->addGlobalHandler( handler, MENGINE_DOCUMENT_PYBIND );

                return id;
            }
            //////////////////////////////////////////////////////////////////////////
            class PyGlobalMouseMoveHandler
                : public PyGlobalBaseHandler
            {
                DECLARE_FACTORABLE( PyGlobalMouseMoveHandler );

            protected:
                bool handleMouseMove( const InputMouseMoveEvent & _event ) override
                {
                    mt::vec2f point( _event.x, _event.y );
                    mt::vec2f delta( _event.dx, _event.dy );

                    mt::vec2f wp;
                    PLAYER_SERVICE()
                        ->calcGlobalMouseWorldPosition( point, &wp );

                    mt::vec2f wd;
                    PLAYER_SERVICE()
                        ->calcGlobalMouseWorldDelta( delta, &wd );

                    InputMouseMoveEvent ev = _event;
                    ev.x = wp.x;
                    ev.y = wp.y;
                    ev.dx = wd.x;
                    ev.dy = wd.y;

                    pybind::object py_result = this->call_cb( ev );

                    if( py_result.is_none() == false )
                    {
                        LOGGER_ERROR( "cb '%s' args '%s' return value '%s' type '%s' not None"
                            , this->get_cb().repr().c_str()
                            , this->get_args().repr().c_str()
                            , py_result.repr().c_str()
                            , py_result.repr_type().c_str()
                        );
                    }

                    return false;
                }
            };
            //////////////////////////////////////////////////////////////////////////
            typedef IntrusivePtr<PyGlobalMouseMoveHandler> PyGlobalMouseMoveHandlerPtr;
            //////////////////////////////////////////////////////////////////////////
            FactoryInterfacePtr m_factoryPyGlobalMouseMoveHandlers;
            //////////////////////////////////////////////////////////////////////////
            UniqueId s_addMouseMoveHandler( const pybind::object & _cb, const pybind::args & _args )
            {
                const GlobalInputHandlerInterfacePtr & globalHandleSystem = PLAYER_SERVICE()
                    ->getGlobalInputHandler();

                PyGlobalMouseMoveHandlerPtr handler = m_factoryPyGlobalMouseMoveHandlers
                    ->createObject( MENGINE_DOCUMENT_PYBIND );

                handler->initialize( _cb, _args );

                UniqueId id = globalHandleSystem->addGlobalHandler( handler, MENGINE_DOCUMENT_PYBIND );

                return id;
            }
            //////////////////////////////////////////////////////////////////////////
            class PyGlobalMouseHandlerButton
                : public PyGlobalBaseHandler
            {
                DECLARE_FACTORABLE( PyGlobalMouseHandlerButton );

            protected:
                bool handleMouseButtonEvent( const InputMouseButtonEvent & _event ) override
                {
                    mt::vec2f point( _event.x, _event.y );

                    mt::vec2f wp;
                    PLAYER_SERVICE()
                        ->calcGlobalMouseWorldPosition( point, &wp );

                    InputMouseButtonEvent ev = _event;
                    ev.x = wp.x;
                    ev.y = wp.y;

                    pybind::object py_result = this->call_cb( ev );

                    if( py_result.is_none() == false )
                    {
                        LOGGER_ERROR( "cb '%s' args '%s' return value '%s' type '%s' not None"
                            , this->get_cb().repr().c_str()
                            , this->get_args().repr().c_str()
                            , py_result.repr().c_str()
                            , py_result.repr_type().c_str()
                        );
                    }

                    return false;
                }
            };
            //////////////////////////////////////////////////////////////////////////
            typedef IntrusivePtr<PyGlobalMouseHandlerButton> PyGlobalMouseHandlerButtonPtr;
            //////////////////////////////////////////////////////////////////////////
            FactoryInterfacePtr m_factoryPyGlobalMouseHandlerButtons;
            //////////////////////////////////////////////////////////////////////////
            UniqueId s_addMouseButtonHandler( const pybind::object & _cb, const pybind::args & _args )
            {
                const GlobalInputHandlerInterfacePtr & globalHandleSystem = PLAYER_SERVICE()
                    ->getGlobalInputHandler();

                PyGlobalMouseHandlerButtonPtr handler = m_factoryPyGlobalMouseHandlerButtons
                    ->createObject( MENGINE_DOCUMENT_PYBIND );

                handler->initialize( _cb, _args );

                UniqueId id = globalHandleSystem->addGlobalHandler( handler, MENGINE_DOCUMENT_PYBIND );

                return id;
            }
            //////////////////////////////////////////////////////////////////////////
            class PyGlobalMouseHandlerButtonEnd
                : public PyGlobalBaseHandler
            {
                DECLARE_FACTORABLE( PyGlobalMouseHandlerButtonEnd );

            protected:
                //////////////////////////////////////////////////////////////////////////
                bool handleMouseButtonEventEnd( const InputMouseButtonEvent & _event ) override
                {
                    mt::vec2f point( _event.x, _event.y );

                    mt::vec2f wp;
                    PLAYER_SERVICE()
                        ->calcGlobalMouseWorldPosition( point, &wp );

                    InputMouseButtonEvent ev = _event;
                    ev.x = wp.x;
                    ev.y = wp.y;

                    pybind::object py_result = this->call_cb( ev );

                    if( py_result.is_none() == false )
                    {
                        LOGGER_ERROR( "cb '%s' args '%s' return value '%s' type '%s' not None"
                            , this->get_cb().repr().c_str()
                            , this->get_args().repr().c_str()
                            , py_result.repr().c_str()
                            , py_result.repr_type().c_str()
                        );
                    }

                    return false;
                }
            };
            //////////////////////////////////////////////////////////////////////////
            typedef IntrusivePtr<PyGlobalMouseHandlerButtonEnd> PyGlobalMouseHandlerButtonEndPtr;
            //////////////////////////////////////////////////////////////////////////
            FactoryInterfacePtr m_factoryPyGlobalMouseHandlerButtonEnds;
            //////////////////////////////////////////////////////////////////////////
            UniqueId s_addMouseButtonHandlerEnd( const pybind::object & _cb, const pybind::args & _args )
            {
                const GlobalInputHandlerInterfacePtr & globalHandleSystem = PLAYER_SERVICE()
                    ->getGlobalInputHandler();

                PyGlobalMouseHandlerButtonEndPtr handler = m_factoryPyGlobalMouseHandlerButtonEnds
                    ->createObject( MENGINE_DOCUMENT_PYBIND );

                handler->initialize( _cb, _args );

                UniqueId id = globalHandleSystem->addGlobalHandler( handler, MENGINE_DOCUMENT_PYBIND );

                return id;
            }
            //////////////////////////////////////////////////////////////////////////
            class PyGlobalMouseHandlerButtonBegin
                : public PyGlobalBaseHandler
            {
                DECLARE_FACTORABLE( PyGlobalMouseHandlerButtonBegin );

            protected:
                //////////////////////////////////////////////////////////////////////////
                bool handleMouseButtonEventBegin( const InputMouseButtonEvent & _event ) override
                {
                    mt::vec2f point( _event.x, _event.y );

                    mt::vec2f wp;
                    PLAYER_SERVICE()
                        ->calcGlobalMouseWorldPosition( point, &wp );

                    InputMouseButtonEvent ev = _event;
                    ev.x = wp.x;
                    ev.y = wp.y;

                    pybind::object py_result = this->call_cb( ev );

                    if( py_result.is_none() == false )
                    {
                        LOGGER_ERROR( "cb '%s' args '%s' return value '%s' type '%s' not None"
                            , this->get_cb().repr().c_str()
                            , this->get_args().repr().c_str()
                            , py_result.repr().c_str()
                            , py_result.repr_type().c_str()
                        );
                    }

                    return false;
                }
            };
            //////////////////////////////////////////////////////////////////////////
            typedef IntrusivePtr<PyGlobalMouseHandlerButtonBegin> PyGlobalMouseHandlerButtonBeginPtr;
            //////////////////////////////////////////////////////////////////////////
            FactoryInterfacePtr m_factoryPyGlobalMouseHandlerButtonBegins;
            //////////////////////////////////////////////////////////////////////////
            UniqueId s_addMouseButtonHandlerBegin( const pybind::object & _cb, const pybind::args & _args )
            {
                const GlobalInputHandlerInterfacePtr & globalHandleSystem = PLAYER_SERVICE()
                    ->getGlobalInputHandler();

                MENGINE_ASSERTION_MEMORY_PANIC( globalHandleSystem, "invalid get global input handler" );

                PyGlobalMouseHandlerButtonBeginPtr handler = m_factoryPyGlobalMouseHandlerButtonBegins
                    ->createObject( MENGINE_DOCUMENT_PYBIND );

                handler->initialize( _cb, _args );

                UniqueId id = globalHandleSystem->addGlobalHandler( handler, MENGINE_DOCUMENT_PYBIND );

                return id;
            }
            //////////////////////////////////////////////////////////////////////////
            class PyGlobalMouseHandlerWheel
                : public PyGlobalBaseHandler
            {
                DECLARE_FACTORABLE( PyGlobalMouseHandlerWheel );

            protected:
                //////////////////////////////////////////////////////////////////////////
                bool handleMouseWheel( const InputMouseWheelEvent & _event ) override
                {
                    pybind::object py_result = this->call_cb( _event );

                    if( py_result.is_none() == false )
                    {
                        LOGGER_ERROR( "cb '%s' args '%s' return value '%s' type '%s' not None"
                            , this->get_cb().repr().c_str()
                            , this->get_args().repr().c_str()
                            , py_result.repr().c_str()
                            , py_result.repr_type().c_str()
                        );
                    }

                    return false;
                }
            };
            //////////////////////////////////////////////////////////////////////////
            typedef IntrusivePtr<PyGlobalMouseHandlerWheel> PyGlobalMouseHandlerWheelPtr;
            //////////////////////////////////////////////////////////////////////////
            FactoryInterfacePtr m_factoryPyGlobalMouseHandlerWheels;
            //////////////////////////////////////////////////////////////////////////
            UniqueId s_addMouseWheelHandler( const pybind::object & _cb, const pybind::args & _args )
            {
                const GlobalInputHandlerInterfacePtr & globalHandleSystem = PLAYER_SERVICE()
                    ->getGlobalInputHandler();

                MENGINE_ASSERTION_MEMORY_PANIC( globalHandleSystem, "invalid get global input handler" );

                const PyGlobalMouseHandlerWheelPtr & handler = m_factoryPyGlobalMouseHandlerWheels
                    ->createObject( MENGINE_DOCUMENT_PYBIND );

                handler->initialize( _cb, _args );

                UniqueId id = globalHandleSystem->addGlobalHandler( handler, MENGINE_DOCUMENT_PYBIND );

                return id;
            }
            //////////////////////////////////////////////////////////////////////////
            class PyGlobalKeyHandler
                : public PyGlobalBaseHandler
            {
                DECLARE_FACTORABLE( PyGlobalKeyHandler );

            protected:
                bool handleKeyEvent( const InputKeyEvent & _event ) override
                {
                    pybind::object py_result = this->call_cb( _event );

                    if( py_result.is_none() == false )
                    {
                        LOGGER_ERROR( "cb '%s' args '%s' return value '%s' type '%s' not None"
                            , this->get_cb().repr().c_str()
                            , this->get_args().repr().c_str()
                            , py_result.repr().c_str()
                            , py_result.repr_type().c_str()
                        );
                    }

                    return false;
                }
            };
            //////////////////////////////////////////////////////////////////////////
            typedef IntrusivePtr<PyGlobalKeyHandler> PyGlobalKeyHandlerPtr;
            //////////////////////////////////////////////////////////////////////////
            FactoryInterfacePtr m_factoryPyGlobalKeyHandler;
            //////////////////////////////////////////////////////////////////////////
            UniqueId s_addKeyHandler( const pybind::object & _cb, const pybind::args & _args )
            {
                const GlobalInputHandlerInterfacePtr & globalHandleSystem = PLAYER_SERVICE()
                    ->getGlobalInputHandler();

                MENGINE_ASSERTION_MEMORY_PANIC( globalHandleSystem, "invalid get global input handler" );

                PyGlobalKeyHandlerPtr handler = m_factoryPyGlobalKeyHandler
                    ->createObject( MENGINE_DOCUMENT_PYBIND );

                handler->initialize( _cb, _args );

                UniqueId id = globalHandleSystem->addGlobalHandler( handler, MENGINE_DOCUMENT_PYBIND );

                return id;
            }
            //////////////////////////////////////////////////////////////////////////
            class PyGlobalTextHandler
                : public PyGlobalBaseHandler
            {
                DECLARE_FACTORABLE( PyGlobalTextHandler );

            protected:
                bool handleTextEvent( const InputTextEvent & _event ) override
                {
                    pybind::object py_result = this->call_cb( _event );

                    if( py_result.is_none() == false )
                    {
                        LOGGER_ERROR( "cb '%s' args '%s' return value '%s' type '%s' not None"
                            , this->get_cb().repr().c_str()
                            , this->get_args().repr().c_str()
                            , py_result.repr().c_str()
                            , py_result.repr_type().c_str()
                        );
                    }

                    return false;
                }
            };
            //////////////////////////////////////////////////////////////////////////
            FactoryInterfacePtr m_factoryPyGlobalTextHandler;
            //////////////////////////////////////////////////////////////////////////
            UniqueId s_addTextHandler( const pybind::object & _cb, const pybind::args & _args )
            {
                const GlobalInputHandlerInterfacePtr & globalHandleSystem = PLAYER_SERVICE()
                    ->getGlobalInputHandler();

                MENGINE_ASSERTION_MEMORY_PANIC( globalHandleSystem, "invalid get global input handler" );

                PyGlobalBaseHandlerPtr handler = m_factoryPyGlobalTextHandler
                    ->createObject( MENGINE_DOCUMENT_PYBIND );

                handler->initialize( _cb, _args );

                UniqueId id = globalHandleSystem->addGlobalHandler( handler, MENGINE_DOCUMENT_PYBIND );

                return id;
            }
            //////////////////////////////////////////////////////////////////////////
            class PyGlobalAccelerometerHandler
                : public PyGlobalBaseHandler
            {
                DECLARE_FACTORABLE( PyGlobalAccelerometerHandler );

            protected:
                bool handleAccelerometerEvent( const InputAccelerometerEvent & _event ) override
                {
                    pybind::object py_result = this->call_cb( _event );

                    if( py_result.is_none() == false )
                    {
                        LOGGER_ERROR( "cb '%s' args '%s' return value '%s' type '%s' not None"
                            , this->get_cb().repr().c_str()
                            , this->get_args().repr().c_str()
                            , py_result.repr().c_str()
                            , py_result.repr_type().c_str()
                        );
                    }

                    return false;
                }
            };
            //////////////////////////////////////////////////////////////////////////
            FactoryInterfacePtr m_factoryPyGlobalAccelerometerHandler;
            //////////////////////////////////////////////////////////////////////////
            UniqueId s_addAccelerometerHandler( const pybind::object & _cb, const pybind::args & _args )
            {
                const GlobalInputHandlerInterfacePtr & globalHandleSystem = PLAYER_SERVICE()
                    ->getGlobalInputHandler();

                MENGINE_ASSERTION_MEMORY_PANIC( globalHandleSystem, "invalid get global input handler" );

                PyGlobalBaseHandlerPtr handler = m_factoryPyGlobalAccelerometerHandler
                    ->createObject( MENGINE_DOCUMENT_PYBIND );

                handler->initialize( _cb, _args );

                UniqueId id = globalHandleSystem->addGlobalHandler( handler, MENGINE_DOCUMENT_PYBIND );

                return id;
            }
            //////////////////////////////////////////////////////////////////////////
            bool s_removeGlobalHandler( UniqueId _id )
            {
                const GlobalInputHandlerInterfacePtr & globalHandleSystem = PLAYER_SERVICE()
                    ->getGlobalInputHandler();

                MENGINE_ASSERTION_MEMORY_PANIC( globalHandleSystem, "invalid get global input handler" );

                InputHandlerInterfacePtr handler = globalHandleSystem->removeGlobalHandler( _id );

                PyGlobalBaseHandlerPtr py_handler = PyGlobalBaseHandlerPtr::dynamic_from( handler );

                MENGINE_ASSERTION_MEMORY_PANIC( py_handler, "%u handler invalid"
                    , _id
                );

                return true;
            }
            //////////////////////////////////////////////////////////////////////////
            bool s_enableGlobalHandler( UniqueId _id, bool _value )
            {
                const GlobalInputHandlerInterfacePtr & globalHandleSystem = PLAYER_SERVICE()
                    ->getGlobalInputHandler();

                MENGINE_ASSERTION_MEMORY_PANIC( globalHandleSystem, "invalid get global input handler" );

                bool successful = globalHandleSystem->enableGlobalHandler( _id, _value );

                return successful;
            }
            //////////////////////////////////////////////////////////////////////////
            float s_getGameAspect()
            {
                float aspect;
                Viewport viewport;
                APPLICATION_SERVICE()
                    ->getGameViewport( &aspect, &viewport );

                return aspect;
            }
            //////////////////////////////////////////////////////////////////////////
            Viewport s_getGameViewport()
            {
                float aspect;
                Viewport viewport;
                APPLICATION_SERVICE()
                    ->getGameViewport( &aspect, &viewport );

                return viewport;
            }
            //////////////////////////////////////////////////////////////////////////
            EContentEdgeMode s_getAspectRatioContentEdgeMode()
            {
                EContentEdgeMode mode = APPLICATION_SERVICE()
                    ->getAspectRatioContentEdgeMode();

                return mode;
            }
            //////////////////////////////////////////////////////////////////////////
            bool s_hasGameParam( const ConstString & _paramName )
            {
                const ConfigInterfacePtr & config = CONFIG_SERVICE()
                    ->getDefaultConfig();

                bool result = config->existValue( "Params", _paramName.c_str() );

                return result;
            }
            //////////////////////////////////////////////////////////////////////////
            PyObject * s_getGameParamString( pybind::kernel_interface * _kernel, const ConstString & _paramName )
            {
                const ConfigInterfacePtr & config = CONFIG_SERVICE()
                    ->getDefaultConfig();

                const Char * param_value;
                if( config->hasValue( "Params", _paramName.c_str(), "", &param_value ) == false )
                {
                    return _kernel->ret_none();
                }

                PyObject * py_param = _kernel->string_from_char( param_value );

                return py_param;
            }
            //////////////////////////////////////////////////////////////////////////
            PyObject * s_getGameParamUnicode( pybind::kernel_interface * _kernel, const ConstString & _paramName )
            {
                const ConfigInterfacePtr & config = CONFIG_SERVICE()
                    ->getDefaultConfig();

                const Char * param_value;
                if( config->hasValue( "Params", _paramName.c_str(), "", &param_value ) == false )
                {
                    return _kernel->ret_none();
                }

                PyObject * py_param = _kernel->unicode_from_utf8( param_value );

                return py_param;
            }
            //////////////////////////////////////////////////////////////////////////
            PyObject * s_getGameParamFloat( pybind::kernel_interface * _kernel, const ConstString & _paramName )
            {
                const ConfigInterfacePtr & config = CONFIG_SERVICE()
                    ->getDefaultConfig();

                float param_value;
                if( config->hasValueFloat( "Params", _paramName.c_str(), 0.f, &param_value ) == false )
                {
                    return _kernel->ret_none();
                }

                return pybind::ptr( _kernel, param_value );
            }
            //////////////////////////////////////////////////////////////////////////
            PyObject * s_getGameParamInt( pybind::kernel_interface * _kernel, const ConstString & _paramName )
            {
                const ConfigInterfacePtr & config = CONFIG_SERVICE()
                    ->getDefaultConfig();

                int32_t param_value;
                if( config->hasValueInteger( "Params", _paramName.c_str(), 0, &param_value ) == false )
                {
                    return _kernel->ret_none();
                }

                return pybind::ptr( _kernel, param_value );
            }
            //////////////////////////////////////////////////////////////////////////
            bool s_getGameParamBool( const ConstString & _paramName, bool _default )
            {
                const ConfigInterfacePtr & config = CONFIG_SERVICE()
                    ->getDefaultConfig();

                bool param_value;
                config->hasValue( "Params", _paramName.c_str(), _default, &param_value );

                return param_value;
            }
            //////////////////////////////////////////////////////////////////////////
            bool s_openUrlInDefaultBrowser( const WString & _url )
            {
                Char utf8_url[4096 + 1] = {'\0'};
                Helper::unicodeToUtf8( _url, utf8_url, 4096 );

                bool val = PLATFORM_SERVICE()
                    ->openUrlInDefaultBrowser( utf8_url );

                return val;
            }
            //////////////////////////////////////////////////////////////////////////
            bool s_openMail( const WString & _email, const WString & _subject, const WString & _body )
            {
                Char utf8_email[4096 + 1] = {'\0'};
                Helper::unicodeToUtf8( _email, utf8_email, 4096 );

                Char utf8_subject[4096 + 1] = {'\0'};
                Helper::unicodeToUtf8( _subject, utf8_subject, 4096 );

                Char utf8_body[4096 + 1] = {'\0'};
                Helper::unicodeToUtf8( _body, utf8_body, 4096 );

                bool val = PLATFORM_SERVICE()
                    ->openMail( utf8_email, utf8_subject, utf8_body );

                return val;
            }
            //////////////////////////////////////////////////////////////////////////
            bool s_openDeleteAccount()
            {
                bool val = PLATFORM_SERVICE()
                    ->openDeleteAccount();

                return val;
            }
            //////////////////////////////////////////////////////////////////////////
            ConstString s_getDefaultResourceFontName()
            {
                const FontInterfacePtr & defaultResourceFont = FONT_SERVICE()
                    ->getDefaultFont();

                const ConstString & defaultResourceFontName = defaultResourceFont->getName();

                return defaultResourceFontName;
            }
            //////////////////////////////////////////////////////////////////////////
            void s_visitChild( Node * _node, const pybind::object & _cb )
            {
                MENGINE_ASSERTION_MEMORY_PANIC( _node, "Menge.visitChild send None object" );

                _node->foreachChildrenSlug( [_cb]( const NodePtr & _node )
                {
                    _cb.call( _node );
                } );
            }
            //////////////////////////////////////////////////////////////////////////
            void s_visitCompiledResources( const ConstString & _groupName, const pybind::object & _cb )
            {
                RESOURCE_SERVICE()
                    ->foreachGroupResources( _groupName, [_cb]( const ResourcePtr & _resource )
                {
                    if( _resource->isCompile() == false )
                    {
                        return;
                    }

                    _cb.call( _resource );
                } );
            }
            //////////////////////////////////////////////////////////////////////////
            void s_visitResources( const ConstString & _groupName, const pybind::object & _cb )
            {
                RESOURCE_SERVICE()
                    ->foreachGroupResources( _groupName, [_cb]( const ResourcePtr & _resource )
                {
                    _cb.call( _resource );
                } );
            }
            //////////////////////////////////////////////////////////////////////////
            void s_incrementResources( const ConstString & _groupName )
            {
                RESOURCE_SERVICE()
                    ->foreachGroupResources( _groupName, []( const ResourcePtr & _resource )
                {
                    if( _resource->compile() == false )
                    {
                        LOGGER_ERROR( "invalid direct compile resource '%s' type '%s' content '%s'"
                            , _resource->getName().c_str()
                            , _resource->getType().c_str()
                            , _resource->getContent() != nullptr ? Helper::getContentFullPath( _resource->getContent() ).c_str() : "[no content]"
                        );
                    }
                } );
            }
            //////////////////////////////////////////////////////////////////////////
            void s_decrementResources( const ConstString & _groupName )
            {
                RESOURCE_SERVICE()
                    ->foreachGroupResources( _groupName, []( const ResourcePtr & _resource )
                {
                    _resource->release();
                } );
            }
            //////////////////////////////////////////////////////////////////////////
            void s_incrementResourcesTags( const Tags & _tags )
            {
                RESOURCE_SERVICE()
                    ->foreachTagsResources( _tags, []( const ResourcePtr & _resource )
                {
                    if( _resource->compile() == false )
                    {
                        LOGGER_ERROR( "invalid direct compile resource '%s' type '%s' content '%s'"
                            , _resource->getName().c_str()
                            , _resource->getType().c_str()
                            , _resource->getContent() != nullptr ? Helper::getContentFullPath( _resource->getContent() ).c_str() : "[no content]"
                        );
                    }
                } );
            }
            //////////////////////////////////////////////////////////////////////////
            void s_decrementResourcesTags( const Tags & _tags )
            {
                RESOURCE_SERVICE()
                    ->foreachTagsResources( _tags, []( const ResourcePtr & _resource )
                {
                    _resource->release();
                } );
            }
            //////////////////////////////////////////////////////////////////////////
            pybind::list s_getResources( pybind::kernel_interface * _kernel, const ConstString & _groupName )
            {
                pybind::list l( _kernel );

                RESOURCE_SERVICE()
                    ->foreachGroupResources( _groupName, [&l]( const ResourcePtr & _resouce )
                {
                    l.append( _resouce );
                } );

                return l;
            }
            //////////////////////////////////////////////////////////////////////////
            bool s_validResource( const ConstString & _resourceName )
            {
                const ResourcePtr & resource = RESOURCE_SERVICE()
                    ->getResourceReference( ConstString::none(), _resourceName );

                MENGINE_ASSERTION_MEMORY_PANIC( resource, "resource '%s' not found"
                    , _resourceName.c_str()
                );

                if( SERVICE_IS_INITIALIZE( ResourceValidateServiceInterface ) == false )
                {
                    return true;
                }

                bool valid = RESOURCEVALIDATE_SERVICE()
                    ->validateResource( resource );

                return valid;
            }
        };
        //////////////////////////////////////////////////////////////////////////
        typedef IntrusivePtr<EngineScriptMethod> EngineScriptMethodPtr;
        //////////////////////////////////////////////////////////////////////////
    }
    EngineScriptEmbedding::EngineScriptEmbedding()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    EngineScriptEmbedding::~EngineScriptEmbedding()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool EngineScriptEmbedding::embed( pybind::kernel_interface * _kernel )
    {
        EngineScriptMethodPtr nodeScriptMethod = Helper::makeFactorableUnique<EngineScriptMethod>( MENGINE_DOCUMENT_FACTORABLE );

        if( nodeScriptMethod->initialize() == false )
        {
            return false;
        }

        pybind::def_functor_args( _kernel, "createCurrentScene", nodeScriptMethod, &EngineScriptMethod::createCurrentScene );
        pybind::def_functor_args( _kernel, "setCurrentScene", nodeScriptMethod, &EngineScriptMethod::setCurrentScene );
        pybind::def_functor_args( _kernel, "restartCurrentScene", nodeScriptMethod, &EngineScriptMethod::restartCurrentScene );
        pybind::def_functor_args( _kernel, "removeCurrentScene", nodeScriptMethod, &EngineScriptMethod::s_removeCurrentScene );
        pybind::def_functor( _kernel, "getCurrentScene", nodeScriptMethod, &EngineScriptMethod::getCurrentScene );

        pybind::def_functor( _kernel, "createGlobalScene", nodeScriptMethod, &EngineScriptMethod::createGlobalScene );
        pybind::def_functor( _kernel, "removeGlobalScene", nodeScriptMethod, &EngineScriptMethod::removeGlobalScene );
        pybind::def_functor( _kernel, "getGlobalScene", nodeScriptMethod, &EngineScriptMethod::getGlobalScene );

        pybind::def_functor( _kernel, "createScene", nodeScriptMethod, &EngineScriptMethod::s_createScene );

        pybind::def_functor( _kernel, "getCamera2DPosition", nodeScriptMethod, &EngineScriptMethod::s_getCamera2DPosition );

        pybind::def_functor( _kernel, "createNode", nodeScriptMethod, &EngineScriptMethod::s_createNode );
        pybind::def_functor( _kernel, "destroyNode", nodeScriptMethod, &EngineScriptMethod::s_destroyNode );

        pybind::def_functor( _kernel, "createSurface", nodeScriptMethod, &EngineScriptMethod::s_createSurface );
        pybind::def_functor( _kernel, "createSprite", nodeScriptMethod, &EngineScriptMethod::s_createSprite );

        pybind::def_functor( _kernel, "generateRandomizer", nodeScriptMethod, &EngineScriptMethod::s_generateRandomizer );

        pybind::def_functor_args( _kernel, "addTimer", nodeScriptMethod, &EngineScriptMethod::s_addTimer );
        pybind::def_functor( _kernel, "removeTimer", nodeScriptMethod, &EngineScriptMethod::s_removeTimer );

        pybind::def_functor_args( _kernel, "timing", nodeScriptMethod, &EngineScriptMethod::s_timing );
        pybind::def_functor( _kernel, "timingRemove", nodeScriptMethod, &EngineScriptMethod::s_timingRemove );

        pybind::def_functor( _kernel, "createScheduler", nodeScriptMethod, &EngineScriptMethod::s_createScheduler );
        pybind::def_functor( _kernel, "destroyScheduler", nodeScriptMethod, &EngineScriptMethod::s_destroyScheduler );

        pybind::def_functor( _kernel, "createLayout", nodeScriptMethod, &EngineScriptMethod::s_createLayout );
        pybind::def_functor( _kernel, "destroyLayout", nodeScriptMethod, &EngineScriptMethod::s_destroyLayout );

        pybind::def_functor_args( _kernel, "schedule", nodeScriptMethod, &EngineScriptMethod::s_schedule );
        pybind::def_functor_args( _kernel, "pipe", nodeScriptMethod, &EngineScriptMethod::s_pipe );
        pybind::def_functor( _kernel, "scheduleRemove", nodeScriptMethod, &EngineScriptMethod::s_scheduleRemove );
        pybind::def_functor( _kernel, "scheduleRemoveAll", nodeScriptMethod, &EngineScriptMethod::s_scheduleRemoveAll );
        pybind::def_functor( _kernel, "scheduleFreeze", nodeScriptMethod, &EngineScriptMethod::s_scheduleFreeze );
        pybind::def_functor( _kernel, "scheduleFreezeAll", nodeScriptMethod, &EngineScriptMethod::s_scheduleFreezeAll );
        pybind::def_functor( _kernel, "scheduleResumeAll", nodeScriptMethod, &EngineScriptMethod::s_scheduleResumeAll );
        pybind::def_functor( _kernel, "scheduleIsFreeze", nodeScriptMethod, &EngineScriptMethod::s_scheduleIsFreeze );
        pybind::def_functor( _kernel, "scheduleTime", nodeScriptMethod, &EngineScriptMethod::s_scheduleTime );

        pybind::def_functor_args( _kernel, "scheduleGlobal", nodeScriptMethod, &EngineScriptMethod::s_scheduleGlobal );
        pybind::def_functor( _kernel, "scheduleGlobalRemove", nodeScriptMethod, &EngineScriptMethod::s_scheduleGlobalRemove );
        pybind::def_functor( _kernel, "scheduleGlobalRemoveAll", nodeScriptMethod, &EngineScriptMethod::s_scheduleGlobalRemoveAll );
        pybind::def_functor( _kernel, "scheduleGlobalFreeze", nodeScriptMethod, &EngineScriptMethod::s_scheduleGlobalFreeze );
        pybind::def_functor( _kernel, "scheduleGlobalFreezeAll", nodeScriptMethod, &EngineScriptMethod::s_scheduleGlobalFreezeAll );
        pybind::def_functor( _kernel, "scheduleGlobalResumeAll", nodeScriptMethod, &EngineScriptMethod::s_scheduleGlobalResumeAll );
        pybind::def_functor( _kernel, "scheduleGlobalIsFreeze", nodeScriptMethod, &EngineScriptMethod::s_scheduleGlobalIsFreeze );
        pybind::def_functor_deprecated( _kernel, "scheduleGlobalTime", nodeScriptMethod, &EngineScriptMethod::s_scheduleGlobalPassed, "use scheduleGlobalPassed" );
        pybind::def_functor( _kernel, "scheduleGlobalPassed", nodeScriptMethod, &EngineScriptMethod::s_scheduleGlobalPassed );
        pybind::def_functor( _kernel, "scheduleGlobalLeft", nodeScriptMethod, &EngineScriptMethod::s_scheduleGlobalLeft );

        pybind::def_functor( _kernel, "getCursorPosition", nodeScriptMethod, &EngineScriptMethod::s_getCursorPosition );
        pybind::def_functor( _kernel, "getTouchPosition", nodeScriptMethod, &EngineScriptMethod::s_getTouchPosition );
        pybind::def_functor( _kernel, "getMousePosition", nodeScriptMethod, &EngineScriptMethod::s_getMousePosition );

        pybind::def_functor( _kernel, "getArrow", nodeScriptMethod, &EngineScriptMethod::s_getArrowService );
        pybind::def_functor( _kernel, "hideArrow", nodeScriptMethod, &EngineScriptMethod::s_hideArrow );

        pybind::def_functor( _kernel, "setArrowLayer", nodeScriptMethod, &EngineScriptMethod::s_setArrowLayer );

        pybind::def_functor( _kernel, "createResource", nodeScriptMethod, &EngineScriptMethod::s_createResource );
        pybind::def_functor( _kernel, "removeResource", nodeScriptMethod, &EngineScriptMethod::s_removeResource );

        pybind::def_functor( _kernel, "directResourceCompile", nodeScriptMethod, &EngineScriptMethod::s_directResourceCompile );
        pybind::def_functor( _kernel, "directResourceRelease", nodeScriptMethod, &EngineScriptMethod::s_directResourceRelease );
        pybind::def_functor( _kernel, "getResource", nodeScriptMethod, &EngineScriptMethod::s_getResource );
        pybind::def_functor( _kernel, "getResourceReference", nodeScriptMethod, &EngineScriptMethod::s_getResourceReference );

        pybind::def_functor( _kernel, "directFontCompile", nodeScriptMethod, &EngineScriptMethod::s_directFontCompile );
        pybind::def_functor( _kernel, "directFontRelease", nodeScriptMethod, &EngineScriptMethod::s_directFontRelease );

        pybind::def_functor( _kernel, "quitApplication", nodeScriptMethod, &EngineScriptMethod::s_quitApplication );
        pybind::def_functor( _kernel, "setFullscreenMode", nodeScriptMethod, &EngineScriptMethod::s_setFullscreenMode );
        pybind::def_functor( _kernel, "getFullscreenMode", nodeScriptMethod, &EngineScriptMethod::s_getFullscreenMode );
        pybind::def_functor( _kernel, "setFixedContentResolution", nodeScriptMethod, &EngineScriptMethod::s_setFixedContentResolution );
        pybind::def_functor( _kernel, "getFixedContentResolution", nodeScriptMethod, &EngineScriptMethod::s_getFixedContentResolution );
        pybind::def_functor( _kernel, "setFixedDisplayResolution", nodeScriptMethod, &EngineScriptMethod::s_setFixedDisplayResolution );
        pybind::def_functor( _kernel, "getFixedDisplayResolution", nodeScriptMethod, &EngineScriptMethod::s_getFixedDisplayResolution );
        pybind::def_functor( _kernel, "setFixedViewportResolution", nodeScriptMethod, &EngineScriptMethod::s_setFixedViewportResolution );
        pybind::def_functor( _kernel, "getFixedViewportResolution", nodeScriptMethod, &EngineScriptMethod::s_getFixedViewportResolution );

        pybind::def_functor_args( _kernel, "foreachSceneHotspot", nodeScriptMethod, &EngineScriptMethod::s_foreachSceneHotspot );


        pybind::def_functor_deprecated( _kernel, "renderOneFrame", nodeScriptMethod, &EngineScriptMethod::s_renderOneFrame, "don't use" );
        pybind::def_functor( _kernel, "writeImageToFile", nodeScriptMethod, &EngineScriptMethod::s_writeImageToFile );
        pybind::def_functor( _kernel, "createImageResource", nodeScriptMethod, &EngineScriptMethod::s_createImageResource );
        pybind::def_functor( _kernel, "createImageSolidResource", nodeScriptMethod, &EngineScriptMethod::s_createImageSolidResource );
        pybind::def_functor( _kernel, "minimizeWindow", nodeScriptMethod, &EngineScriptMethod::s_minimizeWindow );

        pybind::def_functor( _kernel, "getCurrentResolution", nodeScriptMethod, &EngineScriptMethod::s_getCurrentResolution );
        pybind::def_functor( _kernel, "getContentResolution", nodeScriptMethod, &EngineScriptMethod::s_getContentResolution );
        pybind::def_functor( _kernel, "getHotSpotImageSize", nodeScriptMethod, &EngineScriptMethod::s_getHotSpotImageSize );

        pybind::def_functor( _kernel, "setNopause", nodeScriptMethod, &EngineScriptMethod::s_setNopause );
        pybind::def_functor( _kernel, "getNopause", nodeScriptMethod, &EngineScriptMethod::s_getNopause );

        pybind::def_functor( _kernel, "isInViewport", nodeScriptMethod, &EngineScriptMethod::s_isInViewport );
        pybind::def_functor( _kernel, "enableTextureFiltering", nodeScriptMethod, &EngineScriptMethod::s_enableTextureFiltering );
        pybind::def_functor( _kernel, "isTextureFilteringEnabled", nodeScriptMethod, &EngineScriptMethod::s_isTextureFilteringEnabled );

        pybind::def_functor( _kernel, "existText", nodeScriptMethod, &EngineScriptMethod::s_existText );

        pybind::def_functor_kernel( _kernel, "pickHotspot", nodeScriptMethod, &EngineScriptMethod::s_pickHotspot );
        pybind::def_functor_kernel( _kernel, "pickAllHotspot", nodeScriptMethod, &EngineScriptMethod::s_pickAllHotspot );

        pybind::def_functor( _kernel, "blockInput", nodeScriptMethod, &EngineScriptMethod::s_setMousePickerBlockInput );
        pybind::def_functor( _kernel, "setMousePickerHandleValue", nodeScriptMethod, &EngineScriptMethod::s_setMousePickerHandleValue );

        pybind::def_functor( _kernel, "setInputMouseButtonEventBlock", nodeScriptMethod, &EngineScriptMethod::s_setInputMouseButtonEventBlock );
        pybind::def_functor( _kernel, "getInputMouseButtonEventBlock", nodeScriptMethod, &EngineScriptMethod::s_getInputMouseButtonEventBlock );

        pybind::def_functor( _kernel, "loadPlugin", nodeScriptMethod, &EngineScriptMethod::s_loadPlugin );

        pybind::def_functor( _kernel, "getRenderCamera2D", nodeScriptMethod, &EngineScriptMethod::s_getRenderCamera2D );

        pybind::def_functor( _kernel, "testHotspot", nodeScriptMethod, &EngineScriptMethod::s_testHotspot );
        pybind::def_functor( _kernel, "polygon_wm", nodeScriptMethod, &EngineScriptMethod::s_polygon_wm );
        pybind::def_functor( _kernel, "polygon_anchor", nodeScriptMethod, &EngineScriptMethod::s_polygon_anchor );

        pybind::def_functor( _kernel, "showKeyboard", nodeScriptMethod, &EngineScriptMethod::s_showKeyboard );
        pybind::def_functor( _kernel, "hideKeyboard", nodeScriptMethod, &EngineScriptMethod::s_hideKeyboard );
        pybind::def_functor( _kernel, "isShowKeyboard", nodeScriptMethod, &EngineScriptMethod::s_isShowKeyboard );

        pybind::def_functor( _kernel, "hasResource", nodeScriptMethod, &EngineScriptMethod::s_hasResource );

        pybind::def_functor_args( _kernel, "addMouseLeaveHandler", nodeScriptMethod, &EngineScriptMethod::s_addMouseLeaveHandler );
        pybind::def_functor_args( _kernel, "addMouseMoveHandler", nodeScriptMethod, &EngineScriptMethod::s_addMouseMoveHandler );
        pybind::def_functor_args( _kernel, "addMouseButtonHandler", nodeScriptMethod, &EngineScriptMethod::s_addMouseButtonHandler );
        pybind::def_functor_args( _kernel, "addMouseButtonHandlerBegin", nodeScriptMethod, &EngineScriptMethod::s_addMouseButtonHandlerBegin );
        pybind::def_functor_args( _kernel, "addMouseButtonHandlerEnd", nodeScriptMethod, &EngineScriptMethod::s_addMouseButtonHandlerEnd );
        pybind::def_functor_args( _kernel, "addMouseWheelHandler", nodeScriptMethod, &EngineScriptMethod::s_addMouseWheelHandler );
        pybind::def_functor_args( _kernel, "addKeyHandler", nodeScriptMethod, &EngineScriptMethod::s_addKeyHandler );
        pybind::def_functor_args( _kernel, "addTextHandler", nodeScriptMethod, &EngineScriptMethod::s_addTextHandler );
        pybind::def_functor_args( _kernel, "addAccelerometerHandler", nodeScriptMethod, &EngineScriptMethod::s_addAccelerometerHandler );

        pybind::def_functor( _kernel, "removeGlobalHandler", nodeScriptMethod, &EngineScriptMethod::s_removeGlobalHandler );
        pybind::def_functor( _kernel, "enableGlobalHandler", nodeScriptMethod, &EngineScriptMethod::s_enableGlobalHandler );

        pybind::def_functor( _kernel, "visitChild", nodeScriptMethod, &EngineScriptMethod::s_visitChild );

        pybind::def_functor( _kernel, "visitCompiledResources", nodeScriptMethod, &EngineScriptMethod::s_visitCompiledResources );
        pybind::def_functor( _kernel, "visitResources", nodeScriptMethod, &EngineScriptMethod::s_visitResources );

        pybind::def_functor( _kernel, "incrementResources", nodeScriptMethod, &EngineScriptMethod::s_incrementResources );
        pybind::def_functor( _kernel, "decrementResources", nodeScriptMethod, &EngineScriptMethod::s_decrementResources );
        pybind::def_functor( _kernel, "incrementResourcesTags", nodeScriptMethod, &EngineScriptMethod::s_incrementResourcesTags );
        pybind::def_functor( _kernel, "decrementResourcesTags", nodeScriptMethod, &EngineScriptMethod::s_decrementResourcesTags );

        pybind::def_functor( _kernel, "validResource", nodeScriptMethod, &EngineScriptMethod::s_validResource );

        pybind::def_functor_kernel( _kernel, "getResources", nodeScriptMethod, &EngineScriptMethod::s_getResources );

        pybind::def_functor( _kernel, "testPlatformTags", nodeScriptMethod, &EngineScriptMethod::s_testPlatformTags );
        pybind::def_functor( _kernel, "hasPlatformTag", nodeScriptMethod, &EngineScriptMethod::s_hasPlatformTag );
        pybind::def_functor( _kernel, "hasTouchpad", nodeScriptMethod, &EngineScriptMethod::s_hasTouchpad );


        pybind::def_functor( _kernel, "addHomeless", nodeScriptMethod, &EngineScriptMethod::s_addHomeless );
        pybind::def_functor( _kernel, "isHomeless", nodeScriptMethod, &EngineScriptMethod::s_isHomeless );

        pybind::def_functor( _kernel, "cancelTask", nodeScriptMethod, &EngineScriptMethod::s_cancelTask );
        pybind::def_functor( _kernel, "joinTask", nodeScriptMethod, &EngineScriptMethod::s_joinTask );

        pybind::def_functor( _kernel, "getGameAspect", nodeScriptMethod, &EngineScriptMethod::s_getGameAspect );
        pybind::def_functor( _kernel, "getGameViewport", nodeScriptMethod, &EngineScriptMethod::s_getGameViewport );

        pybind::enum_<EContentEdgeMode>( _kernel, "EContentEdgeMode" )
            .def( "ECEM_NONE", ECEM_NONE )
            .def( "ECEM_HORIZONTAL_CONTENT_EDGE", ECEM_HORIZONTAL_CONTENT_EDGE )
            .def( "ECEM_VERTICAL_CONTENT_EDGE", ECEM_VERTICAL_CONTENT_EDGE )
            ;

        pybind::def_functor( _kernel, "getAspectRatioContentEdgeMode", nodeScriptMethod, &EngineScriptMethod::s_getAspectRatioContentEdgeMode );

        pybind::def_functor( _kernel, "hasGameParam", nodeScriptMethod, &EngineScriptMethod::s_hasGameParam );
        pybind::def_functor_kernel( _kernel, "getGameParamString", nodeScriptMethod, &EngineScriptMethod::s_getGameParamString );
        pybind::def_functor_kernel( _kernel, "getGameParamUnicode", nodeScriptMethod, &EngineScriptMethod::s_getGameParamUnicode );
        pybind::def_functor_kernel( _kernel, "getGameParamFloat", nodeScriptMethod, &EngineScriptMethod::s_getGameParamFloat );
        pybind::def_functor_kernel( _kernel, "getGameParamInt", nodeScriptMethod, &EngineScriptMethod::s_getGameParamInt );
        pybind::def_functor( _kernel, "getGameParamBool", nodeScriptMethod, &EngineScriptMethod::s_getGameParamBool );

        pybind::def_functor( _kernel, "openUrlInDefaultBrowser", nodeScriptMethod, &EngineScriptMethod::s_openUrlInDefaultBrowser );
        pybind::def_functor( _kernel, "openMail", nodeScriptMethod, &EngineScriptMethod::s_openMail );
        pybind::def_functor( _kernel, "openDeleteAccount", nodeScriptMethod, &EngineScriptMethod::s_openDeleteAccount );

        pybind::def_functor( _kernel, "getDefaultResourceFontName", nodeScriptMethod, &EngineScriptMethod::s_getDefaultResourceFontName );

        pybind::def_functor( _kernel, "pushMouseMove", nodeScriptMethod, &EngineScriptMethod::s_pushMouseMove );
        pybind::def_functor( _kernel, "pushMouseButtonEvent", nodeScriptMethod, &EngineScriptMethod::s_pushMouseButtonEvent );

        pybind::def_functor( _kernel, "platformEvent", nodeScriptMethod, &EngineScriptMethod::s_platformEvent );

        pybind::def_functor_kernel( _kernel, "getCompanyName", nodeScriptMethod, &EngineScriptMethod::s_getCompanyName );
        pybind::def_functor_kernel( _kernel, "getProjectName", nodeScriptMethod, &EngineScriptMethod::s_getProjectName );
        pybind::def_functor( _kernel, "getProjectCodename", nodeScriptMethod, &EngineScriptMethod::s_getProjectCodename );

        pybind::def_functor_kernel( _kernel, "getDeviceLanguage", nodeScriptMethod, &EngineScriptMethod::s_getDeviceLanguage );

        pybind::def_functor( _kernel, "getDefaultSceneRenderCamera2D", nodeScriptMethod, &EngineScriptMethod::s_getDefaultSceneRenderCamera2D );
        pybind::def_functor( _kernel, "getDefaultRenderViewport2D", nodeScriptMethod, &EngineScriptMethod::s_getDefaultRenderViewport2D );
        pybind::def_functor( _kernel, "getDefaultArrowRenderCamera2D", nodeScriptMethod, &EngineScriptMethod::s_getDefaultArrowRenderCamera2D );

        pybind::enum_<EAnalyticsEventParameterType>( _kernel, "EAnalyticsEventParameterType" )
            .def( "EAEPT_BOOLEAN", EAEPT_BOOLEAN )
            .def( "EAEPT_INTEGER", EAEPT_INTEGER )
            .def( "EAEPT_DOUBLE", EAEPT_DOUBLE )
            .def( "EAEPT_STRING", EAEPT_STRING )
            .def( "EAEPT_CONSTSTRING", EAEPT_CONSTSTRING )
            ;

        pybind::interface_<AnalyticsContextInterface, pybind::bases<ServantInterface>>( _kernel, "AnalyticsContextInterface" )
            .def_proxy_static( "addParameterBoolean", nodeScriptMethod, &EngineScriptMethod::s_AnalyticsContext_addParameterBoolean )
            .def_proxy_static( "addParameterInteger", nodeScriptMethod, &EngineScriptMethod::s_AnalyticsContext_addParameterInteger )
            .def_proxy_static( "addParameterDouble", nodeScriptMethod, &EngineScriptMethod::s_AnalyticsContext_addParameterDouble )
            .def_proxy_static( "addParameterString", nodeScriptMethod, &EngineScriptMethod::s_AnalyticsContext_addParameterString )
            .def_proxy_static_args( "addParameterGetterBoolean", nodeScriptMethod, &EngineScriptMethod::s_AnalyticsContext_addParameterGetterBoolean )
            .def_proxy_static_args( "addParameterGetterInteger", nodeScriptMethod, &EngineScriptMethod::s_AnalyticsContext_addParameterGetterInteger )
            .def_proxy_static_args( "addParameterGetterDouble", nodeScriptMethod, &EngineScriptMethod::s_AnalyticsContext_addParameterGetterDouble )
            .def_proxy_static_args( "addParameterGetterString", nodeScriptMethod, &EngineScriptMethod::s_AnalyticsContext_addParameterGetterString )
            ;

        pybind::def_functor( _kernel, "analyticsMakeContext", nodeScriptMethod, &EngineScriptMethod::s_analyticsMakeContext );
        pybind::def_functor( _kernel, "analyticsAddGlobalContextParameterBoolean", nodeScriptMethod, &EngineScriptMethod::s_addAnalyticsGlobalContextParameterBoolean );
        pybind::def_functor( _kernel, "analyticsAddGlobalContextParameterString", nodeScriptMethod, &EngineScriptMethod::s_addAnalyticsGlobalContextParameterString );
        pybind::def_functor( _kernel, "analyticsAddGlobalContextParameterInteger", nodeScriptMethod, &EngineScriptMethod::s_addAnalyticsGlobalContextParameterInteger );
        pybind::def_functor( _kernel, "analyticsAddGlobalContextParameterDouble", nodeScriptMethod, &EngineScriptMethod::s_addAnalyticsGlobalContextParameterDouble );
        pybind::def_functor_deprecated( _kernel, "analyticsCustomEvent", nodeScriptMethod, &EngineScriptMethod::s_analyticsEvent, "use analyticsEvent" );
        pybind::def_functor( _kernel, "analyticsEvent", nodeScriptMethod, &EngineScriptMethod::s_analyticsEvent );
        pybind::def_functor( _kernel, "analyticsScreenView", nodeScriptMethod, &EngineScriptMethod::s_analyticsScreenView );
        pybind::def_functor( _kernel, "analyticsEventFlush", nodeScriptMethod, &EngineScriptMethod::s_analyticsEventFlush );

        pybind::def_functor_deprecated( _kernel, "mountResourcePak", nodeScriptMethod, &EngineScriptMethod::s_mountResourcePackage, "use 'mountResourcePackage'" );
        pybind::def_functor_deprecated( _kernel, "unmountResourcePak", nodeScriptMethod, &EngineScriptMethod::s_unmountResourcePackage, "use 'unmountResourcePackage'" );
        pybind::def_functor( _kernel, "mountResourcePackage", nodeScriptMethod, &EngineScriptMethod::s_mountResourcePackage );
        pybind::def_functor( _kernel, "unmountResourcePackage", nodeScriptMethod, &EngineScriptMethod::s_unmountResourcePackage );
        pybind::def_functor( _kernel, "hasPackage", nodeScriptMethod, &EngineScriptMethod::s_hasPackage );

        pybind::def_functor( _kernel, "existFile", nodeScriptMethod, &EngineScriptMethod::s_existFile );
        pybind::def_functor( _kernel, "removeFile", nodeScriptMethod, &EngineScriptMethod::s_removeFile );
        pybind::def_functor_kernel( _kernel, "parseXml", nodeScriptMethod, &EngineScriptMethod::s_parseXml );

        pybind::def_functor( _kernel, "visitFonts", nodeScriptMethod, &EngineScriptMethod::s_visitFonts );
        pybind::def_functor_kernel( _kernel, "getFonts", nodeScriptMethod, &EngineScriptMethod::s_getFonts );
        pybind::def_functor( _kernel, "hasFont", nodeScriptMethod, &EngineScriptMethod::s_hasFont );
        pybind::def_functor( _kernel, "validateFont", nodeScriptMethod, &EngineScriptMethod::s_validateFont );

        pybind::def_functor( _kernel, "cacheResources", nodeScriptMethod, &EngineScriptMethod::s_cacheResources );
        pybind::def_functor( _kernel, "uncacheResources", nodeScriptMethod, &EngineScriptMethod::s_uncacheResources );

        pybind::def_functor( _kernel, "rotateToTrimetric", nodeScriptMethod, &EngineScriptMethod::s_rotateToTrimetric );
        pybind::def_functor( _kernel, "rotateToTrimetric2", nodeScriptMethod, &EngineScriptMethod::s_rotateToTrimetric2 );

        pybind::def_functor( _kernel, "updateUserWallpaper", nodeScriptMethod, &EngineScriptMethod::s_updateUserWallpaper );
        pybind::def_functor( _kernel, "copyUserPicture", nodeScriptMethod, &EngineScriptMethod::s_copyUserPicture );
        pybind::def_functor( _kernel, "copyUserMusic", nodeScriptMethod, &EngineScriptMethod::s_copyUserMusic );

        pybind::def_functor( _kernel, "screenToWorldPoint", nodeScriptMethod, &EngineScriptMethod::s_screenToWorldPoint );
        pybind::def_functor( _kernel, "screenToWorldClick", nodeScriptMethod, &EngineScriptMethod::s_screenToWorldClick );

        pybind::interface_<InputMousePositionProviderInterface, pybind::bases<Mixin>>( _kernel, "InputMousePositionProvider" )
            ;

        pybind::def_functor_args( _kernel, "addMousePositionProvider", nodeScriptMethod, &EngineScriptMethod::s_addMousePositionProvider );
        pybind::def_functor( _kernel, "removeMousePositionProvider", nodeScriptMethod, &EngineScriptMethod::s_removeMousePositionProvider );

        pybind::def_functor( _kernel, "gridBurnTransparency", nodeScriptMethod, &EngineScriptMethod::s_gridBurnTransparency );

        pybind::def_functor( _kernel, "affectorUIWheelOfFortune", nodeScriptMethod, &EngineScriptMethod::s_affectorUIWheelOfFortune );

        pybind::def_functor_args( _kernel, "createAffector", nodeScriptMethod, &EngineScriptMethod::s_createAffector );
        pybind::def_functor_args( _kernel, "addAffector", nodeScriptMethod, &EngineScriptMethod::s_addAffector );
        pybind::def_functor( _kernel, "removeAffector", nodeScriptMethod, &EngineScriptMethod::s_removeAffector );

        pybind::interface_<EngineScriptMethod::AffectorFollower, pybind::bases<Affector>>( _kernel, "AffectorFollower" )
            .def( "follow", &EngineScriptMethod::AffectorFollower::follow )
            ;

        pybind::def_functor( _kernel, "addNodeFollowerLocalAlpha", nodeScriptMethod, &EngineScriptMethod::s_addNodeFollowerLocalAlpha );
        pybind::def_functor( _kernel, "addShapeFollowerCustomSize", nodeScriptMethod, &EngineScriptMethod::s_addShapeFollowerCustomSize );
        pybind::def_functor( _kernel, "addShapeFollowerTextureUVScale", nodeScriptMethod, &EngineScriptMethod::s_addShapeFollowerTextureUVScale );
        pybind::def_functor( _kernel, "removeNodeFollower", nodeScriptMethod, &EngineScriptMethod::s_removeNodeFollower );
        pybind::def_functor( _kernel, "findNodeScene", nodeScriptMethod, &EngineScriptMethod::s_findNodeScene );
        pybind::def_functor( _kernel, "getCameraPosition", nodeScriptMethod, &EngineScriptMethod::s_getCameraPosition );

        pybind::interface_<PythonValueFollower, pybind::bases<Affector, Scriptable>>( _kernel, "PythonValueFollower" )
            ;

        pybind::struct_<InputSpecialData>( _kernel, "InputSpecialData" )
            .def_member( "isAlt", &InputSpecialData::isAlt )
            .def_member( "isShift", &InputSpecialData::isShift )
            .def_member( "isControl", &InputSpecialData::isControl )
            .def_member( "isSpecial", &InputSpecialData::isSpecial )
            ;

        pybind::struct_<InputKeyEvent>( _kernel, "InputKeyEvent" )
            .def_member( "special", &InputKeyEvent::special )
            .def_member( "x", &InputKeyEvent::x )
            .def_member( "y", &InputKeyEvent::y )
            .def_member( "pressure", &InputKeyEvent::pressure )
            .def_member( "code", &InputKeyEvent::code )
            .def_member( "isDown", &InputKeyEvent::isDown )
            .def_member( "isRepeat", &InputKeyEvent::isRepeat )
            ;

        pybind::struct_<InputTextEvent>( _kernel, "InputTextEvent" )
            .def_member( "special", &InputTextEvent::special )
            .def_member( "x", &InputTextEvent::x )
            .def_member( "y", &InputTextEvent::y )
            .def_member( "pressure", &InputTextEvent::pressure )
            .def_property_static( "symbol", &EngineScriptMethod::s_InputTextEvent_getSymbol, nullptr )
            .def_property_static( "text", &EngineScriptMethod::s_InputTextEvent_getText, nullptr )
            ;

        pybind::struct_<InputAccelerometerEvent>( _kernel, "InputAccelerometerEvent" )
            .def_member( "special", &InputAccelerometerEvent::special )
            .def_member( "dx", &InputAccelerometerEvent::dx )
            .def_member( "dy", &InputAccelerometerEvent::dy )
            .def_member( "dz", &InputAccelerometerEvent::dz )
            ;

        pybind::struct_<InputMouseButtonEvent>( _kernel, "InputMouseButtonEvent" )
            .def_member( "special", &InputMouseButtonEvent::special )
            .def_member( "touchId", &InputMouseButtonEvent::touchId )
            .def_member( "x", &InputMouseButtonEvent::x )
            .def_member( "y", &InputMouseButtonEvent::y )
            .def_member( "pressure", &InputMouseButtonEvent::pressure )
            .def_member( "button", &InputMouseButtonEvent::button )
            .def_member( "isDown", &InputMouseButtonEvent::isDown )
            .def_member( "isPressed", &InputMouseButtonEvent::isPressed )
            ;

        pybind::struct_<InputMouseWheelEvent>( _kernel, "InputMouseWheelEvent" )
            .def_member( "special", &InputMouseWheelEvent::special )
            .def_member( "x", &InputMouseWheelEvent::x )
            .def_member( "y", &InputMouseWheelEvent::y )
            .def_member( "pressure", &InputMouseWheelEvent::pressure )
            .def_member( "wheel", &InputMouseWheelEvent::wheel )
            .def_member( "scroll", &InputMouseWheelEvent::scroll )
            ;

        pybind::struct_<InputMouseMoveEvent>( _kernel, "InputMouseMoveEvent" )
            .def_member( "special", &InputMouseMoveEvent::special )
            .def_member( "touchId", &InputMouseMoveEvent::touchId )
            .def_member( "x", &InputMouseMoveEvent::x )
            .def_member( "y", &InputMouseMoveEvent::y )
            .def_member( "pressure", &InputMouseMoveEvent::pressure )
            .def_member( "dx", &InputMouseMoveEvent::dx )
            .def_member( "dy", &InputMouseMoveEvent::dy )
            .def_member( "dpressure", &InputMouseMoveEvent::dpressure )
            ;

        pybind::struct_<InputMouseEnterEvent>( _kernel, "InputMouseEnterEvent" )
            .def_member( "special", &InputMouseEnterEvent::special )
            .def_member( "touchId", &InputMouseEnterEvent::touchId )
            .def_member( "x", &InputMouseEnterEvent::x )
            .def_member( "y", &InputMouseEnterEvent::y )
            .def_member( "pressure", &InputMouseEnterEvent::pressure )
            ;

        pybind::struct_<InputMouseLeaveEvent>( _kernel, "InputMouseLeaveEvent" )
            .def_member( "special", &InputMouseLeaveEvent::special )
            .def_member( "touchId", &InputMouseLeaveEvent::touchId )
            .def_member( "x", &InputMouseLeaveEvent::x )
            .def_member( "y", &InputMouseLeaveEvent::y )
            .def_member( "pressure", &InputMouseLeaveEvent::pressure )
            ;

        pybind::interface_<PythonValueFollowerLinear, pybind::bases<PythonValueFollower>>( _kernel, "PythonValueFollowerLinear" )
            .def( "setSpeed", &PythonValueFollowerLinear::setSpeed )
            .def( "getSpeed", &PythonValueFollowerLinear::getSpeed )
            .def( "setValue", &PythonValueFollowerLinear::setValue )
            .def( "getValue", &PythonValueFollowerLinear::getValue )
            .def( "setFollow", &PythonValueFollowerLinear::setFollow )
            .def( "getFollow", &PythonValueFollowerLinear::getFollow )
            .def( "resetValue", &PythonValueFollowerLinear::resetValue )
            ;

        pybind::interface_<PythonValueFollowerAcceleration, pybind::bases<PythonValueFollower>>( _kernel, "PythonValueFollowerAcceleration" )
            .def( "setSpeed", &PythonValueFollowerAcceleration::setSpeed )
            .def( "getSpeed", &PythonValueFollowerAcceleration::getSpeed )
            .def( "setAcceleration", &PythonValueFollowerAcceleration::setAcceleration )
            .def( "getAcceleration", &PythonValueFollowerAcceleration::getAcceleration )
            .def( "setValue", &PythonValueFollowerAcceleration::setValue )
            .def( "getValue", &PythonValueFollowerAcceleration::getValue )
            .def( "setFollow", &PythonValueFollowerAcceleration::setFollow )
            .def( "getFollow", &PythonValueFollowerAcceleration::getFollow )
            .def( "resetValue", &PythonValueFollowerAcceleration::resetValue )
            ;

        pybind::def_functor_args( _kernel, "createValueFollowerLinear", nodeScriptMethod, &EngineScriptMethod::s_createValueFollowerLinear );
        pybind::def_functor_args( _kernel, "createValueFollowerAcceleration", nodeScriptMethod, &EngineScriptMethod::s_createValueFollowerAcceleration );
        pybind::def_functor( _kernel, "destroyValueFollower", nodeScriptMethod, &EngineScriptMethod::s_destroyValueFollower );

        m_implement = nodeScriptMethod;

        Helper::registerScriptWrapping<PythonValueFollowerLinear>( _kernel, MENGINE_DOCUMENT_FACTORABLE );
        Helper::registerScriptWrapping<PythonValueFollowerAcceleration>( _kernel, MENGINE_DOCUMENT_FACTORABLE );

        if( PROTOTYPE_SERVICE()
            ->addPrototype( Affector::getFactorableType(), PythonValueFollowerLinear::getFactorableType(), Helper::makeFactorableUnique<ScriptablePrototypeGenerator<PythonValueFollowerLinear, 32>>( MENGINE_DOCUMENT_FACTORABLE ) ) == false )
        {
            return false;
        }

        if( PROTOTYPE_SERVICE()
            ->addPrototype( Affector::getFactorableType(), PythonValueFollowerAcceleration::getFactorableType(), Helper::makeFactorableUnique<ScriptablePrototypeGenerator<PythonValueFollowerAcceleration, 32>>( MENGINE_DOCUMENT_FACTORABLE ) ) == false )
        {
            return false;
        }

        pybind::interface_<RandomizerInterface, pybind::bases<Mixin>>( _kernel, "Randomizer" )
            .def( "setSeed", &RandomizerInterface::setSeed )
            .def( "getRandom", &RandomizerInterface::getRandom32 )
            .def( "getRandomRange", &RandomizerInterface::getRandomRange32 )
            .def( "getRandomf", &RandomizerInterface::getRandomf )
            .def( "getRandomRangef", &RandomizerInterface::getRandomRangef )
            ;

        pybind::interface_<SecureValueInterface, pybind::bases<Mixin>>( _kernel, "SecureValueInterface" )
            .def_proxy_static( "saveHexadecimal", nodeScriptMethod, &EngineScriptMethod::s_SecureValueInterface_saveHexadecimal )
            .def_proxy_static( "loadHexadecimal", nodeScriptMethod, &EngineScriptMethod::s_SecureValueInterface_loadHexadecimal )
            ;

        pybind::interface_<SecureUnsignedValue, pybind::bases<SecureValueInterface>>( _kernel, "SecureUnsignedValue" )
            .def( "setupSecureValue", &SecureUnsignedValue::setupSecureValue )
            .def( "setUnprotectedValue", &SecureUnsignedValue::setUnprotectedValue )
            .def_proxy_static_kernel( "getUnprotectedValue", nodeScriptMethod, &EngineScriptMethod::s_SecureUnsignedValue_getUnprotectedValue )
            .def( "additiveSecureValue", &SecureUnsignedValue::additiveSecureValue )
            .def( "substractSecureValue", &SecureUnsignedValue::substractSecureValue )
            .def( "additive2SecureValue", &SecureUnsignedValue::additive2SecureValue )
            .def_proxy_static_kernel( "cmpSecureValue", nodeScriptMethod, &EngineScriptMethod::s_SecureUnsignedValue_cmpSecureValue )
            ;

        pybind::def_functor_deprecated( _kernel, "makeSecureValue", nodeScriptMethod, &EngineScriptMethod::s_makeSecureUnsignedValue, "use makeSecureUnsignedValue" );
        pybind::def_functor( _kernel, "makeSecureUnsignedValue", nodeScriptMethod, &EngineScriptMethod::s_makeSecureUnsignedValue );

        pybind::interface_<SecureStringValue, pybind::bases<SecureValueInterface>>( _kernel, "SecureStringValue" )
            .def( "setUnprotectedValue", &SecureStringValue::setUnprotectedValue )
            .def_proxy_static_kernel( "getUnprotectedValue", nodeScriptMethod, &EngineScriptMethod::s_SecureStringValue_getUnprotectedValue )
            .def_proxy_static_kernel( "cmpSecureValue", nodeScriptMethod, &EngineScriptMethod::s_SecureStringValue_cmpSecureValue )
            ;

        pybind::def_functor( _kernel, "makeSecureStringValue", nodeScriptMethod, &EngineScriptMethod::s_makeSecureStringValue );

        pybind::interface_<PythonFileLogger, pybind::bases<Mixin>>( _kernel, "PythonFileLogger" )
            .def_call( &PythonFileLogger::write )
            .def( "setTimestamp", &PythonFileLogger::setTimestamp )
            .def( "getTimestamp", &PythonFileLogger::getTimestamp )
            ;

        pybind::def_functor( _kernel, "makeFileLogger", nodeScriptMethod, &EngineScriptMethod::s_makeFileLogger );

        pybind::interface_<SettingInterface, pybind::bases<Mixin>>( _kernel, "SettingInterface" )
            .def( "getName", &SettingInterface::getName )
            ;

        pybind::def_functor_kernel( _kernel, "getSettings", nodeScriptMethod, &EngineScriptMethod::s_getSettings );

        pybind::def_functor( _kernel, "setAvailablePlugin", nodeScriptMethod, &EngineScriptMethod::s_setAvailablePlugin );
        pybind::def_functor( _kernel, "isAvailablePlugin", nodeScriptMethod, &EngineScriptMethod::s_isAvailablePlugin );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void EngineScriptEmbedding::eject( pybind::kernel_interface * _kernel )
    {
        MENGINE_UNUSED( _kernel );

        EngineScriptMethodPtr nodeScriptMethod = m_implement;
        nodeScriptMethod->finalize();

        m_implement = nullptr;

        Helper::unregisterScriptWrapping<PythonValueFollowerLinear>();
        Helper::unregisterScriptWrapping<PythonValueFollowerAcceleration>();

        Helper::removeAffectorPrototype<PythonValueFollowerLinear>();
        Helper::removeAffectorPrototype<PythonValueFollowerAcceleration>();
    }
    //////////////////////////////////////////////////////////////////////////
}
