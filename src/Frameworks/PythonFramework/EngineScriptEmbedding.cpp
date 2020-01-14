#include "EngineScriptEmbedding.h"

#include "Config/Lambda.h"

#include "Interface/ApplicationInterface.h"
#include "Interface/FileServiceInterface.h"
#include "Interface/TimelineServiceInterface.h"
#include "Interface/StringizeServiceInterface.h"
#include "Interface/ThreadServiceInterface.h"
#include "Interface/InputServiceInterface.h"
#include "Interface/MemoryServiceInterface.h"
#include "Interface/PrefetcherServiceInterface.h"
#include "Interface/PlatformInterface.h"
#include "Interface/PackageServiceInterface.h"
#include "Interface/RandomizerInterface.h"
#include "Interface/PluginServiceInterface.h"
#include "Interface/CodecServiceInterface.h"
#include "Interface/ScriptServiceInterface.h"
#include "Interface/SchedulerInterface.h"
#include "Interface/TextServiceInterface.h"
#include "Interface/AccountInterface.h"
#include "Interface/ResourceServiceInterface.h"
#include "Interface/ModuleServiceInterface.h"
#include "Interface/SceneServiceInterface.h"
#include "Interface/PrototypeServiceInterface.h"
#include "Interface/PlayerServiceInterface.h"
#include "Interface/ConfigServiceInterface.h"

#include "Kernel/ThreadTask.h"
#include "Kernel/Scene.h"
#include "Kernel/Arrow.h"
#include "Kernel/MT19937Randomizer.h"
#include "Kernel/InputServiceHelper.h"
#include "Kernel/FileStreamHelper.h"
#include "Kernel/UnicodeHelper.h"
#include "Kernel/AssertionResourceType.h"

#include "Engine/ResourceFile.h"
#include "Engine/ResourceImageDefault.h"
#include "Engine/ResourceImageSubstractRGBAndAlpha.h"
#include "Engine/ResourceImageSubstract.h"
#include "Engine/ResourceImageSequence.h"
#include "Engine/ResourceImageSolid.h"
#include "Engine/ResourceImageData.h"
#include "Engine/ResourceSound.h"
#include "Engine/ResourceTestPick.h"
#include "Engine/ResourceHIT.h"
#include "Engine/ResourceShape.h"

#include "Plugins/ResourceValidatePlugin/ResourceValidateServiceInterface.h"
#include "Plugins/MoviePlugin/ResourceMovie2.h"

#include "Interface/ApplicationInterface.h"
#include "Interface/PickerServiceInterface.h"

#include "Engine/HotSpot.h"
#include "Engine/HotSpotPolygon.h"
#include "Engine/HotSpotCircle.h"
#include "Engine/HotSpotBubbles.h"
#include "Engine/HotSpotImage.h"
#include "Engine/HotSpotShape.h"
#include "Engine/Landscape2D.h"
#include "Engine/Grid2D.h"

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

#include "Engine/SurfaceSound.h"
#include "Engine/SurfaceImage.h"
#include "Engine/SurfaceImageSequence.h"
#include "Engine/SurfaceSolidColor.h"


#include "Kernel/RenderViewport.h"
#include "Kernel/RenderScissor.h"
#include "Kernel/RenderCameraOrthogonal.h"
#include "Kernel/RenderCameraProjection.h"
#include "Kernel/RenderCameraOrthogonalTarget.h"

#include "Kernel/ResourceImage.h"
#include "Kernel/PolygonHelper.h"

#include "Engine/Meshget.h"
#include "Engine/Isometric.h"
#include "Engine/Window.h"


#include "Kernel/Shape.h"
#include "Kernel/Entity.h"
#include "Kernel/Layer.h"

#include "Kernel/Logger.h"
#include "Kernel/Document.h"

#include "Interface/RenderServiceInterface.h"

#include "Kernel/Identity.h"
#include "Kernel/Affector.h"
#include "Kernel/ThreadTask.h"
#include "Kernel/DefaultPrototypeGenerator.h"
#include "Kernel/ScriptablePrototypeGenerator.h"
#include "Kernel/AssertionMemoryPanic.h"

#include "Environment/Python/PythonEventReceiver.h"
#include "Environment/Python/PythonScriptWrapper.h"

#include "PythonValueFollowerLinear.h"
#include "PythonValueFollowerAcceleration.h"

#include "PythonEntityBehavior.h"
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
#include "Kernel/Polygon.h"
#include "Kernel/ValueFollower.h"

#include "Kernel/FactoryPool.h"
#include "Kernel/AssertionFactory.h"

#include "pybind/stl/stl_type_cast.hpp"
#include "stdex/xml_sax_parser.h"
#include "utf8.h"

#include <math.h>

namespace Mengine
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
            m_factoryPyInputMousePositionProvider = nullptr;
        }

    public:
        //////////////////////////////////////////////////////////////////////////
        Polygon s_polygon_wm( Node * _node, const Polygon & _polygon )
        {
            const mt::mat4f & wm = _node->getWorldMatrix();

            Polygon polygon;
            _polygon.mul_wm( polygon, wm );

            return polygon;
        }
        //////////////////////////////////////////////////////////////////////////
        Polygon s_polygon_anchor( const Polygon & _polygon, const mt::vec2f & _anchor )
        {
            Polygon polygon;
            _polygon.transpose( polygon, _anchor );

            return polygon;
        }
        //////////////////////////////////////////////////////////////////////////
        bool s_testHotspot( HotSpotPolygon * _left, HotSpotPolygon * _right )
        {
            const Polygon & left_poligon = _left->getPolygon();
            const mt::mat4f & left_wm = _left->getWorldMatrix();

            const Polygon & right_poligon = _right->getPolygon();
            const mt::mat4f & right_wm = _right->getWorldMatrix();

            Polygon left_polygon_wm;
            left_poligon.mul_wm( left_polygon_wm, left_wm );

            Polygon right_polygon_wm;
            right_poligon.mul_wm( right_polygon_wm, right_wm );

            bool result = Helper::intersects( left_polygon_wm, right_polygon_wm );

            return result;
        }
        //////////////////////////////////////////////////////////////////////////
        bool s_loadPlugin( const WString & _pluginName )
        {
            Char utf8_pluginName[MENGINE_MAX_PATH];
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
        FactoryPtr m_factoryPythonScheduleTiming;
        //////////////////////////////////////////////////////////////////////////
        FactoryPtr m_factoryPythonSchedulePipe;
        //////////////////////////////////////////////////////////////////////////
        FactoryPtr m_factoryDelaySchedulePipe;
        //////////////////////////////////////////////////////////////////////////
        FactoryPtr m_factoryPythonScheduleEvent;
        //////////////////////////////////////////////////////////////////////////
        uint32_t s_timing( float _delay, const pybind::object & _timing, const pybind::object & _event, const pybind::args & _args )
        {
            const SchedulerInterfacePtr & tm = PLAYER_SERVICE()
                ->getScheduler();

            DelaySchedulePipePtr py_pipe = m_factoryDelaySchedulePipe->createObject( MENGINE_DOCUMENT_PYBIND );
            py_pipe->initialize( _delay );

            PythonScheduleTimingPtr py_timing = m_factoryPythonScheduleTiming->createObject( MENGINE_DOCUMENT_PYBIND );
            py_timing->initialize( _timing, _args );

            PythonScheduleEventPtr py_event = m_factoryPythonScheduleEvent->createObject( MENGINE_DOCUMENT_PYBIND );
            py_event->initialize( _event, _args );

            uint32_t id = tm->timing( py_pipe, py_timing, py_event );

            return id;
        }
        //////////////////////////////////////////////////////////////////////////
        bool s_timingRemove( uint32_t _id )
        {
            const SchedulerInterfacePtr & tm = PLAYER_SERVICE()
                ->getScheduler();

            bool successful = tm->remove( _id );

            return successful;
        }
        //////////////////////////////////////////////////////////////////////////
        SchedulerInterfacePtr s_createScheduler( const ConstString & _name )
        {
            SchedulerInterfacePtr sm = PLAYER_SERVICE()
                ->createSchedulerManager( _name );

            return sm;
        }
        //////////////////////////////////////////////////////////////////////////
        bool s_destroyScheduler( const SchedulerInterfacePtr & _sm )
        {
            MENGINE_ASSERTION_MEMORY_PANIC( _sm, false, "destroy scheduler is NULL" );

            bool successful = PLAYER_SERVICE()
                ->destroySchedulerManager( _sm );

            return successful;
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
                ->getScheduler();

            uint32_t id = sm->timing( py_pipe, py_timing, py_event );

            return id;
        }
        //////////////////////////////////////////////////////////////////////////
        uint32_t s_schedule( float _timing, const pybind::object & _script, const pybind::args & _args )
        {
            const SchedulerInterfacePtr & sm = PLAYER_SERVICE()
                ->getScheduler();

            PythonScheduleEventPtr sl = m_factoryPythonScheduleEvent->createObject( MENGINE_DOCUMENT_PYBIND );
            sl->initialize( _script, _args );

            uint32_t id = sm->event( _timing, sl );

            return id;
        }
        //////////////////////////////////////////////////////////////////////////
        bool s_scheduleRemove( uint32_t _id )
        {
            const SchedulerInterfacePtr & sm = PLAYER_SERVICE()
                ->getScheduler();

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
                ->getScheduler();

            if( sm == nullptr )
            {
                return;
            }

            sm->removeAll();
        }
        //////////////////////////////////////////////////////////////////////////
        bool s_scheduleFreeze( uint32_t _id, bool _freeze )
        {
            const SchedulerInterfacePtr & sm = PLAYER_SERVICE()
                ->getScheduler();

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
                ->getScheduler();

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
                ->getScheduler();

            if( sm == nullptr )
            {
                return;
            }

            sm->freezeAll( false );
        }
        //////////////////////////////////////////////////////////////////////////
        bool s_scheduleIsFreeze( uint32_t _id )
        {
            const SchedulerInterfacePtr & sm = PLAYER_SERVICE()
                ->getScheduler();

            if( sm == nullptr )
            {
                return false;
            }

            bool result = sm->isFreeze( _id );

            return result;
        }
        //////////////////////////////////////////////////////////////////////////
        float s_scheduleTime( uint32_t _id )
        {
            const SchedulerInterfacePtr & sm = PLAYER_SERVICE()
                ->getScheduler();

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

            MENGINE_ASSERTION_MEMORY_PANIC( sm, 0 );

            PythonScheduleEventPtr sl = m_factoryPythonScheduleEvent->createObject( MENGINE_DOCUMENT_PYBIND );
            sl->initialize( _cb, _args );

            uint32_t id = sm->event( _timing, sl );

            return id;
        }
        //////////////////////////////////////////////////////////////////////////
        bool s_scheduleGlobalRemove( uint32_t _id )
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
        bool s_scheduleGlobalFreeze( uint32_t _id, bool _freeze )
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
        bool s_scheduleGlobalIsFreeze( uint32_t _id )
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
        float s_scheduleGlobalPassed( uint32_t _id )
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
        float s_scheduleGlobalLeft( uint32_t _id )
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
        void s_setTimeFactor( float _factor )
        {
            GAME_SERVICE()
                ->setTimeFactor( _factor );
        }
        //////////////////////////////////////////////////////////////////////////
        void s_addHomeless( Node * _node )
        {
            _node->removeFromParent();
            _node->disable();
        }
        //////////////////////////////////////////////////////////////////////////
        bool s_isHomeless( Node * _node )
        {
            return _node->hasParent() == false;
        }
        //////////////////////////////////////////////////////////////////////////
        float s_getTimeFactor()
        {
            float factor = GAME_SERVICE()
                ->getTimeFactor();

            return factor;
        }
        //////////////////////////////////////////////////////////////////////////
        class PythonSceneChangeCallback
            : public SceneChangeCallbackInterface
            , public Factorable
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
        bool createCurrentScene( const ConstString & _prototype, const ConstString & _name, bool _immediately, bool _destroyOld, const pybind::object & _cb, const pybind::args & _args )
        {
            PythonSceneChangeCallbackPtr py_cb = nullptr;

            if( _cb.is_none() == false )
            {
                if( _cb.is_callable() == false )
                {
                    LOGGER_ERROR( "prototype '%s' name '%s' cb '%s' not callable"
                        , _prototype.c_str()
                        , _name.c_str()
                        , _cb.repr()
                    );

                    return false;
                }

                LOGGER_INFO( "set current scene '%s'"
                    , _name.c_str()
                );

                py_cb = m_factoryPythonSceneChangeCallback->createObject( MENGINE_DOCUMENT_PYBIND );
                py_cb->initialize( _cb, _args );
            }

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
                    ->generatePrototype( STRINGIZE_STRING_LOCAL( "Scene" ), _prototype, MENGINE_DOCUMENT_PYBIND );

                MENGINE_ASSERTION_MEMORY_PANIC( scene, false );

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
        bool setCurrentScene( const ScenePtr & _scene, const ConstString & _name, bool _immediately, bool _destroyOld, const pybind::object & _cb, const pybind::args & _args )
        {
            PythonSceneChangeCallbackPtr py_cb = nullptr;

            if( _cb.is_none() == false )
            {
                if( _cb.is_callable() == false )
                {
                    LOGGER_ERROR( "scene name '%s' cb '%s' not callable"
                        , _name.c_str()
                        , _cb.repr()
                    );

                    return false;
                }

                LOGGER_INFO( "set current scene '%s'"
                    , _name.c_str()
                );

                py_cb = m_factoryPythonSceneChangeCallback->createObject( MENGINE_DOCUMENT_PYBIND );
                py_cb->initialize( _cb, _args );
            }

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
        const ScenePtr & getCurrentScene()
        {
            const ScenePtr & scene = SCENE_SERVICE()
                ->getCurrentScene();

            return scene;
        }
        //////////////////////////////////////////////////////////////////////////
        ScenePtr s_createScene( const ConstString & _name, const pybind::object & _type )
        {
            ScenePtr scene = PROTOTYPE_SERVICE()
                ->generatePrototype( STRINGIZE_STRING_LOCAL( "Node" ), STRINGIZE_STRING_LOCAL( "Scene" ), MENGINE_DOCUMENT_PYBIND );

            MENGINE_ASSERTION_MEMORY_PANIC( scene, nullptr );

            scene->setName( _name );

            if( _type.is_none() == false )
            {
                EventablePtr eventable = SCRIPT_SERVICE()
                    ->eventableEntity( _type );

                MENGINE_ASSERTION_MEMORY_PANIC( eventable, nullptr, "scene '%s' invalid eventable '%s'"
                    , _name.c_str()
                    , _type.repr()
                );

                scene->setBehaviorEventable( eventable );

                pybind::object py_scene = _type.call();

                if( py_scene.is_invalid() == true )
                {
                    LOGGER_ERROR( "scene '%s' invalid create type '%s'"
                        , _name.c_str()
                        , _type.repr()
                    );

                    return nullptr;
                }

                PythonEntityBehaviorPtr behavior = Helper::makeFactorableUnique<PythonEntityBehavior>( MENGINE_DOCUMENT_PYBIND );
                behavior->setScriptObject( py_scene );

                scene->setBehavior( behavior );
            }

            scene->onCreate();

            return scene;
        }
        //////////////////////////////////////////////////////////////////////////
        bool createGlobalScene()
        {
            bool successful = SCENE_SERVICE()
                ->createGlobalScene();

            return successful;
        }
        //////////////////////////////////////////////////////////////////////////
        void removeGlobalScene()
        {
            SCENE_SERVICE()
                ->removeGlobalScene();
        }
        //////////////////////////////////////////////////////////////////////////
        const ScenePtr & getGlobalScene()
        {
            const ScenePtr & scene = SCENE_SERVICE()
                ->getGlobalScene();

            return scene;
        }
        //////////////////////////////////////////////////////////////////////////
        void s_setArrow( const ConstString & _prototype )
        {
            ArrowPtr arrow = PROTOTYPE_SERVICE()
                ->generatePrototype( STRINGIZE_STRING_LOCAL( "Arrow" ), _prototype, MENGINE_DOCUMENT_PYBIND );

            MENGINE_ASSERTION_MEMORY_PANIC_VOID( arrow, "Error: can't setup arrow '%s'"
                , _prototype.c_str()
            );

            PLAYER_SERVICE()
                ->setArrow( arrow );
        }
        //////////////////////////////////////////////////////////////////////////
        const pybind::object & s_getArrow()
        {
            const ArrowPtr & arrow = PLAYER_SERVICE()
                ->getArrow();

            if( arrow == nullptr )
            {
                return pybind::object::get_invalid();
            }

            const PythonEntityBehaviorPtr & behavior = arrow->getBehavior();
            const pybind::object & py_arrow = behavior->getScriptObject();

            return py_arrow;
        }
        //////////////////////////////////////////////////////////////////////////
        void s_hideArrow( bool _hide )
        {
            const ArrowPtr & arrow = PLAYER_SERVICE()
                ->getArrow();

            arrow->setLocalHide( _hide );
        }
        //////////////////////////////////////////////////////////////////////////
        const Resolution & s_getCurrentResolution()
        {
            const Resolution & resolution = APPLICATION_SERVICE()
                ->getCurrentResolution();

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

            const ArrowPtr & arrow = PLAYER_SERVICE()
                ->getArrow();

            if( arrow == nullptr )
            {
                return false;
            }

            _layer->addChild( arrow );

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

            const Viewport & vp = rv->getViewport();

            return vp.begin;
        }
        //////////////////////////////////////////////////////////////////////////
        void s_destroyNode( const NodePtr & _node )
        {
            MENGINE_ASSERTION_MEMORY_PANIC_VOID( _node, "invalid take None object" );

            _node->disable();
            _node->removeFromParent();
            _node->unwrap();
        }
        //////////////////////////////////////////////////////////////////////////
        NodePtr s_createNode( const ConstString & _type )
        {
            NodePtr node = PROTOTYPE_SERVICE()
                ->generatePrototype( STRINGIZE_STRING_LOCAL( "Node" ), _type, MENGINE_DOCUMENT_PYBIND );

            MENGINE_ASSERTION_MEMORY_PANIC( node, nullptr );

            return node;
        }
        //////////////////////////////////////////////////////////////////////////
        SurfacePtr s_createSurface( const ConstString & _type )
        {
            SurfacePtr surface = PROTOTYPE_SERVICE()
                ->generatePrototype( STRINGIZE_STRING_LOCAL( "Surface" ), _type, MENGINE_DOCUMENT_PYBIND );

            MENGINE_ASSERTION_MEMORY_PANIC( surface, nullptr );

            return surface;
        }
        //////////////////////////////////////////////////////////////////////////
        RandomizerInterfacePtr s_generateRandomizer( const ConstString & _prototype )
        {
            RandomizerInterfacePtr randomizer = PROTOTYPE_SERVICE()
                ->generatePrototype( STRINGIZE_STRING_LOCAL( "Randomizer" ), _prototype, MENGINE_DOCUMENT_PYBIND );

            MENGINE_ASSERTION_MEMORY_PANIC( randomizer, nullptr );

            return randomizer;
        }
        //////////////////////////////////////////////////////////////////////////
        ShapeQuadFixedPtr s_createSprite( const ConstString & _name, const ResourcePtr & _resource )
        {
            MENGINE_ASSERTION_MEMORY_PANIC( _resource, nullptr, "'%s' resource is NULL"
                , _name.c_str()
            );

            SurfaceImagePtr surface = PROTOTYPE_SERVICE()
                ->generatePrototype( STRINGIZE_STRING_LOCAL( "Surface" ), STRINGIZE_STRING_LOCAL( "SurfaceImage" ), MENGINE_DOCUMENT_PYBIND );

            MENGINE_ASSERTION_MEMORY_PANIC( surface, nullptr );

            surface->setName( _name );
            surface->setResourceImage( _resource );

            ShapeQuadFixedPtr shape = PROTOTYPE_SERVICE()
                ->generatePrototype( STRINGIZE_STRING_LOCAL( "Node" ), STRINGIZE_STRING_LOCAL( "ShapeQuadFixed" ), MENGINE_DOCUMENT_PYBIND );

            MENGINE_ASSERTION_MEMORY_PANIC( shape, nullptr );

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
            ResourcePtr resource = RESOURCE_SERVICE()
                ->createResource( ConstString::none(), ConstString::none(), ConstString::none(), _type, false, false, MENGINE_DOCUMENT_PYBIND );

            MENGINE_ASSERTION_MEMORY_PANIC( resource, nullptr, "invalid create resource '%s'"
                , _type.c_str()
            );

            return resource;
        }
        //////////////////////////////////////////////////////////////////////////
        bool s_directResourceCompile( const ConstString & _nameResource )
        {
            ResourcePtr resource;
            if( RESOURCE_SERVICE()
                ->hasResource( _nameResource, &resource ) == false )
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
                ->hasResource( _nameResource, &resource ) == false )
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
            bool successful = TEXT_SERVICE()
                ->directFontCompile( _fontName );

            return successful;
        }
        //////////////////////////////////////////////////////////////////////////
        bool s_directFontRelease( const ConstString & _fontName )
        {
            bool successful = TEXT_SERVICE()
                ->directFontRelease( _fontName );

            return successful;
        }
        //////////////////////////////////////////////////////////////////////////
        const ResourcePtr & s_getResource( const ConstString & _name )
        {
            const ResourcePtr & resource = RESOURCE_SERVICE()
                ->getResource( _name );

            MENGINE_ASSERTION_MEMORY_PANIC( resource, ResourcePtr::none(), "not exist resource '%s'"
                , _name.c_str()
            );

            return resource;
        }
        //////////////////////////////////////////////////////////////////////////
        const ResourcePtr & s_getResourceReference( const ConstString & _name )
        {
            const ResourcePtr & resource = RESOURCE_SERVICE()
                ->getResourceReference( _name );

            MENGINE_ASSERTION_MEMORY_PANIC( resource, ResourcePtr::none(), "not exist resource '%s'"
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
                ->getResource( _resource );

            MENGINE_ASSERTION_MEMORY_PANIC_VOID( resource, "image resource not getting '%s'"
                , _resource.c_str()
            );

            const FileGroupInterfacePtr & fileGroup = FILE_SERVICE()
                ->getFileGroup( STRINGIZE_STRING_LOCAL( "user" ) );

            const RenderTextureInterfacePtr & texture = resource->getTexture();

            RENDERTEXTURE_SERVICE()
                ->saveImage( texture, fileGroup, STRINGIZE_STRING_LOCAL( "pngImage" ), _filePath );
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
            mt::vec2f maxSize;

            if( _maxSize.x < 0.f || _maxSize.y < 0.f )
            {
                const FileGroupInterfacePtr & fileGroup = FILE_SERVICE()
                    ->getFileGroup( _fileGroupName );

                InputStreamInterfacePtr stream = Helper::openInputStreamFile( fileGroup, _filePath, false, false, MENGINE_DOCUMENT_PYBIND );

                MENGINE_ASSERTION_MEMORY_PANIC( stream, nullptr );

                ConstString codecType = CODEC_SERVICE()
                    ->findCodecType( _filePath );

                ImageDecoderInterfacePtr imageDecoder = CODEC_SERVICE()
                    ->createDecoderT<ImageDecoderInterfacePtr>( codecType, MENGINE_DOCUMENT_PYBIND );

                MENGINE_ASSERTION_MEMORY_PANIC( imageDecoder, nullptr );

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

            ResourceImageDefaultPtr resource = RESOURCE_SERVICE()
                ->createResource( ConstString::none(), ConstString::none(), ConstString::none(), STRINGIZE_STRING_LOCAL( "ResourceImageDefault" ), false, false, MENGINE_DOCUMENT_PYBIND );

            MENGINE_ASSERTION_MEMORY_PANIC( resource, nullptr );

            resource->setName( _resourceName );

            mt::uv4f uv_image;
            mt::uv4f uv_alpha;

            if( resource->setup( _filePath, ConstString::none(), uv_image, uv_alpha, maxSize ) == false )
            {
                return nullptr;
            }

            return resource;
        }
        //////////////////////////////////////////////////////////////////////////
        ResourceImageSolidPtr s_createImageSolidResource( const ConstString & _resourceName, const Color & _color, const mt::vec2f & _maxSize )
        {
            ResourceImageSolidPtr resource = RESOURCE_SERVICE()
                ->createResource( ConstString::none(), ConstString::none(), ConstString::none(), STRINGIZE_STRING_LOCAL( "ResourceImageSolid" ), false, false, MENGINE_DOCUMENT_PYBIND );

            MENGINE_ASSERTION_MEMORY_PANIC( resource, nullptr );

            resource->setName( _resourceName );

            resource->setColor( _color );

            resource->setMaxSize( _maxSize );
            resource->setSize( _maxSize );

            return resource;
        }
        //////////////////////////////////////////////////////////////////////////
        void s_minimizeWindow()
        {
            APPLICATION_SERVICE()
                ->minimizeWindow();
        }
        //////////////////////////////////////////////////////////////////////////
        bool s_calcMouseScreenPosition( const mt::vec2f & _pos, mt::vec2f * _screen )
        {
            const ArrowPtr & arrow = PLAYER_SERVICE()
                ->getArrow();

            if( arrow == nullptr )
            {
                return false;
            }

            const RenderCameraInterfacePtr & renderCamera = arrow->getRenderCamera();
            const RenderViewportInterfacePtr & renderViewport = arrow->getRenderViewport();

            arrow->calcMouseScreenPosition( renderCamera, renderViewport, _pos, _screen );

            return true;
        }
        //////////////////////////////////////////////////////////////////////////
        void s_pushMouseMove( uint32_t _touchId, const mt::vec2f & _pos )
        {
            const mt::vec2f & cp = INPUT_SERVICE()
                ->getCursorPosition( _touchId );

            mt::vec2f pos_screen;
            this->s_calcMouseScreenPosition( _pos, &pos_screen );

            mt::vec2f mp = pos_screen - cp;

            Helper::pushMouseMoveEvent( _touchId, cp.x, cp.y, mp.x, mp.y, 0.f );
        }
        //////////////////////////////////////////////////////////////////////////
        void s_pushMouseButtonEvent( uint32_t _touchId, const mt::vec2f & _pos, EMouseCode _code, bool _isDown )
        {
            mt::vec2f pos_screen;
            this->s_calcMouseScreenPosition( _pos, &pos_screen );

            Helper::pushMouseButtonEvent( _touchId, pos_screen.x, pos_screen.y, _code, 0.f, _isDown );
        }
        //////////////////////////////////////////////////////////////////////////
        void s_platformEvent( const ConstString & _event, const MapWParams & _params )
        {
            PLATFORM_SERVICE()
                ->onEvent( _event, _params );
        }
        //////////////////////////////////////////////////////////////////////////
        const ConstString & s_getProjectCodename()
        {
            const ConstString & codename = APPLICATION_SERVICE()
                ->getProjectCodename();

            return codename;
        }
        //////////////////////////////////////////////////////////////////////////
        void s_sleep( uint32_t _time )
        {
            PLATFORM_SERVICE()
                ->sleep( _time );
        }
        //////////////////////////////////////////////////////////////////////////
        bool s_mountResourcePackage( const ConstString & _fileGroupName
            , const ConstString & _name
            , const ConstString & _type
            , const ConstString & _category
            , const FilePath & _path
            , const FilePath & _descriptionPath
        )
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

            LOGGER_MESSAGE( "mount resource package name '%s' type '%s' category '%s' path '%s'"
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

            bool result = fileGroup->existFile( _filePath, true );

            return result;
        }
        //////////////////////////////////////////////////////////////////////////
        bool s_removeFile( const FilePath & _filePath )
        {
            bool result = PLATFORM_SERVICE()
                ->removeFile( _filePath.c_str() );

            return result;
        }
        //////////////////////////////////////////////////////////////////////////
        class PythonSaxCallback
        {
        public:
            PythonSaxCallback( pybind::kernel_interface * _kernel, const pybind::object & _cb )
                : m_kernel( _kernel )
                , m_cb( _cb )
            {
            }

            ~PythonSaxCallback()
            {
            }

        protected:
            PythonSaxCallback( const PythonSaxCallback & );
            void operator = ( const PythonSaxCallback & );

        public:
            void callback_begin_node( const char * _node )
            {
                pybind::object py_cb_begin = m_cb.get_attr( "begin" );

                py_cb_begin.call( _node );
            }

            void callback_node_attributes( const char * _node, uint32_t _count, const char ** _keys, const char ** _values )
            {
                MENGINE_UNUSED( _node );

                pybind::dict py_attr( m_kernel, _count );

                for( uint32_t i = 0; i != _count; ++i )
                {
                    const char * key = _keys[i];
                    const char * value = _values[i];

                    py_attr[key] = value;
                }

                pybind::object py_cb_attr = m_cb.get_attr( "attr" );

                py_cb_attr.call( py_attr );
            }

            void callback_end_node( const char * _node )
            {
                MENGINE_UNUSED( _node );

                pybind::object py_cb_end = m_cb.get_attr( "end" );

                py_cb_end.call();
            }

        protected:
            pybind::kernel_interface * m_kernel;
            pybind::object m_cb;
        };
        //////////////////////////////////////////////////////////////////////////
        bool s_parseXml( pybind::kernel_interface * _kernel, const ConstString & _fileGroupName, const FilePath & _filePath, const pybind::object & _cb )
        {
            const FileGroupInterfacePtr & fileGroup = FILE_SERVICE()
                ->getFileGroup( _fileGroupName );

            MENGINE_ASSERTION_MEMORY_PANIC( fileGroup, false );

            MemoryInterfacePtr binary_buffer = Helper::createMemoryCacheFileString( fileGroup, _filePath, false, false, MENGINE_DOCUMENT_PYBIND );

            MENGINE_ASSERTION_MEMORY_PANIC( binary_buffer, false );

            char * memory = binary_buffer->getBuffer();

            PythonSaxCallback pysc( _kernel, _cb );
            if( stdex::xml_sax_parse( memory, pysc ) == false )
            {
                return false;
            }

            return true;
        }
        //////////////////////////////////////////////////////////////////////////
        void s_visitFonts( const pybind::object & _cb )
        {
            TEXT_SERVICE()
                ->foreachFonts( [_cb]( const TextFontInterfacePtr & _font )
            {
                const ConstString & name = _font->getName();

                _cb.call( name );
            } );
        }
        //////////////////////////////////////////////////////////////////////////
        pybind::list s_getFonts( pybind::kernel_interface * _kernel )
        {
            pybind::list l( _kernel );

            TEXT_SERVICE()
                ->foreachFonts( [&l]( const TextFontInterfacePtr & _font )
            {
                const ConstString & name = _font->getName();

                l.append( name );
            } );

            return l;
        }
        //////////////////////////////////////////////////////////////////////////
        bool s_hasFont( const ConstString & _fontName )
        {
            TextFontInterfacePtr font;
            bool has = TEXT_SERVICE()
                ->existFont( _fontName, &font );

            return has;
        }
        //////////////////////////////////////////////////////////////////////////
        bool s_validateFont( const ConstString & _fontName, const String & _text )
        {
            TextFontInterfacePtr font;
            if( TEXT_SERVICE()
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
        class CacheResourceVisitor
            : public Visitor
            , public Factorable
            , public ConcreteVisitor<Resource>
        {
        public:
            CacheResourceVisitor()
            {
            }

            ~CacheResourceVisitor() override
            {
            }

        protected:
            bool filterResource( Resource * _resource ) const
            {
                if( _resource->isCompile() == false )
                {
                    return false;
                }

                return true;
            }

        protected:
            void accept( Resource * _resource ) override
            {
                if( this->filterResource( _resource ) == false )
                {
                    return;
                }

                _resource->cache();
            }
        };
        //////////////////////////////////////////////////////////////////////////
        typedef IntrusivePtr<CacheResourceVisitor> CacheResourceVisitorPtr;
        //////////////////////////////////////////////////////////////////////////
        void s_cacheResources( const ConstString & _groupName )
        {
            CacheResourceVisitorPtr rv_gac = Helper::makeFactorableUnique<CacheResourceVisitor>( MENGINE_DOCUMENT_PYBIND );

            RESOURCE_SERVICE()
                ->visitGroupResources( _groupName, rv_gac );
        }
        //////////////////////////////////////////////////////////////////////////
        class UncacheResourceVisitor
            : public Visitor
            , public Factorable
            , public ConcreteVisitor<Resource>
        {
        public:
            UncacheResourceVisitor()
            {
            }

            ~UncacheResourceVisitor() override
            {
            }

        protected:
            bool filterResource( Resource * _resource ) const
            {
                if( _resource->isCompile() == false )
                {
                    return false;
                }

                if( _resource->isCache() == false )
                {
                    return false;
                }

                return true;
            }

        protected:
            void accept( Resource * _resource ) override
            {
                if( this->filterResource( _resource ) == false )
                {
                    return;
                }

                _resource->uncache();
            }
        };
        //////////////////////////////////////////////////////////////////////////
        typedef IntrusivePtr<UncacheResourceVisitor> UncacheResourceVisitorPtr;
        //////////////////////////////////////////////////////////////////////////
        void s_uncacheResources( const ConstString & _groupName )
        {
            UncacheResourceVisitorPtr rv_gac = Helper::makeFactorableUnique<UncacheResourceVisitor>( MENGINE_DOCUMENT_PYBIND );

            RESOURCE_SERVICE()
                ->visitGroupResources( _groupName, rv_gac );
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

                float abs_angle_length = ::fabsf( angle_length );

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
        Polygon s_hotspotCorrect( HotSpotPolygon * _base, const VectorHotSpotPolygons & _overlap )
        {
            MENGINE_UNUSED( _base );

            Polygon correct_polygon;

            for( const HotSpotPolygonPtr & overlap_hotspot : _overlap )
            {
                const Polygon & overlap_polygon = overlap_hotspot->getPolygon();

                VectorGeolygon output;
                Helper::intersection( correct_polygon, overlap_polygon, output );

                if( output.empty() == true )
                {
                    return Polygon();
                }

                correct_polygon = output[0].getOuter();
            }

            return correct_polygon;
        }
        //////////////////////////////////////////////////////////////////////////
        bool s_copyFile_( const ConstString & _resourceFilePath, const MemoryBufferInterfacePtr & _memory )
        {
            MENGINE_ASSERTION_RESOURCE_TYPE( _resourceFilePath, ResourceFilePtr, false, "resource '%s' type does not match 'ResourceFile'"
                , _resourceFilePath.c_str()
            );

            const ResourceFilePtr & resourceFile = RESOURCE_SERVICE()
                ->getResource( _resourceFilePath );

            MENGINE_ASSERTION_MEMORY_PANIC( resourceFile, false );

            const ContentInterface * content = resourceFile->getContent();

            const FilePath & filePath = content->getFilePath();
            const FileGroupInterfacePtr & fileGroup = content->getFileGroup();

            InputStreamInterfacePtr stream = Helper::openInputStreamFile( fileGroup, filePath, false, false, MENGINE_DOCUMENT_PYBIND );

            if( stream == nullptr )
            {
                return false;
            }

            size_t size = stream->size();

            void * memory_buffer = _memory->newBuffer( size );

            if( stream->read( memory_buffer, size ) != size )
            {
                return false;
            }

            resourceFile->release();

            return true;
        }
        //////////////////////////////////////////////////////////////////////////
        bool s_updateUserWallpaper( const String & _filePath )
        {
            Char projectName[MENGINE_APPLICATION_PROJECT_MAXNAME + 1] = {'\0'};
            APPLICATION_SERVICE()
                ->getProjectName( projectName );

            strcat( projectName, "/" );

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

            MENGINE_ASSERTION_MEMORY_PANIC( memory, false );

            if( this->s_copyFile_( _resourceFilePath, memory ) == false )
            {
                return false;
            }

            Char projectName[MENGINE_APPLICATION_PROJECT_MAXNAME + 1] = {'\0'};
            APPLICATION_SERVICE()
                ->getProjectName( projectName );

            strcat( projectName, "/" );

            if( PLATFORM_SERVICE()
                ->createDirectoryUserPicture( projectName, _filePath.c_str(), memory->getBuffer(), memory->getSize() ) == false )
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

            MENGINE_ASSERTION_MEMORY_PANIC( memory, false );

            if( this->s_copyFile_( _resourceFilePath, memory ) == false )
            {
                return false;
            }

            Char projectName[MENGINE_APPLICATION_PROJECT_MAXNAME + 1] = {'\0'};
            APPLICATION_SERVICE()
                ->getProjectName( projectName );

            strcat( projectName, "/" );

            if( PLATFORM_SERVICE()
                ->createDirectoryUserMusic( projectName, _filePath.c_str(), memory->getBuffer(), memory->getSize() ) == false )
            {
                return false;
            }

            return true;
        }
        //////////////////////////////////////////////////////////////////////////
        class PyInputMousePositionProvider
            : public Factorable
            , public InputMousePositionProviderInterface
        {
        public:
            PyInputMousePositionProvider()
                : m_arrow( nullptr )
                , m_renderCamera( nullptr )
                , m_renderViewport( nullptr )
            {
            }

        public:
            void initialize( const ArrowPtr & _arrow, const RenderCameraInterfacePtr & _camera, const RenderViewportInterfacePtr & _viewport, const pybind::object & _cb, const pybind::args & _args )
            {
                m_arrow = _arrow;
                m_renderCamera = _camera;
                m_renderViewport = _viewport;
                m_cb = _cb;
                m_args = _args;
            }

        protected:
            void onMousePositionChange( uint32_t _touchId, const mt::vec2f & _position, float _pressure ) override
            {
                MENGINE_UNUSED( _pressure );

                mt::vec2f wp;
                m_arrow->calcMouseWorldPosition( m_renderCamera, m_renderViewport, _position, &wp );

                mt::vec3f v3( wp.x, wp.y, 0.f );

                m_cb.call_args( _touchId, v3, m_args );
            }

        protected:
            ArrowPtr m_arrow;
            RenderCameraInterfacePtr m_renderCamera;
            RenderViewportInterfacePtr m_renderViewport;
            pybind::object m_cb;
            pybind::args m_args;
        };
        //////////////////////////////////////////////////////////////////////////
        typedef IntrusivePtr<PyInputMousePositionProvider> PyInputMousePositionProviderPtr;
        //////////////////////////////////////////////////////////////////////////
        FactoryPtr m_factoryPyInputMousePositionProvider;
        //////////////////////////////////////////////////////////////////////////
        uint32_t s_addMousePositionProvider( const ArrowPtr & _arrow, const RenderCameraInterfacePtr & _camera, const RenderViewportInterfacePtr & _viewport, const pybind::object & _cb, const pybind::args & _args )
        {
            ArrowPtr arrow = _arrow;
            if( arrow == nullptr )
            {
                arrow = PLAYER_SERVICE()
                    ->getArrow();

                if( arrow == nullptr )
                {
                    return 0;
                }
            }

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
            provider->initialize( arrow, camera, viewport, _cb, _args );

            uint32_t id = INPUT_SERVICE()
                ->addMousePositionProvider( provider );

            return id;
        }
        //////////////////////////////////////////////////////////////////////////
        void s_removeMousePositionProvider( uint32_t _id )
        {
            INPUT_SERVICE()
                ->removeMousePositionProvider( _id );
        }
        //////////////////////////////////////////////////////////////////////////
        mt::vec2f s_screenToWorldPoint( const ArrowPtr & _arrow, const RenderCameraInterfacePtr & _camera, const RenderViewportInterfacePtr & _viewport, const mt::vec2f & _screenPoint )
        {
            ArrowPtr arrow = _arrow;

            if( arrow == nullptr )
            {
                arrow = PLAYER_SERVICE()
                    ->getArrow();

                if( arrow == nullptr )
                {
                    return mt::vec2f( 0.f, 0.f );
                }
            }

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

            mt::vec2f wp;
            arrow->calcMouseWorldPosition( camera, viewport, _screenPoint, &wp );

            return wp;
        }
        //////////////////////////////////////////////////////////////////////////
        mt::vec2f s_screenToWorldClick( const ArrowPtr & _arrow, const RenderCameraInterfacePtr & _camera, const RenderViewportInterfacePtr & _viewport, const mt::vec2f & _screenPoint )
        {
            ArrowPtr arrow = _arrow;

            if( arrow == nullptr )
            {
                arrow = PLAYER_SERVICE()
                    ->getArrow();

                if( arrow == nullptr )
                {
                    return mt::vec2f( 0.f, 0.f );
                }
            }

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

            mt::vec2f wp;
            arrow->calcPointClick( camera, viewport, _screenPoint, &wp );

            return wp;
        }
        //////////////////////////////////////////////////////////////////////////
        class AffectorGridBurnTransparency
            : public Affector
        {
        public:
            AffectorGridBurnTransparency()
                : m_pos( 0.f, 0.f )
                , m_duration( 0.f )
                , m_radius( 0.f )
                , m_ellipse( 0.f )
                , m_penumbra( 0.f )
                , m_time( 0.f )
            {
            }

        public:
            void initialize( const Grid2DPtr & _grid, const mt::vec2f & _pos, float _duration, float _radius, float _ellipse, float _penumbra, const pybind::object & _cb )
            {
                m_grid = _grid;
                m_pos = _pos;
                m_duration = _duration;
                m_radius = _radius;
                m_ellipse = _ellipse;
                m_penumbra = _penumbra;
                m_cb = _cb;
            }

        protected:
            bool _affect( const UpdateContext * _context, float * _used ) override
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

                float pos_step_x = width / float( countX - 1 );
                float pos_step_y = height / float( countY - 1 );

                for( uint32_t j = 0; j != countY; ++j )
                {
                    for( uint32_t i = 0; i != countX; ++i )
                    {
                        mt::vec2f grid_pos( pos_step_x * i, pos_step_y * j );

                        mt::vec2f v = m_pos - grid_pos;

                        v.y /= m_ellipse;

                        float pos_sqrdistance = v.sqrlength();

                        if( pos_sqrdistance < radius * radius )
                        {
                            m_grid->setGridColor( i, j, Color( 0x00FFFFFF ) );
                        }
                        else if( pos_sqrdistance < (radius + penumbra) * (radius + penumbra) )
                        {
                            Color cv;
                            m_grid->getGridColor( i, j, cv );

                            float cv_a = cv.getA();

                            float pos_distance = sqrtf( pos_sqrdistance );

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

            mt::vec2f m_pos;
            float m_duration;
            float m_radius;
            float m_ellipse;
            float m_penumbra;

            pybind::object m_cb;

            float m_time;
        };
        //////////////////////////////////////////////////////////////////////////
        typedef IntrusivePtr<AffectorGridBurnTransparency> AffectorGridBurnTransparencyPtr;
        //////////////////////////////////////////////////////////////////////////
        FactoryPtr m_factoryAffectorGridBurnTransparency;
        //////////////////////////////////////////////////////////////////////////
        uint32_t s_gridBurnTransparency( const Grid2DPtr & _grid, const mt::vec2f & _pos, float _time, float _radius, float _ellipse, float _penumbra, const pybind::object & _cb )
        {
            AffectorGridBurnTransparencyPtr affector = m_factoryAffectorGridBurnTransparency->createObject( MENGINE_DOCUMENT_PYBIND );

            affector->setAffectorType( ETA_USER );

            affector->initialize( _grid, _pos, _time, _radius, _ellipse, _penumbra, _cb );

            uint32_t id = _grid->addAffector( affector );

            return id;
        }
        //////////////////////////////////////////////////////////////////////////
        class AffectorUser
            : public Affector
        {
        public:
            bool initialize( const pybind::object & _cb, const pybind::args & _args )
            {
                m_cb = _cb;
                m_args = _args;

                return true;
            }

        protected:
            bool _affect( const UpdateContext * _context, float * _used ) override
            {
                *_used = _context->time;

                bool complete = m_cb.call_args( _context->time, m_args );

                return complete;
            }

        protected:
            pybind::object m_cb;
            pybind::args m_args;
        };
        //////////////////////////////////////////////////////////////////////////
        typedef IntrusivePtr<AffectorUser> AffectorUserPtr;
        //////////////////////////////////////////////////////////////////////////
        FactoryPtr m_factoryAffectorUser;
        //////////////////////////////////////////////////////////////////////////
        AffectorPtr s_createAffector( const pybind::object & _cb, const pybind::args & _args )
        {
            AffectorUserPtr affector = m_factoryAffectorUser->createObject( MENGINE_DOCUMENT_PYBIND );

            if( affector->initialize( _cb, _args ) == false )
            {
                return nullptr;
            }

            return affector;
        }
        //////////////////////////////////////////////////////////////////////////
        AFFECTOR_ID s_addAffector( const pybind::object & _cb, const pybind::args & _args )
        {
            AffectorUserPtr affector = m_factoryAffectorUser->createObject( MENGINE_DOCUMENT_PYBIND );

            if( affector->initialize( _cb, _args ) == false )
            {
                return 0;
            }

            const AffectorablePtr & affectorable = PLAYER_SERVICE()
                ->getGlobalAffectorable();

            AFFECTOR_ID id = affectorable->addAffector( affector );

            return id;
        }
        //////////////////////////////////////////////////////////////////////////
        bool s_removeAffector( AFFECTOR_ID _id )
        {
            const AffectorablePtr & affectorable = PLAYER_SERVICE()
                ->getGlobalAffectorable();

            bool successful = affectorable->stopAffector( _id );

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
        typedef IntrusivePtr<AffectorFollower> AffectorFollowerPtr;
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
                AFFECTOR_ID id = this->getId();

                m_affectorable->stopAffector( id );
            }

        protected:
            AffectorablePtr m_affectorable;
        };
        //////////////////////////////////////////////////////////////////////////
        template<class T_Value, class T_Setter, class T_Getter>
        class AffectorNodeFollowerMethod
            : public AffectorNodeFollower
        {
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
            bool _affect( const UpdateContext * _context, float * _used ) override
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
                m_factory = Helper::makeFactoryPool<TAffectorNodeFollowerMethod, 4>( MENGINE_DOCUMENT_FACTORABLE );

                return true;
            }

            void finalize()
            {
                MENGINE_ASSERTION_FACTORY_EMPTY( m_factory );
                m_factory = nullptr;
            }

        public:
            AffectorFollowerPtr create( const AffectorablePtr & _affectorable, const T_Setter & _setter, const T_Getter & _getter, const T_Value & _value, const T_Value & _target, float _speed, const DocumentPtr & _doc )
            {
                TAffectorNodeFollowerMethodPtr affector = m_factory->createObject( _doc );

                affector->setAffectorable( _affectorable );

                if( affector->initialize( _setter, _getter, _value, _target, _speed ) == false )
                {
                    return nullptr;
                }

                if( _affectorable->addAffector( affector ) == INVALID_AFFECTOR_ID )
                {
                    return nullptr;
                }

                return affector;
            }

        protected:
            FactoryPtr m_factory;
        };
        //////////////////////////////////////////////////////////////////////////
        IntrusivePtr<AffectorNodeFollowerCreator<Node, float>> m_creatorAffectorNodeFollowerLocalAlpha;
        //////////////////////////////////////////////////////////////////////////
        AffectorFollowerPtr s_addNodeFollowerLocalAlpha( const NodePtr & _node, float _value, float _target, float _speed )
        {
            RenderInterface * render = _node->getRender();

            MENGINE_ASSERTION_MEMORY_PANIC( render, nullptr, "node '%s' is not renderable"
                , _node->getName().c_str()
            );

            AffectorFollowerPtr affector = m_creatorAffectorNodeFollowerLocalAlpha->create( _node
                , [render]( float _alpha ) { render->setLocalAlpha( _alpha ); }
            , [render]() { return render->getLocalAlpha(); }
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
                , [_node]( const mt::vec2f & _value ) { _node->setCustomSize( _value ); }
            , [_node]() { return _node->getCustomSize(); }
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
                , [_node]( const mt::vec2f & _value ) { _node->setTextureUVScale( _value ); }
            , [_node]() { return _node->getTextureUVScale(); }
                , _value, _target, _speed
                , MENGINE_DOCUMENT_PYBIND );

            return affector;
        }
        //////////////////////////////////////////////////////////////////////////
        void s_removeNodeFollower( const AffectorFollowerPtr & _affector )
        {
            MENGINE_ASSERTION_MEMORY_PANIC_VOID( _affector, "take NULL affector" );

            _affector->stop();
        }
        //////////////////////////////////////////////////////////////////////////
        void s_moduleMessage( const ConstString & _moduleName, const ConstString & _messageName, const MapWParams & _params )
        {
            MODULE_SERVICE()
                ->message( _moduleName, _messageName, _params );
        }
        //////////////////////////////////////////////////////////////////////////
        mt::vec3f s_getCameraPosition( const RenderCameraInterfacePtr & _camera, const NodePtr & _node )
        {
            const mt::vec3f & wp = _node->getWorldPosition();

            const mt::mat4f & vm = _camera->getCameraViewMatrix();

            mt::vec3f wp_screen;
            mt::mul_v3_v3_m4( wp_screen, wp, vm );

            return wp_screen;
        }
        //////////////////////////////////////////////////////////////////////////
        PythonValueFollowerLinearPtr s_createValueFollowerLinear( float _value, float _speed, const pybind::object & _cb, const pybind::args & _args )
        {
            PythonValueFollowerLinearPtr follower = PROTOTYPE_SERVICE()
                ->generatePrototype( STRINGIZE_STRING_LOCAL( "Affector" ), STRINGIZE_STRING_LOCAL( "PythonValueFollowerLinear" ), MENGINE_DOCUMENT_PYBIND );

            follower->initialize( _value, _speed, _cb, _args );

            const AffectorablePtr & affectorable = PLAYER_SERVICE()
                ->getGlobalAffectorable();

            affectorable->addAffector( follower );

            return follower;
        }
        //////////////////////////////////////////////////////////////////////////
        PythonValueFollowerAccelerationPtr s_createValueFollowerAcceleration( float _value, float _speed, float _acceleration, const pybind::object & _cb, const pybind::args & _args )
        {
            PythonValueFollowerAccelerationPtr follower = PROTOTYPE_SERVICE()
                ->generatePrototype( STRINGIZE_STRING_LOCAL( "Affector" ), STRINGIZE_STRING_LOCAL( "PythonValueFollowerAcceleration" ), MENGINE_DOCUMENT_PYBIND );

            follower->initialize( _value, _speed, _acceleration, _cb, _args );

            const AffectorablePtr & affectorable = PLAYER_SERVICE()
                ->getGlobalAffectorable();

            affectorable->addAffector( follower );

            return follower;
        }
        //////////////////////////////////////////////////////////////////////////
        void s_destroyValueFollower( const PythonValueFollowerPtr & _follower )
        {
            AFFECTOR_ID id = _follower->getId();

            const AffectorablePtr & affectorable = PLAYER_SERVICE()
                ->getGlobalAffectorable();

            affectorable->stopAffector( id );
        }
        //////////////////////////////////////////////////////////////////////////
        Scene * s_findNodeScene( Node * _node )
        {
            Node * node_iterator = _node;

            while( node_iterator != nullptr )
            {
                Scene * node_scene = dynamic_cast<Scene *>(node_iterator);

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
            mt::vec2f wp = this->s_getTouchPosition( 0 );

            return wp;
        }
        //////////////////////////////////////////////////////////////////////////
        mt::vec2f s_getMousePosition()
        {
            mt::vec2f wp = this->s_getTouchPosition( 0 );

            return wp;
        }
        //////////////////////////////////////////////////////////////////////////
        mt::vec2f s_getTouchPosition( uint32_t _touchId )
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

            const Viewport & vp = renderViewport->getViewport();

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
            VectorPickers pickers;
            PICKER_SERVICE()
                ->pickTrap( _point, 0, 0.f, pickers );

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
        bool s_hasResource( const ConstString & _name )
        {
            return RESOURCE_SERVICE()
                ->hasResource( _name, nullptr );
        }
        //////////////////////////////////////////////////////////////////////////
        bool s_removeCurrentScene( bool _immediately, const pybind::object & _cb, const pybind::args & _args )
        {
            if( _cb.is_callable() == false )
            {
                LOGGER_ERROR( "cb '%s' not callable"
                    , _cb.repr()
                );

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
            , public Factorable
        {
        public:
            PyGlobalBaseHandler()
            {
            }

            ~PyGlobalBaseHandler() override
            {
            }

        public:
            void initialize( const pybind::object & _cb, const pybind::args & _args )
            {
                m_cb = _cb;
                m_args = _args;
            }

            void finalize()
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

        protected:
            pybind::object m_cb;
            pybind::args m_args;
        };
        //////////////////////////////////////////////////////////////////////////
        typedef IntrusivePtr<PyGlobalBaseHandler> PyGlobalBaseHandlerPtr;
        //////////////////////////////////////////////////////////////////////////
        class PyGlobalMouseLeaveHandler
            : public PyGlobalBaseHandler
        {
        protected:
            void handleMouseLeave( const InputMouseLeaveEvent & _event ) override
            {
                mt::vec2f point( _event.x, _event.y );

                mt::vec2f wp;
                PLAYER_SERVICE()
                    ->calcGlobalMouseWorldPosition( point, &wp );

                pybind::object py_result = m_cb.call_args( _event.touchId, wp.x, wp.y, m_args );

                if( py_result.is_none() == false )
                {
                    LOGGER_ERROR( "'%s' return value '%s' not None"
                        , m_cb.repr()
                        , py_result.repr()
                    );
                }
            }
        };
        //////////////////////////////////////////////////////////////////////////
        typedef IntrusivePtr<PyGlobalMouseLeaveHandler> PyGlobalMouseLeaveHandlerPtr;
        //////////////////////////////////////////////////////////////////////////
        FactoryPtr m_factoryPyGlobalMouseLeaveHandlers;
        //////////////////////////////////////////////////////////////////////////
        uint32_t s_addMouseLeaveHandler( const pybind::object & _cb, const pybind::args & _args )
        {
            const GlobalInputHandlerInterfacePtr & globalHandleSystem = PLAYER_SERVICE()
                ->getGlobalInputHandler();

            PyGlobalMouseLeaveHandlerPtr handler = m_factoryPyGlobalMouseLeaveHandlers
                ->createObject( MENGINE_DOCUMENT_PYBIND );

            handler->initialize( _cb, _args );

            uint32_t id = globalHandleSystem->addGlobalHandler( handler, MENGINE_DOCUMENT_PYBIND );

            return id;
        }
        //////////////////////////////////////////////////////////////////////////
        class PyGlobalMouseMoveHandler
            : public PyGlobalBaseHandler
        {
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

                pybind::object py_result = m_cb.call_args( _event.touchId, wp.x, wp.y, wd.x, wd.y, m_args );

                if( py_result.is_none() == false )
                {
                    LOGGER_ERROR( "'%s' return value '%s' not None"
                        , m_cb.repr()
                        , py_result.repr()
                    );
                }

                return false;
            }
        };
        //////////////////////////////////////////////////////////////////////////
        typedef IntrusivePtr<PyGlobalMouseMoveHandler> PyGlobalMouseMoveHandlerPtr;
        //////////////////////////////////////////////////////////////////////////
        FactoryPtr m_factoryPyGlobalMouseMoveHandlers;
        //////////////////////////////////////////////////////////////////////////
        uint32_t s_addMouseMoveHandler( const pybind::object & _cb, const pybind::args & _args )
        {
            const GlobalInputHandlerInterfacePtr & globalHandleSystem = PLAYER_SERVICE()
                ->getGlobalInputHandler();

            PyGlobalMouseMoveHandlerPtr handler = m_factoryPyGlobalMouseMoveHandlers
                ->createObject( MENGINE_DOCUMENT_PYBIND );

            handler->initialize( _cb, _args );

            uint32_t id = globalHandleSystem->addGlobalHandler( handler, MENGINE_DOCUMENT_PYBIND );

            return id;
        }
        //////////////////////////////////////////////////////////////////////////
        class PyGlobalMouseHandlerButton
            : public PyGlobalBaseHandler
        {
        protected:
            bool handleMouseButtonEvent( const InputMouseButtonEvent & _event ) override
            {
                mt::vec2f point( _event.x, _event.y );

                mt::vec2f wp;
                PLAYER_SERVICE()
                    ->calcGlobalMouseWorldPosition( point, &wp );

                pybind::object py_result = m_cb.call_args( _event.touchId, wp.x, wp.y, _event.code, _event.pressure, _event.isDown, _event.isPressed, m_args );

                if( py_result.is_none() == false )
                {
                    LOGGER_ERROR( "'%s' return value '%s' not None"
                        , m_cb.repr()
                        , py_result.repr()
                    );
                }

                return false;
            }
        };
        //////////////////////////////////////////////////////////////////////////
        typedef IntrusivePtr<PyGlobalMouseHandlerButton> PyGlobalMouseHandlerButtonPtr;
        //////////////////////////////////////////////////////////////////////////
        FactoryPtr m_factoryPyGlobalMouseHandlerButtons;
        //////////////////////////////////////////////////////////////////////////
        uint32_t s_addMouseButtonHandler( const pybind::object & _cb, const pybind::args & _args )
        {
            const GlobalInputHandlerInterfacePtr & globalHandleSystem = PLAYER_SERVICE()
                ->getGlobalInputHandler();

            PyGlobalMouseHandlerButtonPtr handler = m_factoryPyGlobalMouseHandlerButtons
                ->createObject( MENGINE_DOCUMENT_PYBIND );

            handler->initialize( _cb, _args );

            uint32_t id = globalHandleSystem->addGlobalHandler( handler, MENGINE_DOCUMENT_PYBIND );

            return id;
        }
        //////////////////////////////////////////////////////////////////////////
        class PyGlobalMouseHandlerButtonEnd
            : public PyGlobalBaseHandler
        {
        protected:
            //////////////////////////////////////////////////////////////////////////
            bool handleMouseButtonEventEnd( const InputMouseButtonEvent & _event ) override
            {
                mt::vec2f point( _event.x, _event.y );

                mt::vec2f wp;
                PLAYER_SERVICE()
                    ->calcGlobalMouseWorldPosition( point, &wp );

                pybind::object py_result = m_cb.call_args( _event.touchId, wp.x, wp.y, _event.code, _event.pressure, _event.isDown, _event.isPressed, m_args );

                if( py_result.is_none() == false )
                {
                    LOGGER_ERROR( "'%s' return value '%s' not None"
                        , m_cb.repr()
                        , py_result.repr()
                    );
                }

                return false;
            }
        };
        //////////////////////////////////////////////////////////////////////////
        typedef IntrusivePtr<PyGlobalMouseHandlerButtonEnd> PyGlobalMouseHandlerButtonEndPtr;
        //////////////////////////////////////////////////////////////////////////
        FactoryPtr m_factoryPyGlobalMouseHandlerButtonEnds;
        //////////////////////////////////////////////////////////////////////////
        uint32_t s_addMouseButtonHandlerEnd( const pybind::object & _cb, const pybind::args & _args )
        {
            const GlobalInputHandlerInterfacePtr & globalHandleSystem = PLAYER_SERVICE()
                ->getGlobalInputHandler();

            PyGlobalMouseHandlerButtonEndPtr handler = m_factoryPyGlobalMouseHandlerButtonEnds
                ->createObject( MENGINE_DOCUMENT_PYBIND );

            handler->initialize( _cb, _args );

            uint32_t id = globalHandleSystem->addGlobalHandler( handler, MENGINE_DOCUMENT_PYBIND );

            return id;
        }
        //////////////////////////////////////////////////////////////////////////
        class PyGlobalMouseHandlerWheel
            : public PyGlobalBaseHandler
        {
        protected:
            //////////////////////////////////////////////////////////////////////////
            bool handleMouseWheel( const InputMouseWheelEvent & _event ) override
            {
                pybind::object py_result = m_cb.call_args( _event.code, _event.x, _event.y, _event.wheel, m_args );

                if( py_result.is_none() == false )
                {
                    LOGGER_ERROR( "'%s' return value '%s' not None"
                        , m_cb.repr()
                        , py_result.repr()
                    );
                }

                return false;
            }
        };
        //////////////////////////////////////////////////////////////////////////
        typedef IntrusivePtr<PyGlobalMouseHandlerWheel> PyGlobalMouseHandlerWheelPtr;
        //////////////////////////////////////////////////////////////////////////
        FactoryPtr m_factoryPyGlobalMouseHandlerWheels;
        //////////////////////////////////////////////////////////////////////////
        uint32_t s_addMouseWheelHandler( const pybind::object & _cb, const pybind::args & _args )
        {
            const GlobalInputHandlerInterfacePtr & globalHandleSystem = PLAYER_SERVICE()
                ->getGlobalInputHandler();

            MENGINE_ASSERTION_MEMORY_PANIC( globalHandleSystem, 0 );

            const PyGlobalMouseHandlerWheelPtr & handler = m_factoryPyGlobalMouseHandlerWheels
                ->createObject( MENGINE_DOCUMENT_PYBIND );

            handler->initialize( _cb, _args );

            uint32_t id = globalHandleSystem->addGlobalHandler( handler, MENGINE_DOCUMENT_PYBIND );

            return id;
        }
        //////////////////////////////////////////////////////////////////////////
        class PyGlobalMouseHandlerButtonBegin
            : public PyGlobalBaseHandler
        {
        protected:
            //////////////////////////////////////////////////////////////////////////
            bool handleMouseButtonEventBegin( const InputMouseButtonEvent & _event ) override
            {
                mt::vec2f point( _event.x, _event.y );

                mt::vec2f wp;
                PLAYER_SERVICE()
                    ->calcGlobalMouseWorldPosition( point, &wp );

                pybind::object py_result = m_cb.call_args( _event.touchId, wp.x, wp.y, _event.code, _event.pressure, _event.isDown, _event.isPressed, m_args );

                if( py_result.is_none() == false )
                {
                    LOGGER_ERROR( "'%s' return value '%s' not None"
                        , m_cb.repr()
                        , py_result.repr()
                    );
                }

                return false;
            }
        };
        //////////////////////////////////////////////////////////////////////////
        typedef IntrusivePtr<PyGlobalMouseHandlerButtonBegin> PyGlobalMouseHandlerButtonBeginPtr;
        //////////////////////////////////////////////////////////////////////////
        FactoryPtr m_factoryPyGlobalMouseHandlerButtonBegins;
        //////////////////////////////////////////////////////////////////////////
        uint32_t s_addMouseButtonHandlerBegin( const pybind::object & _cb, const pybind::args & _args )
        {
            const GlobalInputHandlerInterfacePtr & globalHandleSystem = PLAYER_SERVICE()
                ->getGlobalInputHandler();

            MENGINE_ASSERTION_MEMORY_PANIC( globalHandleSystem, 0 );

            PyGlobalMouseHandlerButtonBeginPtr handler = m_factoryPyGlobalMouseHandlerButtonBegins
                ->createObject( MENGINE_DOCUMENT_PYBIND );

            handler->initialize( _cb, _args );

            uint32_t id = globalHandleSystem->addGlobalHandler( handler, MENGINE_DOCUMENT_PYBIND );

            return id;
        }
        //////////////////////////////////////////////////////////////////////////
        class PyGlobalKeyHandler
            : public PyGlobalBaseHandler
        {
        protected:
            bool handleKeyEvent( const InputKeyEvent & _event ) override
            {
                pybind::object py_result = m_cb.call_args( (uint32_t)_event.code, _event.x, _event.y, _event.isDown, _event.isRepeat, m_args );

                if( py_result.is_none() == false )
                {
                    LOGGER_ERROR( "'%s' return value '%s' not None"
                        , m_cb.repr()
                        , py_result.repr()
                    );
                }

                return false;
            }
        };
        //////////////////////////////////////////////////////////////////////////
        typedef IntrusivePtr<PyGlobalKeyHandler> PyGlobalKeyHandlerPtr;
        //////////////////////////////////////////////////////////////////////////
        FactoryPtr m_factoryPyGlobalKeyHandler;
        //////////////////////////////////////////////////////////////////////////
        uint32_t s_addKeyHandler( const pybind::object & _cb, const pybind::args & _args )
        {
            const GlobalInputHandlerInterfacePtr & globalHandleSystem = PLAYER_SERVICE()
                ->getGlobalInputHandler();

            MENGINE_ASSERTION_MEMORY_PANIC( globalHandleSystem, 0 );

            PyGlobalKeyHandlerPtr handler = m_factoryPyGlobalKeyHandler
                ->createObject( MENGINE_DOCUMENT_PYBIND );

            handler->initialize( _cb, _args );

            uint32_t id = globalHandleSystem->addGlobalHandler( handler, MENGINE_DOCUMENT_PYBIND );

            return id;
        }
        //////////////////////////////////////////////////////////////////////////
        class PyGlobalTextHandler
            : public PyGlobalBaseHandler
        {
        protected:
            bool handleTextEvent( const InputTextEvent & _event ) override
            {
                pybind::object py_result = m_cb.call_args( _event.key, _event.x, _event.y, m_args );

                if( py_result.is_none() == false )
                {
                    LOGGER_ERROR( "'%s' return value '%s' not None"
                        , m_cb.repr()
                        , py_result.repr()
                    );
                }

                return false;
            }
        };
        //////////////////////////////////////////////////////////////////////////
        FactoryPtr m_factoryPyGlobalTextHandler;
        //////////////////////////////////////////////////////////////////////////
        uint32_t s_addTextHandler( const pybind::object & _cb, const pybind::args & _args )
        {
            const GlobalInputHandlerInterfacePtr & globalHandleSystem = PLAYER_SERVICE()
                ->getGlobalInputHandler();

            MENGINE_ASSERTION_MEMORY_PANIC( globalHandleSystem, 0 );

            PyGlobalBaseHandlerPtr handler = m_factoryPyGlobalTextHandler
                ->createObject( MENGINE_DOCUMENT_PYBIND );

            handler->initialize( _cb, _args );

            uint32_t id = globalHandleSystem->addGlobalHandler( handler, MENGINE_DOCUMENT_PYBIND );

            return id;
        }
        //////////////////////////////////////////////////////////////////////////
        bool s_removeGlobalHandler( uint32_t _id )
        {
            const GlobalInputHandlerInterfacePtr & globalHandleSystem = PLAYER_SERVICE()
                ->getGlobalInputHandler();

            MENGINE_ASSERTION_MEMORY_PANIC( globalHandleSystem, false );

            InputHandlerInterfacePtr handler = globalHandleSystem->removeGlobalHandler( _id );

            PyGlobalBaseHandlerPtr py_handler = stdex::intrusive_dynamic_cast<PyGlobalBaseHandlerPtr>(handler);

            MENGINE_ASSERTION_MEMORY_PANIC( py_handler, false, "%d handler invalid"
                , _id
            );

            py_handler->finalize();

            return true;
        }
        //////////////////////////////////////////////////////////////////////////
        bool s_enableGlobalHandler( uint32_t _id, bool _value )
        {
            const GlobalInputHandlerInterfacePtr & globalHandleSystem = PLAYER_SERVICE()
                ->getGlobalInputHandler();

            MENGINE_ASSERTION_MEMORY_PANIC( globalHandleSystem, false );

            bool successful = globalHandleSystem->enableGlobalHandler( _id, _value );

            return successful;
        }
        //////////////////////////////////////////////////////////////////////////
        float s_getGameAspect()
        {
            float aspect;
            Viewport viewport;

            APPLICATION_SERVICE()
                ->getGameViewport( aspect, viewport );

            return aspect;
        }
        //////////////////////////////////////////////////////////////////////////
        Viewport s_getGameViewport()
        {
            float aspect;
            Viewport viewport;

            APPLICATION_SERVICE()
                ->getGameViewport( aspect, viewport );

            return viewport;
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
        PyObject * s_getGameParam( pybind::kernel_interface * _kernel, const ConstString & _paramName )
        {
            const ConfigInterfacePtr & config = CONFIG_SERVICE()
                ->getDefaultConfig();

            const Char * param_value;
            if( config->hasValue( "Params", _paramName.c_str(), &param_value ) == false )
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
            if( config->hasValue( "Params", _paramName.c_str(), &param_value ) == false )
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
            if( config->hasValue( "Params", _paramName.c_str(), &param_value ) == false )
            {
                return _kernel->ret_none();
            }

            return pybind::ptr( _kernel, param_value );
        }
        //////////////////////////////////////////////////////////////////////////
        PyObject * s_getGameParamBool( pybind::kernel_interface * _kernel, const ConstString & _paramName )
        {
            const ConfigInterfacePtr & config = CONFIG_SERVICE()
                ->getDefaultConfig();

            bool param_value;
            if( config->hasValue( "Params", _paramName.c_str(), &param_value ) == false )
            {
                return _kernel->ret_none();
            }

            return _kernel->ret_bool( param_value );
        }
        //////////////////////////////////////////////////////////////////////////
        bool s_openUrlInDefaultBrowser( const WString & _url )
        {
            Char utf8_url[4096];
            Helper::unicodeToUtf8( _url, utf8_url, 4096 );

            bool val = PLATFORM_SERVICE()
                ->openUrlInDefaultBrowser( utf8_url );

            return val;
        }
        //////////////////////////////////////////////////////////////////////////
        ConstString s_getDefaultResourceFontName()
        {
            const ConstString & defaultResourceFontName = TEXT_SERVICE()
                ->getDefaultFontName();

            return defaultResourceFontName;
        }
        //////////////////////////////////////////////////////////////////////////
        void s_visitChild( Node * _node, const pybind::object & _cb )
        {
            IntrusiveSlugListNodeChild & child = _node->getChildren();

            pybind::foreach_t( _cb, child.begin(), child.end() );
        }
        //////////////////////////////////////////////////////////////////////////
        class ResourceVisitorGetAlreadyCompiled
            : public Visitor
            , public Factorable
            , public ConcreteVisitor<Resource>
        {
        public:
            explicit ResourceVisitorGetAlreadyCompiled( const pybind::object & _cb )
                : m_cb( _cb )
            {
            }

            ~ResourceVisitorGetAlreadyCompiled() override
            {
            }

        protected:
            void accept( Resource * _resource ) override
            {
                if( _resource->isCompile() == false )
                {
                    return;
                }

                m_cb.call( _resource );
            }

        protected:
            pybind::object m_cb;
        };
        //////////////////////////////////////////////////////////////////////////
        typedef IntrusivePtr<ResourceVisitorGetAlreadyCompiled> ResourceVisitorGetAlreadyCompiledPtr;
        //////////////////////////////////////////////////////////////////////////
        void s_visitCompiledResources( const ConstString & _groupName, const pybind::object & _cb )
        {
            ResourceVisitorGetAlreadyCompiledPtr rv_gac = Helper::makeFactorableUnique<ResourceVisitorGetAlreadyCompiled>( MENGINE_DOCUMENT_PYBIND, _cb );

            RESOURCE_SERVICE()
                ->visitGroupResources( _groupName, rv_gac );
        }
        //////////////////////////////////////////////////////////////////////////
        class MyResourceVisitor
            : public Visitor
            , public Factorable
            , public ConcreteVisitor<Resource>
        {
        public:
            explicit MyResourceVisitor( const pybind::object & _cb )
                : m_cb( _cb )
            {
            }

            ~MyResourceVisitor() override
            {
            }

        protected:
            void accept( Resource * _resource ) override
            {
                m_cb.call( _resource );
            }

        protected:
            pybind::object m_cb;
        };
        //////////////////////////////////////////////////////////////////////////
        typedef IntrusivePtr<MyResourceVisitor> MyResourceVisitorPtr;
        //////////////////////////////////////////////////////////////////////////
        void s_visitResources( const ConstString & _groupName, const pybind::object & _cb )
        {
            MyResourceVisitorPtr rv_gac = Helper::makeFactorableUnique<MyResourceVisitor>( MENGINE_DOCUMENT_PYBIND, _cb );

            RESOURCE_SERVICE()
                ->visitGroupResources( _groupName, rv_gac );
        }
        //////////////////////////////////////////////////////////////////////////
        class IncrefResourceVisitor
            : public Visitor
            , public Factorable
            , public ConcreteVisitor<Resource>
        {
        protected:
            void accept( Resource * _resource ) override
            {
                _resource->compile();
            }
        };
        //////////////////////////////////////////////////////////////////////////
        typedef IntrusivePtr<IncrefResourceVisitor> IncrefResourceVisitorPtr;
        //////////////////////////////////////////////////////////////////////////
        void s_incrementResources( const ConstString & _groupName )
        {
            IncrefResourceVisitorPtr rv_gac = Helper::makeFactorableUnique<IncrefResourceVisitor>( MENGINE_DOCUMENT_PYBIND );

            RESOURCE_SERVICE()
                ->visitGroupResources( _groupName, rv_gac );
        }
        //////////////////////////////////////////////////////////////////////////
        class DecrementResourceVisitor
            : public Visitor
            , public Factorable
            , public ConcreteVisitor<Resource>
        {
        protected:
            void accept( Resource * _resource ) override
            {
                _resource->release();
            }
        };
        //////////////////////////////////////////////////////////////////////////
        typedef IntrusivePtr<DecrementResourceVisitor> DecrementResourceVisitorPtr;
        //////////////////////////////////////////////////////////////////////////
        void s_decrementResources( const ConstString & _groupName )
        {
            DecrementResourceVisitorPtr rv_gac = Helper::makeFactorableUnique<DecrementResourceVisitor>( MENGINE_DOCUMENT_PYBIND );

            RESOURCE_SERVICE()
                ->visitGroupResources( _groupName, rv_gac );
        }
        //////////////////////////////////////////////////////////////////////////
        void s_incrementResourcesTags( const Tags & _tags )
        {
            RESOURCE_SERVICE()
                ->foreachTagsResources( _tags, []( const ResourcePtr & _resource )
            {
                _resource->compile();
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
        class GetResourceVisitor
            : public Visitor
            , public Factorable
            , public ConcreteVisitor<Resource>
        {
        public:
            explicit GetResourceVisitor( pybind::kernel_interface * _kernel )
                : m_kernel( _kernel )
                , m_l( _kernel )
            {
                m_scope = m_kernel->get_class_type_scope_t<Resource>();
            }

            ~GetResourceVisitor() override
            {
            }

        public:
            const pybind::list & getResult() const
            {
                return m_l;
            }

        protected:
            void accept( Resource * _resource ) override
            {
                PyObject * py_obj = m_scope->create_holder( (void *)_resource );

                m_l.append( py_obj );
            }

        protected:
            pybind::kernel_interface * m_kernel;
            pybind::class_type_scope_interface_ptr m_scope;
            pybind::list m_l;
        };
        //////////////////////////////////////////////////////////////////////////
        typedef IntrusivePtr<GetResourceVisitor> GetResourceVisitorPtr;
        //////////////////////////////////////////////////////////////////////////
        pybind::list s_getResources( pybind::kernel_interface * _kernel, const ConstString & _groupName )
        {
            GetResourceVisitorPtr rv_gac = Helper::makeFactorableUnique<GetResourceVisitor>( MENGINE_DOCUMENT_PYBIND, _kernel );

            RESOURCE_SERVICE()
                ->visitGroupResources( _groupName, rv_gac );

            const pybind::list & l = rv_gac->getResult();

            return l;
        }
        //////////////////////////////////////////////////////////////////////////
        bool s_validResource( const ConstString & _resourceName )
        {
            const ResourcePtr & resource = RESOURCE_SERVICE()
                ->getResourceReference( _resourceName );

            MENGINE_ASSERTION_MEMORY_PANIC( resource, false, "resource '%s' not found"
                , _resourceName.c_str()
            );

            if( SERVICE_EXIST( ResourceValidateServiceInterface ) == false )
            {
                return true;
            }

            bool valid = RESOURCEVALIDATE_SERVICE()
                ->validResource( resource );

            return valid;
        }
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<EngineScriptMethod> EngineScriptMethodPtr;
    //////////////////////////////////////////////////////////////////////////
    EngineScriptEmbedding::EngineScriptEmbedding()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    EngineScriptEmbedding::~EngineScriptEmbedding()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool EngineScriptEmbedding::embedding( pybind::kernel_interface * _kernel )
    {
        EngineScriptMethodPtr nodeScriptMethod = Helper::makeFactorableUnique<EngineScriptMethod>( MENGINE_DOCUMENT_FACTORABLE );

        if( nodeScriptMethod->initialize() == false )
        {
            return false;
        }

        pybind::def_functor_args( _kernel, "createCurrentScene", nodeScriptMethod, &EngineScriptMethod::createCurrentScene );
        pybind::def_functor_args( _kernel, "setCurrentScene", nodeScriptMethod, &EngineScriptMethod::setCurrentScene );
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

        pybind::def_functor_args( _kernel, "timing", nodeScriptMethod, &EngineScriptMethod::s_timing );
        pybind::def_functor( _kernel, "timingRemove", nodeScriptMethod, &EngineScriptMethod::s_timingRemove );

        pybind::def_functor( _kernel, "createScheduler", nodeScriptMethod, &EngineScriptMethod::s_createScheduler );
        pybind::def_functor( _kernel, "destroyScheduler", nodeScriptMethod, &EngineScriptMethod::s_destroyScheduler );

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

        pybind::def_functor( _kernel, "setArrow", nodeScriptMethod, &EngineScriptMethod::s_setArrow );
        pybind::def_functor( _kernel, "getArrow", nodeScriptMethod, &EngineScriptMethod::s_getArrow );
        pybind::def_functor( _kernel, "hideArrow", nodeScriptMethod, &EngineScriptMethod::s_hideArrow );

        pybind::def_functor( _kernel, "setArrowLayer", nodeScriptMethod, &EngineScriptMethod::s_setArrowLayer );

        pybind::def_functor( _kernel, "createResource", nodeScriptMethod, &EngineScriptMethod::s_createResource );
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

        pybind::def_functor( _kernel, "hasResource", nodeScriptMethod, &EngineScriptMethod::s_hasResource );

        pybind::def_functor_args( _kernel, "removeCurrentScene", nodeScriptMethod, &EngineScriptMethod::s_removeCurrentScene );

        pybind::def_functor_args( _kernel, "addMouseLeaveHandler", nodeScriptMethod, &EngineScriptMethod::s_addMouseLeaveHandler );
        pybind::def_functor_args( _kernel, "addMouseMoveHandler", nodeScriptMethod, &EngineScriptMethod::s_addMouseMoveHandler );
        pybind::def_functor_args( _kernel, "addMouseButtonHandler", nodeScriptMethod, &EngineScriptMethod::s_addMouseButtonHandler );
        pybind::def_functor_args( _kernel, "addMouseButtonHandlerBegin", nodeScriptMethod, &EngineScriptMethod::s_addMouseButtonHandlerBegin );
        pybind::def_functor_args( _kernel, "addMouseButtonHandlerEnd", nodeScriptMethod, &EngineScriptMethod::s_addMouseButtonHandlerEnd );
        pybind::def_functor_args( _kernel, "addMouseWheelHandler", nodeScriptMethod, &EngineScriptMethod::s_addMouseWheelHandler );
        pybind::def_functor_args( _kernel, "addKeyHandler", nodeScriptMethod, &EngineScriptMethod::s_addKeyHandler );
        pybind::def_functor_args( _kernel, "addTextHandler", nodeScriptMethod, &EngineScriptMethod::s_addTextHandler );

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
        pybind::def_functor_deprecated( _kernel, "getTimingFactor", nodeScriptMethod, &EngineScriptMethod::s_getTimeFactor, "use getTimeFactor" );
        pybind::def_functor_deprecated( _kernel, "setTimingFactor", nodeScriptMethod, &EngineScriptMethod::s_setTimeFactor, "use setTimeFactor" );
        pybind::def_functor( _kernel, "getTimeFactor", nodeScriptMethod, &EngineScriptMethod::s_getTimeFactor );
        pybind::def_functor( _kernel, "setTimeFactor", nodeScriptMethod, &EngineScriptMethod::s_setTimeFactor );


        pybind::def_functor( _kernel, "addHomeless", nodeScriptMethod, &EngineScriptMethod::s_addHomeless );
        pybind::def_functor( _kernel, "isHomeless", nodeScriptMethod, &EngineScriptMethod::s_isHomeless );

        pybind::def_functor( _kernel, "cancelTask", nodeScriptMethod, &EngineScriptMethod::s_cancelTask );
        pybind::def_functor( _kernel, "joinTask", nodeScriptMethod, &EngineScriptMethod::s_joinTask );

        pybind::def_functor( _kernel, "getGameAspect", nodeScriptMethod, &EngineScriptMethod::s_getGameAspect );
        pybind::def_functor( _kernel, "getGameViewport", nodeScriptMethod, &EngineScriptMethod::s_getGameViewport );

        pybind::def_functor( _kernel, "hasGameParam", nodeScriptMethod, &EngineScriptMethod::s_hasGameParam );
        pybind::def_functor_kernel( _kernel, "getGameParam", nodeScriptMethod, &EngineScriptMethod::s_getGameParam );
        pybind::def_functor_kernel( _kernel, "getGameParamFloat", nodeScriptMethod, &EngineScriptMethod::s_getGameParamFloat );
        pybind::def_functor_kernel( _kernel, "getGameParamInt", nodeScriptMethod, &EngineScriptMethod::s_getGameParamInt );
        pybind::def_functor_kernel( _kernel, "getGameParamBool", nodeScriptMethod, &EngineScriptMethod::s_getGameParamBool );
        
        pybind::def_functor( _kernel, "openUrlInDefaultBrowser", nodeScriptMethod, &EngineScriptMethod::s_openUrlInDefaultBrowser );

        pybind::def_functor( _kernel, "getDefaultResourceFontName", nodeScriptMethod, &EngineScriptMethod::s_getDefaultResourceFontName );

        pybind::def_functor( _kernel, "pushMouseMove", nodeScriptMethod, &EngineScriptMethod::s_pushMouseMove );
        pybind::def_functor( _kernel, "pushMouseButtonEvent", nodeScriptMethod, &EngineScriptMethod::s_pushMouseButtonEvent );

        pybind::def_functor( _kernel, "platformEvent", nodeScriptMethod, &EngineScriptMethod::s_platformEvent );

        pybind::def_functor( _kernel, "getProjectCodename", nodeScriptMethod, &EngineScriptMethod::s_getProjectCodename );

        pybind::def_functor( _kernel, "sleep", nodeScriptMethod, &EngineScriptMethod::s_sleep );




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

        pybind::def_functor( _kernel, "hotspotCorrect", nodeScriptMethod, &EngineScriptMethod::s_hotspotCorrect );

        pybind::def_functor( _kernel, "updateUserWallpaper", nodeScriptMethod, &EngineScriptMethod::s_updateUserWallpaper );
        pybind::def_functor( _kernel, "copyUserPicture", nodeScriptMethod, &EngineScriptMethod::s_copyUserPicture );
        pybind::def_functor( _kernel, "copyUserMusic", nodeScriptMethod, &EngineScriptMethod::s_copyUserMusic );

        pybind::def_functor( _kernel, "screenToWorldPoint", nodeScriptMethod, &EngineScriptMethod::s_screenToWorldPoint );
        pybind::def_functor( _kernel, "screenToWorldClick", nodeScriptMethod, &EngineScriptMethod::s_screenToWorldClick );

        pybind::def_functor_args( _kernel, "addMousePositionProvider", nodeScriptMethod, &EngineScriptMethod::s_addMousePositionProvider );
        pybind::def_functor( _kernel, "removeMousePositionProvider", nodeScriptMethod, &EngineScriptMethod::s_removeMousePositionProvider );

        pybind::def_functor( _kernel, "gridBurnTransparency", nodeScriptMethod, &EngineScriptMethod::s_gridBurnTransparency );

        pybind::def_functor( _kernel, "setLocale", nodeScriptMethod, &EngineScriptMethod::s_setLocale );

        pybind::def_functor_args( _kernel, "createAffector", nodeScriptMethod, &EngineScriptMethod::s_createAffector );
        pybind::def_functor_args( _kernel, "addAffector", nodeScriptMethod, &EngineScriptMethod::s_addAffector );
        pybind::def_functor( _kernel, "removeAffector", nodeScriptMethod, &EngineScriptMethod::s_removeAffector );

        pybind::interface_<EngineScriptMethod::AffectorFollower, pybind::bases<Affector> >( _kernel, "AffectorFollower" )
            .def( "follow", &EngineScriptMethod::AffectorFollower::follow )
            ;

        pybind::def_functor( _kernel, "addNodeFollowerLocalAlpha", nodeScriptMethod, &EngineScriptMethod::s_addNodeFollowerLocalAlpha );
        pybind::def_functor( _kernel, "addShapeFollowerCustomSize", nodeScriptMethod, &EngineScriptMethod::s_addShapeFollowerCustomSize );
        pybind::def_functor( _kernel, "addShapeFollowerTextureUVScale", nodeScriptMethod, &EngineScriptMethod::s_addShapeFollowerTextureUVScale );
        pybind::def_functor( _kernel, "removeNodeFollower", nodeScriptMethod, &EngineScriptMethod::s_removeNodeFollower );
        pybind::def_functor( _kernel, "moduleMessage", nodeScriptMethod, &EngineScriptMethod::s_moduleMessage );
        pybind::def_functor( _kernel, "findNodeScene", nodeScriptMethod, &EngineScriptMethod::s_findNodeScene );
        pybind::def_functor( _kernel, "getCameraPosition", nodeScriptMethod, &EngineScriptMethod::s_getCameraPosition );

        pybind::interface_<PythonValueFollower, pybind::bases<Affector, Scriptable> >( _kernel, "PythonValueFollower" )
            ;

        pybind::interface_<PythonValueFollowerLinear, pybind::bases<PythonValueFollower> >( _kernel, "PythonValueFollowerLinear" )
            .def( "setSpeed", &PythonValueFollowerLinear::setSpeed )
            .def( "getSpeed", &PythonValueFollowerLinear::getSpeed )
            .def( "setValue", &PythonValueFollowerLinear::setValue )
            .def( "getValue", &PythonValueFollowerLinear::getValue )
            .def( "setFollow", &PythonValueFollowerLinear::setFollow )
            .def( "getFollow", &PythonValueFollowerLinear::getFollow )
            .def( "resetValue", &PythonValueFollowerLinear::resetValue )
            ;

        pybind::interface_<PythonValueFollowerAcceleration, pybind::bases<PythonValueFollower> >( _kernel, "PythonValueFollowerAcceleration" )
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

        VOCABULARY_SET( ScriptWrapperInterface, STRINGIZE_STRING_LOCAL( "ClassWrapping" ), STRINGIZE_STRING_LOCAL( "PythonValueFollowerLinear" ), Helper::makeFactorableUnique<PythonScriptWrapper<PythonValueFollowerLinear> >( MENGINE_DOCUMENT_FACTORABLE, _kernel ) );
        VOCABULARY_SET( ScriptWrapperInterface, STRINGIZE_STRING_LOCAL( "ClassWrapping" ), STRINGIZE_STRING_LOCAL( "PythonValueFollowerAcceleration" ), Helper::makeFactorableUnique<PythonScriptWrapper<PythonValueFollowerAcceleration> >( MENGINE_DOCUMENT_FACTORABLE, _kernel ) );

        if( PROTOTYPE_SERVICE()
            ->addPrototype( STRINGIZE_STRING_LOCAL( "Affector" ), STRINGIZE_STRING_LOCAL( "PythonValueFollowerLinear" ), Helper::makeFactorableUnique<ScriptablePrototypeGenerator<PythonValueFollowerLinear, 32> >( MENGINE_DOCUMENT_FACTORABLE ) ) == false )
        {
            return false;
        }

        if( PROTOTYPE_SERVICE()
            ->addPrototype( STRINGIZE_STRING_LOCAL( "Affector" ), STRINGIZE_STRING_LOCAL( "PythonValueFollowerAcceleration" ), Helper::makeFactorableUnique<ScriptablePrototypeGenerator<PythonValueFollowerAcceleration, 32> >( MENGINE_DOCUMENT_FACTORABLE ) ) == false )
        {
            return false;
        }

        pybind::interface_<RandomizerInterface, pybind::bases<Mixin> >( _kernel, "Randomizer" )
            .def( "setSeed", &RandomizerInterface::setSeed )
            .def( "getRandom", &RandomizerInterface::getRandom )
            .def( "getRandomRange", &RandomizerInterface::getRandomRange )
            .def( "getRandomf", &RandomizerInterface::getRandomf )
            .def( "getRandomRangef", &RandomizerInterface::getRandomRangef )
            ;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void EngineScriptEmbedding::ejecting( pybind::kernel_interface * _kernel )
    {
        MENGINE_UNUSED( _kernel );

        EngineScriptMethodPtr nodeScriptMethod = m_implement;
        nodeScriptMethod->finalize();

        m_implement = nullptr;

        VOCABULARY_REMOVE( STRINGIZE_STRING_LOCAL( "ClassWrapping" ), STRINGIZE_STRING_LOCAL( "PythonValueFollowerLinear" ) );
        VOCABULARY_REMOVE( STRINGIZE_STRING_LOCAL( "ClassWrapping" ), STRINGIZE_STRING_LOCAL( "PythonValueFollowerAcceleration" ) );

        PROTOTYPE_SERVICE()->removePrototype( STRINGIZE_STRING_LOCAL( "Affector" ), STRINGIZE_STRING_LOCAL( "PythonValueFollowerLinear" ) );
        PROTOTYPE_SERVICE()->removePrototype( STRINGIZE_STRING_LOCAL( "Affector" ), STRINGIZE_STRING_LOCAL( "PythonValueFollowerAcceleration" ) );
    }
}
