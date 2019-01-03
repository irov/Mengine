#include "EngineScriptWrapper.h"

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
#include "Interface/PluginInterface.h"
#include "Interface/CodecServiceInterface.h"
#include "Interface/ScriptServiceInterface.h"
#include "Interface/SchedulerInterface.h"
#include "Interface/TextServiceInterface.h"
#include "Interface/AccountInterface.h"
#include "Interface/UnicodeSystemInterface.h"
#include "Interface/ResourceServiceInterface.h"
#include "Interface/ModuleServiceInterface.h"
#include "Interface/SceneServiceInterface.h"

#include "Kernel/ThreadTask.h"
#include "Kernel/Scene.h"
#include "Kernel/Arrow.h"
#include "Kernel/MT19937Randomizer.h"

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

//#include "Light2D.h"
#include "Engine/Gyroscope.h"
#include "Engine/TextField.h"
#include "Engine/SoundEmitter.h"
#include "Engine/Point.h"
#include "Engine/Line.h"
#include "Kernel/RenderCameraProjection.h"
//#include "RigidBody3D.h"
//#include "CapsuleController.h"
//#include "Skeleton.h"

#include "ScriptHolder.h"

#include "Engine/SurfaceSound.h"
#include "Engine/SurfaceImage.h"
#include "Engine/SurfaceImageSequence.h"
#include "Engine/SurfaceSolidColor.h"


#include "Kernel/Isometric.h"
#include "Kernel/Parallax.h"
#include "Kernel/RenderViewport.h"
#include "Kernel/RenderScissor.h"
#include "Kernel/RenderCameraOrthogonal.h"
#include "Kernel/RenderCameraProjection.h"
#include "Kernel/RenderCameraOrthogonalTarget.h"

#include "Kernel/ResourceImage.h"
#include "Kernel/PolygonHelper.h"

#include "Engine/Meshget.h"

#include "Engine/Window.h"


#include "Kernel/Shape.h"
#include "Kernel/Entity.h"

//#include "DiscreteEntity.h"

//#include "SoundEngine.h"
#include "Kernel/Logger.h"

#include "Interface/RenderServiceInterface.h"

#include "Kernel/Identity.h"
#include "Kernel/Affector.h"
#include "Kernel/ThreadTask.h"
#include "Kernel/DefaultPrototypeGenerator.h"
#include "Kernel/ScriptablePrototypeGenerator.h"

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
#include "Kernel/String.h"
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
    typedef Vector<HotSpotPolygon *> VectorHotSpotPolygons;
    //////////////////////////////////////////////////////////////////////////
    class EngineScriptMethod
        : public Factorable
    {
    public:
        EngineScriptMethod()
        {
            m_factoryPythonScheduleTiming = new FactoryPool<PythonScheduleTiming, 8>();
            m_factoryPythonSchedulePipe = new FactoryPool<PythonSchedulePipe, 8>();
            m_factoryDelaySchedulePipe = new FactoryPool<DelaySchedulePipe, 8>();
            m_factoryPythonScheduleEvent = new FactoryPool<PythonScheduleEvent, 8>();
            m_factoryPythonSceneChangeCallback = new FactoryPool<PythonSceneChangeCallback, 8>();
            m_factoryAffectorGridBurnTransparency = new FactoryPool<AffectorGridBurnTransparency, 4>();
            m_factoryAffectorUser = new FactoryPool<AffectorUser, 4>();
            m_factoryPyGlobalMouseMoveHandlers = new FactoryPool<PyGlobalMouseMoveHandler, 32>();
            m_factoryPyGlobalMouseHandlerButtons = new FactoryPool<PyGlobalMouseHandlerButton, 32>();
            m_factoryPyGlobalMouseHandlerButtonEnds = new FactoryPool<PyGlobalMouseHandlerButtonEnd, 32>();
            m_factoryPyGlobalMouseHandlerWheels = new FactoryPool<PyGlobalMouseHandlerWheel, 32>();
            m_factoryPyGlobalMouseHandlerButtonBegins = new FactoryPool<PyGlobalMouseHandlerButtonBegin, 32>();
            m_factoryPyGlobalKeyHandler = new FactoryPool<PyGlobalKeyHandler, 32>();
            m_factoryPyGlobalTextHandler = new FactoryPool<PyGlobalTextHandler, 32>();
            m_factoryPyInputMousePositionProvider = new FactoryPool<PyInputMousePositionProvider, 8>();
        }

        ~EngineScriptMethod() override
        {
            MENGINE_ASSERTION_FACTORY_EMPTY( m_factoryPythonScheduleTiming );
            MENGINE_ASSERTION_FACTORY_EMPTY( m_factoryPythonSchedulePipe );
            MENGINE_ASSERTION_FACTORY_EMPTY( m_factoryDelaySchedulePipe );
            MENGINE_ASSERTION_FACTORY_EMPTY( m_factoryPythonScheduleEvent );
            MENGINE_ASSERTION_FACTORY_EMPTY( m_factoryPythonSceneChangeCallback );
            MENGINE_ASSERTION_FACTORY_EMPTY( m_factoryAffectorGridBurnTransparency );
            MENGINE_ASSERTION_FACTORY_EMPTY( m_factoryAffectorUser );
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
                ->loadPlugin( utf8_pluginName );

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
        uint32_t timing( float _delay, const pybind::object & _timing, const pybind::object & _event, const pybind::args & _args )
        {
            const SchedulerInterfacePtr & tm = PLAYER_SERVICE()
                ->getScheduler();

            DelaySchedulePipePtr py_pipe = m_factoryDelaySchedulePipe->createObject();

            py_pipe->initialize( _delay );

            PythonScheduleTimingPtr py_timing = m_factoryPythonScheduleTiming->createObject();

            py_timing->initialize( _timing, _args );

            PythonScheduleEventPtr py_event = m_factoryPythonScheduleEvent->createObject();

            py_event->initialize( _event, _args );

            uint32_t id = tm->timing( py_pipe, py_timing, py_event );

            return id;
        }
        //////////////////////////////////////////////////////////////////////////
        bool timingRemove( uint32_t _id )
        {
            const SchedulerInterfacePtr & tm = PLAYER_SERVICE()
                ->getScheduler();

            bool successful = tm->remove( _id );

            return successful;
        }
        //////////////////////////////////////////////////////////////////////////
        SchedulerInterfacePtr createScheduler( const ConstString & _name )
        {
            SchedulerInterfacePtr sm = PLAYER_SERVICE()
                ->createSchedulerManager( _name );

            return sm;
        }
        //////////////////////////////////////////////////////////////////////////
        bool destroyScheduler( const SchedulerInterfacePtr & _sm )
        {
            if( _sm == nullptr )
            {
                LOGGER_ERROR( "Mengine.destroyScheduler destroy scheduler is NULL"
                );

                return false;
            }

            bool successful = PLAYER_SERVICE()
                ->destroySchedulerManager( _sm );

            return successful;
        }
        //////////////////////////////////////////////////////////////////////////
        uint32_t schedule( float _timing, const pybind::object & _script, const pybind::args & _args )
        {
            const SchedulerInterfacePtr & sm = PLAYER_SERVICE()
                ->getScheduler();

            PythonScheduleEventPtr sl = m_factoryPythonScheduleEvent->createObject();

            sl->initialize( _script, _args );

            uint32_t id = sm->event( _timing, sl );

            return id;
        }
        //////////////////////////////////////////////////////////////////////////
        bool scheduleRemove( uint32_t _id )
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
        void scheduleRemoveAll()
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
        void scheduleResumeAll()
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

            return sm->isFreeze( _id );
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

            if( sm == nullptr )
            {
                return 0;
            }

            PythonScheduleEventPtr sl = m_factoryPythonScheduleEvent->createObject();

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
            _node->release();
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
        bool setCurrentScene( const ConstString & _prototype, const ConstString & _name, bool _destroyOld, const pybind::object & _cb, const pybind::args & _args )
        {
            if( _cb.is_callable() == false )
            {
                LOGGER_ERROR( "setCurrentScene prototype '%s' name '%s' cb '%s' not callable"
                    , _prototype.c_str()
                    , _name.c_str()
                    , _cb.repr()
                );

                return false;
            }

            LOGGER_INFO( "set current scene '%s'"
                , _name.c_str()
            );

            //if( PLAYER_SERVICE()
            //	->isChangedScene() == true )
            //{
            //	return false;
            //}

            PythonSceneChangeCallbackPtr py_cb = m_factoryPythonSceneChangeCallback->createObject();

            py_cb->initialize( _cb, _args );

            ScenePtr currentScene = SCENE_SERVICE()
                ->getCurrentScene();

            if( currentScene != nullptr && currentScene->getName() == _name )
            {
                if( SCENE_SERVICE()
                    ->restartCurrentScene( py_cb ) == false )
                {
                    return false;
                }
            }
            else
            {
                ScenePtr scene = PROTOTYPE_SERVICE()
                    ->generatePrototype( STRINGIZE_STRING_LOCAL( "Scene" ), _prototype );

                if( scene == nullptr )
                {
                    return false;
                }

                scene->setName( _name );

                if( SCENE_SERVICE()
                    ->setCurrentScene( scene, _destroyOld, py_cb ) == false )
                {
                    scene = nullptr;

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
        ScenePtr s_createScene( const ConstString & _prototype, const ConstString & _name )
        {
            ScenePtr scene = PROTOTYPE_SERVICE()
                ->generatePrototype( STRINGIZE_STRING_LOCAL( "Scene" ), _prototype );

            scene->setName( _name );

            return scene;
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
                ->generatePrototype( STRINGIZE_STRING_LOCAL( "Arrow" ), _prototype );

            if( arrow == nullptr )
            {
                LOGGER_ERROR( "Error: can't setup arrow '%s'"
                    , _prototype.c_str()
                );

                return;
            }

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
            if( _hotspotImage == 0 || _hotspotImage->isCompile() == false )
            {
                return mt::vec2f( 0.f, 0.f );
            }

            const ResourceTestPickPtr & resourceHIT = _hotspotImage->getResourceTestPick();

            mt::vec2f size;
            size.x = (float)resourceHIT->getImageWidth();
            size.y = (float)resourceHIT->getImageHeight();

            return size;
        }
        //////////////////////////////////////////////////////////////////////////
        const mt::vec2f& s_getCamera2DPosition()
        {
            const RenderViewportInterfacePtr & rv = PLAYER_SERVICE()
                ->getRenderViewport();

            const Viewport & vp = rv->getViewport();

            return vp.begin;
        }
        //////////////////////////////////////////////////////////////////////////
        void destroyNode( const NodePtr & _node )
        {
            if( _node == nullptr )
            {
                LOGGER_ERROR( "Mengine.destroyNode invalid take None object"
                );

                return;
            }

            _node->removeFromParent();
            _node->release();
        }
        //////////////////////////////////////////////////////////////////////////
        NodePtr createNode( const ConstString & _type )
        {
            NodePtr node = PROTOTYPE_SERVICE()
                ->generatePrototype( STRINGIZE_STRING_LOCAL( "Node" ), _type );

            if( node == nullptr )
            {
                return nullptr;
            }

            return node;
        }
        //////////////////////////////////////////////////////////////////////////
        SurfacePtr createSurface( const ConstString & _type )
        {
            SurfacePtr surface = PROTOTYPE_SERVICE()
                ->generatePrototype( STRINGIZE_STRING_LOCAL( "Surface" ), _type );

            return surface;
        }
        //////////////////////////////////////////////////////////////////////////
        RandomizerInterfacePtr s_generateRandomizer( const ConstString & _prototype )
        {
            RandomizerInterfacePtr randomizer = PROTOTYPE_SERVICE()
                ->generatePrototype( STRINGIZE_STRING_LOCAL( "Randomizer" ), _prototype );

            return randomizer;
        }
        //////////////////////////////////////////////////////////////////////////
        ShapeQuadFixedPtr createSprite( const ConstString & _name, const ResourcePtr & _resource )
        {
            if( _resource == nullptr )
            {
                LOGGER_ERROR( "Mengine.createSprite: '%s' resource is NULL"
                    , _name.c_str()
                );

                return nullptr;
            }

            SurfaceImagePtr surface = PROTOTYPE_SERVICE()
                ->generatePrototype( STRINGIZE_STRING_LOCAL( "Surface" ), STRINGIZE_STRING_LOCAL( "SurfaceImage" ) );

            if( surface == nullptr )
            {
                LOGGER_ERROR( "createSprite: '%s' resource '%s' invalid create surface 'SurfaceImage'"
                    , _name.c_str()
                    , _resource->getName().c_str()
                );

                return nullptr;
            }

            surface->setName( _name );
            surface->setResourceImage( _resource );

            ShapeQuadFixedPtr shape = PROTOTYPE_SERVICE()
                ->generatePrototype( STRINGIZE_STRING_LOCAL( "Node" ), STRINGIZE_STRING_LOCAL( "ShapeQuadFixed" ) );

            if( shape == nullptr )
            {
                LOGGER_ERROR( "Mengine.createSprite: '%s' resource '%s' invalid create shape 'ShapeQuadFixed'"
                    , _name.c_str()
                    , _resource->getName().c_str()
                );

                return nullptr;
            }

            shape->setName( _name );
            shape->setSurface( surface );

            return shape;
        }
        //////////////////////////////////////////////////////////////////////////
        void quitApplication()
        {
            APPLICATION_SERVICE()
                ->quit();
        }
        //////////////////////////////////////////////////////////////////////////
        ResourcePtr createResource( const ConstString& _type )
        {
            ResourcePtr resource = RESOURCE_SERVICE()
                ->generateResource( _type );

            if( resource == nullptr )
            {
                LOGGER_ERROR( "createResource: invalid create resource '%s'"
                    , _type.c_str()
                );

                return nullptr;
            }

            return resource;
        }
        //////////////////////////////////////////////////////////////////////////
        bool directResourceCompile( const ConstString & _nameResource )
        {
            ResourcePtr resource;
            if( RESOURCE_SERVICE()
                ->hasResource( _nameResource, &resource ) == false )
            {
                LOGGER_ERROR( "directResourceCompile: not found resource '%s'"
                    , _nameResource.c_str()
                );

                return false;
            }

            if( resource->incrementReference() == false )
            {
                LOGGER_ERROR( "directResourceCompile: resource '%s' type '%s' invalid compile"
                    , _nameResource.c_str()
                    , resource->getType().c_str()
                );

                return false;
            }

            return true;
        }
        //////////////////////////////////////////////////////////////////////////
        bool directResourceRelease( const ConstString & _nameResource )
        {
            ResourcePtr resource;
            if( RESOURCE_SERVICE()
                ->hasResource( _nameResource, &resource ) == false )
            {
                LOGGER_ERROR( "directResourceRelease: not found resource '%s'"
                    , _nameResource.c_str()
                );

                return false;
            }

            if( resource->decrementReference() == false )
            {
                LOGGER_ERROR( "directResourceCompile: resource '%s' type '%s' invalid release"
                    , _nameResource.c_str()
                    , resource->getType().c_str()
                );

                return false;
            }

            return true;
        }
        //////////////////////////////////////////////////////////////////////////
        bool directFontCompile( const ConstString & _fontName )
        {
            bool successful = TEXT_SERVICE()
                ->directFontCompile( _fontName );

            return successful;
        }
        //////////////////////////////////////////////////////////////////////////
        bool directFontRelease( const ConstString & _fontName )
        {
            bool successful = TEXT_SERVICE()
                ->directFontRelease( _fontName );

            return successful;
        }
        //////////////////////////////////////////////////////////////////////////
        ResourcePtr s_getResourceReference( const ConstString & _name )
        {
            const ResourcePtr & resource = RESOURCE_SERVICE()
                ->getResourceReference( _name );

            if( resource == nullptr )
            {
                LOGGER_ERROR( "Mengine.getResourceReference: not exist resource %s"
                    , _name.c_str()
                );

                return nullptr;
            }

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
        void renderOneFrame()
        {
            RENDER_SERVICE()
                ->beginScene();

            GAME_SERVICE()
                ->render();

            RENDER_SERVICE()
                ->endScene();
        }
        //////////////////////////////////////////////////////////////////////////
        void s_writeImageToFile( const ConstString& _resource, const FilePath& _fileName )
        {
            const ResourceImagePtr & resource = RESOURCE_SERVICE()
                ->getResource( _resource );

            if( resource == nullptr )
            {
                LOGGER_ERROR( "Error: Image resource not getting '%s'"
                    , _resource.c_str()
                );

                return;
            }

            const FileGroupInterfacePtr & fileGroup = FILE_SERVICE()
                ->getFileGroup( STRINGIZE_STRING_LOCAL( "user" ) );

            const RenderTextureInterfacePtr & texture = resource->getTexture();

            RENDERTEXTURE_SERVICE()
                ->saveImage( texture, fileGroup, STRINGIZE_STRING_LOCAL( "pngImage" ), _fileName );
        }
        //////////////////////////////////////////////////////////////////////////
        void setParticlesEnabled( bool _enabled )
        {
            APPLICATION_SERVICE()
                ->setParticleEnable( _enabled );
        }
        //////////////////////////////////////////////////////////////////////////
        ResourceImageDefaultPtr s_createImageResource( const ConstString & _resourceName, const ConstString& _pakName, const FilePath& _fileName, const mt::vec2f & _maxSize )
        {
            mt::vec2f maxSize;

            if( _maxSize.x < 0.f || _maxSize.y < 0.f )
            {
                const FileGroupInterfacePtr & fileGroup = FILE_SERVICE()
                    ->getFileGroup( _pakName );

                InputStreamInterfacePtr stream = FILE_SERVICE()
                    ->openInputFile( fileGroup, _fileName, false );

                if( stream == nullptr )
                {
                    return nullptr;
                }

                ConstString codecType = CODEC_SERVICE()
                    ->findCodecType( _fileName );

                ImageDecoderInterfacePtr imageDecoder = CODEC_SERVICE()
                    ->createDecoderT<ImageDecoderInterfacePtr>( codecType );

                if( imageDecoder == nullptr )
                {
                    return nullptr;
                }

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
                ->generateResource( STRINGIZE_STRING_LOCAL( "ResourceImageDefault" ) );

            if( resource == nullptr )
            {
                return nullptr;
            }

            resource->setName( _resourceName );

            mt::uv4f uv_image;
            mt::uv4f uv_alpha;

            if( resource->setup( _fileName, ConstString::none(), uv_image, uv_alpha, maxSize ) == false )
            {
                return nullptr;
            }

            return resource;
        }
        //////////////////////////////////////////////////////////////////////////
        ResourceImageSolidPtr s_createImageSolidResource( const ConstString & _resourceName, const Color & _colour, const mt::vec2f & _maxSize )
        {
            ResourceImageSolidPtr resource = RESOURCE_SERVICE()
                ->generateResource( STRINGIZE_STRING_LOCAL( "ResourceImageSolid" ) );

            if( resource == nullptr )
            {
                return nullptr;
            }

            resource->setName( _resourceName );

            resource->setColor( _colour );

            resource->setMaxSize( _maxSize );
            resource->setSize( _maxSize );

            return resource;
        }
        //////////////////////////////////////////////////////////////////////////
        void minimizeWindow()
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

            INPUT_SERVICE()
                ->pushMouseMoveEvent( _touchId, cp.x, cp.y, mp.x, mp.y, 0.f );
        }
        //////////////////////////////////////////////////////////////////////////
        void s_pushMouseButtonEvent( uint32_t _touchId, const mt::vec2f & _pos, uint32_t _button, bool _isDown )
        {
            mt::vec2f pos_screen;
            this->s_calcMouseScreenPosition( _pos, &pos_screen );

            INPUT_SERVICE()
                ->pushMouseButtonEvent( _touchId, pos_screen.x, pos_screen.y, _button, 0.f, _isDown );
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
        bool s_mountResourcePak( const ConstString & _fileGroup
            , const ConstString & _name
            , const ConstString & _type
            , const ConstString & _category
            , const FilePath & _path
            , const FilePath & _descriptionPath
        )
        {
            FileGroupInterfacePtr fileGroup;
            if( FILE_SERVICE()
                ->hasFileGroup( _fileGroup, &fileGroup ) == false )
            {
                LOGGER_ERROR( "loadResourcePak invalid found file group %s"
                    , _fileGroup.c_str()
                );

                return false;
            }

            LOGGER_WARNING( "mountResourcePak name '%s' type '%s' category '%s' path '%s'"
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
            desc.category = _category;
            desc.path = _path;
            desc.descriptionPath = _descriptionPath;

            bool result = PACKAGE_SERVICE()
                ->addPackage( desc );

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
        bool s_unmountResourcePak( const ConstString & _name )
        {
            bool result = PACKAGE_SERVICE()
                ->removePackage( _name );

            return result;
        }
        //////////////////////////////////////////////////////////////////////////
        bool s_existFile( const ConstString & _fileGroupName, const FilePath & _path )
        {
            const FileGroupInterfacePtr & fileGroup = FILE_SERVICE()
                ->getFileGroup( _fileGroupName );

            bool result = fileGroup->existFile( _path );

            return result;
        }
        //////////////////////////////////////////////////////////////////////////
        bool s_removeFile( const FilePath & _path )
        {
            bool result = PLATFORM_SERVICE()
                ->removeFile( _path.c_str() );

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
                (void)_node;

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
                (void)_node;

                pybind::object py_cb_end = m_cb.get_attr( "end" );

                py_cb_end.call();
            }

        protected:
            pybind::kernel_interface * m_kernel;
            pybind::object m_cb;
        };
        //////////////////////////////////////////////////////////////////////////
        bool s_parseXml( pybind::kernel_interface * _kernel, const ConstString & _fileGroupName, const FilePath & _path, const pybind::object & _cb )
        {
            const FileGroupInterfacePtr & fileGroup = FILE_SERVICE()
                ->getFileGroup( _fileGroupName );

            MemoryInterfacePtr binary_buffer = Helper::createMemoryCacheFileString( fileGroup, _path, false, "Python::parseXml", __FILE__, __LINE__ );

            if( binary_buffer == nullptr )
            {
                return false;
            }

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
                ->existFont( _fontName, font );

            return has;
        }
        //////////////////////////////////////////////////////////////////////////
        bool s_validateFont( const ConstString & _fontName, const String & _text )
        {
            TextFontInterfacePtr font;
            if( TEXT_SERVICE()
                ->existFont( _fontName, font ) == false )
            {
                return false;
            }

            if( font->validateText( ConstString::none(), _text ) == false )
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
        void s_cacheResources( const ConstString & _category, const ConstString & _groupName )
        {
            const FileGroupInterfacePtr & fileGroup = FILE_SERVICE()
                ->getFileGroup( _category );

            if( fileGroup == nullptr )
            {
                return;
            }

            CacheResourceVisitorPtr rv_gac = new FactorableUnique<CacheResourceVisitor>();

            RESOURCE_SERVICE()
                ->visitGroupResources( fileGroup, _groupName, rv_gac );
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
        void s_uncacheResources( const ConstString & _category, const ConstString & _groupName )
        {
            const FileGroupInterfacePtr & fileGroup = FILE_SERVICE()
                ->getFileGroup( _category );

            if( fileGroup == nullptr )
            {
                return;
            }

            UncacheResourceVisitorPtr rv_gac = new FactorableUnique<UncacheResourceVisitor>();

            RESOURCE_SERVICE()
                ->visitGroupResources( fileGroup, _groupName, rv_gac );
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
            (void)_base;
            //const Polygon & base_polygon = _base->getPolygon();

            Polygon correct_polygon;

            for( const HotSpotPolygon * overlap_hotspot : _overlap )
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
        bool s_copyFile_( const ConstString & _resourceFileName, const MemoryBufferInterfacePtr & _blob )
        {
            const ResourceFilePtr & resourceFile = RESOURCE_SERVICE()
                ->getResource( _resourceFileName );

            if( resourceFile == nullptr )
            {
                return false;
            }

            const FileGroupInterfacePtr & fileGroup = resourceFile->getFileGroup();

            const FilePath & filePath = resourceFile->getFilePath();

            InputStreamInterfacePtr stream = FILE_SERVICE()
                ->openInputFile( fileGroup, filePath, false );

            if( stream == nullptr )
            {
                return false;
            }

            size_t size = stream->size();

            void * memory_buffer = _blob->newBuffer( size, "Python::copyFile", __FILE__, __LINE__ );

            if( stream->read( memory_buffer, size ) != size )
            {
                return false;
            }

            resourceFile->decrementReference();

            return true;
        }
        //////////////////////////////////////////////////////////////////////////
        bool s_copyUserPicture( const ConstString & _resourceFileName, const String & _fileName )
        {
            MemoryBufferInterfacePtr memory = MEMORY_SERVICE()
                ->createMemoryBuffer();

            if( this->s_copyFile_( _resourceFileName, memory ) == false )
            {
                return false;
            }

            Char projectName[MENGINE_APPLICATION_PROJECT_MAXNAME];
            APPLICATION_SERVICE()
                ->getProjectName( projectName );

            if( PLATFORM_SERVICE()
                ->createDirectoryUserPicture( projectName, _fileName.c_str(), memory->getBuffer(), memory->getSize() ) == false )
            {
                return false;
            }

            return true;
        }
        //////////////////////////////////////////////////////////////////////////
        bool s_copyUserMusic( const ConstString & _resourceFileName, const String & _fileName )
        {
            MemoryBufferInterfacePtr memory = MEMORY_SERVICE()
                ->createMemoryBuffer();

            if( this->s_copyFile_( _resourceFileName, memory ) == false )
            {
                return false;
            }

            Char projectName[MENGINE_APPLICATION_PROJECT_MAXNAME];
            APPLICATION_SERVICE()
                ->getProjectName( projectName );

            if( PLATFORM_SERVICE()
                ->createDirectoryUserMusic( projectName, _fileName.c_str(), memory->getBuffer(), memory->getSize() ) == false )
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
            void setup( const ArrowPtr & _arrow, const RenderCameraInterfacePtr & _camera, const RenderViewportInterfacePtr & _viewport, const pybind::object & _cb, const pybind::args & _args )
            {
                m_arrow = _arrow;
                m_renderCamera = _camera;
                m_renderViewport = _viewport;
                m_cb = _cb;
                m_args = _args;
            }

        protected:
            void onMousePositionChange( uint32_t _touchId, const mt::vec2f & _position ) override
            {
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

            PyInputMousePositionProviderPtr provider = m_factoryPyInputMousePositionProvider->createObject();

            provider->setup( arrow, camera, viewport, _cb, _args );

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
            AffectorGridBurnTransparencyPtr affector = m_factoryAffectorGridBurnTransparency->createObject();

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
            AffectorUserPtr affector = m_factoryAffectorUser->createObject();

            if( affector->initialize( _cb, _args ) == false )
            {
                return nullptr;
            }

            return affector;
        }
        //////////////////////////////////////////////////////////////////////////
        AFFECTOR_ID s_addAffector( const pybind::object & _cb, const pybind::args & _args )
        {
            AffectorUserPtr affector = m_factoryAffectorUser->createObject();

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
            , public pybind::bindable
        {
        public:
            virtual void follow( const pybind::object & _target ) = 0;

        protected:
            PyObject * _embedded( pybind::kernel_interface * _kernel ) override
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
        {
            typedef Lambda<void( const T_Value & )> T_Setter;
            typedef Lambda<T_Value()> T_Getter;

            typedef AffectorNodeFollowerMethod<T_Value, T_Setter, T_Getter> TAffectorNodeFollowerMethod;
            typedef IntrusivePtr<TAffectorNodeFollowerMethod> TAffectorNodeFollowerMethodPtr;

        public:
            AffectorNodeFollowerCreator()
            {
                m_factory = new FactoryPool<TAffectorNodeFollowerMethod, 4>();
            }

        public:
            AffectorFollowerPtr create( const AffectorablePtr & _affectorable, const T_Setter & _setter, const T_Getter & _getter, const T_Value & _value, const T_Value & _target, float _speed )
            {
                TAffectorNodeFollowerMethodPtr affector = m_factory->createObject();

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
        AffectorNodeFollowerCreator<Node, float> m_creatorAffectorNodeFollowerLocalAlpha;
        //////////////////////////////////////////////////////////////////////////
        AffectorFollowerPtr s_addNodeFollowerLocalAlpha( const NodePtr & _node, float _value, float _target, float _speed )
        {
            RenderInterface * render = _node->getRender();

            if( render == nullptr )
            {
                LOGGER_ERROR( "Node.addNodeFollowerLocalAlpha node '%s' is not renderable"
                    , _node->getName().c_str()
                );

                return nullptr;
            }

            AffectorFollowerPtr affector = m_creatorAffectorNodeFollowerLocalAlpha.create( _node
                , [render]( float _alpha ) { render->setLocalAlpha( _alpha ); }
            , [render]() { return render->getLocalAlpha(); }
            , _value, _target, _speed );

            return affector;
        }
        //////////////////////////////////////////////////////////////////////////
        AffectorNodeFollowerCreator<ShapeQuadFlex, mt::vec2f> m_creatorAffectorNodeFollowerCustomSize;
        //////////////////////////////////////////////////////////////////////////
        AffectorFollowerPtr s_addShapeFollowerCustomSize( const ShapeQuadFlexPtr & _node, const mt::vec2f & _value, const mt::vec2f & _target, float _speed )
        {
            AffectorFollowerPtr affector = m_creatorAffectorNodeFollowerCustomSize.create( _node
                , [_node]( const mt::vec2f & _value ) { _node->setCustomSize( _value ); }
            , [_node]() { return _node->getCustomSize(); }
            , _value, _target, _speed );

            return affector;
        }
        //////////////////////////////////////////////////////////////////////////
        AffectorNodeFollowerCreator<ShapeQuadFlex, mt::vec2f> m_creatorAffectorNodeFollowerTextureUVScale;
        //////////////////////////////////////////////////////////////////////////
        AffectorFollowerPtr s_addShapeFollowerTextureUVScale( const ShapeQuadFlexPtr & _node, const mt::vec2f & _value, const mt::vec2f & _target, float _speed )
        {
            AffectorFollowerPtr affector = m_creatorAffectorNodeFollowerTextureUVScale.create( _node
                , [_node]( const mt::vec2f & _value ) { _node->setTextureUVScale( _value ); }
            , [_node]() { return _node->getTextureUVScale(); }
            , _value, _target, _speed );

            return affector;
        }
        //////////////////////////////////////////////////////////////////////////
        void s_removeNodeFollower( const AffectorFollowerPtr & _affector )
        {
            if( _affector == nullptr )
            {
                LOGGER_ERROR( "removeNodeFollower take NULL affector"
                );

                return;
            }

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
                ->generatePrototype( STRINGIZE_STRING_LOCAL( "Affector" ), STRINGIZE_STRING_LOCAL( "PythonValueFollowerLinear" ) );

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
                ->generatePrototype( STRINGIZE_STRING_LOCAL( "Affector" ), STRINGIZE_STRING_LOCAL( "PythonValueFollowerAcceleration" ) );

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
            (void)_enable;
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
                ->existText( _key, nullptr );

            return exist;
        }
        //////////////////////////////////////////////////////////////////////////
        pybind::list s_pickHotspot( pybind::kernel_interface * _kernel, const mt::vec2f & _point )
        {
            VectorPickerTraps traps;
            PICKER_SERVICE()
                ->pickTrap( _point, traps );

            pybind::list pyret( _kernel );

            bool onFocus = APPLICATION_SERVICE()
                ->isFocus();

            if( onFocus == false )
            {
                return pyret;
            }

            for( const PickerTrapInterfacePtr & trap : traps )
            {
                ScriptablePtr scriptable = trap->propagatePickerScriptable();

                pyret.append( scriptable );
            }

            return pyret;
        }
        //////////////////////////////////////////////////////////////////////////
        bool s_testPlatformTags( const Tags & _tags )
        {
            const Tags & platformTags = PLATFORM_SERVICE()
                ->getPlatformTags();

            bool successful = platformTags.inTags( _tags );

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
        void showKeyboard()
        {
            APPLICATION_SERVICE()
                ->showKeyboard();
        }
        //////////////////////////////////////////////////////////////////////////
        void hideKeyboard()
        {
            APPLICATION_SERVICE()
                ->hideKeyboard();
        }
        //////////////////////////////////////////////////////////////////////////
        bool hasResource( const ConstString & _name )
        {
            return RESOURCE_SERVICE()
                ->hasResource( _name, nullptr );
        }
        //////////////////////////////////////////////////////////////////////////
        bool removeCurrentScene( const pybind::object & _cb, const pybind::args & _args )
        {
            if( _cb.is_callable() == false )
            {
                LOGGER_ERROR( "removeCurrentScene cb '%s' not callable"
                    , _cb.repr()
                );

                return false;
            }

            PythonSceneChangeCallbackPtr py_cb = m_factoryPythonSceneChangeCallback->createObject();

            py_cb->initialize( _cb, _args );

            if( SCENE_SERVICE()
                ->removeCurrentScene( py_cb ) == false )
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
                (void)_event;

                return false;
            }

            bool handleTextEvent( const InputTextEvent & _event ) override
            {
                (void)_event;

                return false;
            }

        protected:
            bool handleMouseButtonEvent( const InputMouseButtonEvent & _event ) override
            {
                (void)_event;

                return false;
            }

            bool handleMouseButtonEventBegin( const InputMouseButtonEvent & _event ) override
            {
                (void)_event;

                return false;
            }

            bool handleMouseButtonEventEnd( const InputMouseButtonEvent & _event ) override
            {
                (void)_event;

                return false;
            }

            bool handleMouseMove( const InputMouseMoveEvent & _event ) override
            {
                (void)_event;

                return false;
            }

            bool handleMouseWheel( const InputMouseWheelEvent & _event ) override
            {
                (void)_event;

                return false;
            }

        protected:
            pybind::object m_cb;
            pybind::args m_args;
        };
        //////////////////////////////////////////////////////////////////////////
        typedef IntrusivePtr<PyGlobalBaseHandler> PyGlobalBaseHandlerPtr;
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
                    LOGGER_ERROR( "PyGlobalMouseMoveHandler %s return value %s not None"
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

            PyGlobalMouseMoveHandlerPtr handler = m_factoryPyGlobalMouseMoveHandlers->createObject();

            handler->initialize( _cb, _args );

            uint32_t id = globalHandleSystem->addGlobalHandler( handler, _cb.repr() );

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

                pybind::object py_result = m_cb.call_args( _event.touchId, wp.x, wp.y, _event.button, _event.pressure, _event.isDown, _event.isPressed, m_args );

                if( py_result.is_none() == false )
                {
                    LOGGER_ERROR( "PyGlobalMouseHandlerButton %s return value %s not None"
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

            PyGlobalMouseHandlerButtonPtr handler = m_factoryPyGlobalMouseHandlerButtons->createObject();

            handler->initialize( _cb, _args );

            uint32_t id = globalHandleSystem->addGlobalHandler( handler, _cb.repr() );

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

                pybind::object py_result = m_cb.call_args( _event.touchId, wp.x, wp.y, _event.button, _event.pressure, _event.isDown, _event.isPressed, m_args );

                if( py_result.is_none() == false )
                {
                    LOGGER_ERROR( "PyGlobalMouseHandlerButtonEnd %s return value %s not None"
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

            PyGlobalMouseHandlerButtonEndPtr handler = m_factoryPyGlobalMouseHandlerButtonEnds->createObject();

            handler->initialize( _cb, _args );

            uint32_t id = globalHandleSystem->addGlobalHandler( handler, _cb.repr() );

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
                pybind::object py_result = m_cb.call_args( _event.button, _event.x, _event.y, _event.wheel, m_args );

                if( py_result.is_none() == false )
                {
                    LOGGER_ERROR( "PyGlobalMouseHandlerWheel %s return value %s not None"
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

            const PyGlobalMouseHandlerWheelPtr & handler = m_factoryPyGlobalMouseHandlerWheels->createObject();

            handler->initialize( _cb, _args );

            uint32_t id = globalHandleSystem->addGlobalHandler( handler, _cb.repr() );

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

                pybind::object py_result = m_cb.call_args( _event.touchId, wp.x, wp.y, _event.button, _event.pressure, _event.isDown, _event.isPressed, m_args );

                if( py_result.is_none() == false )
                {
                    LOGGER_ERROR( "PyGlobalMouseHandlerButtonBegin %s return value %s not None"
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

            PyGlobalMouseHandlerButtonBeginPtr handler = m_factoryPyGlobalMouseHandlerButtonBegins->createObject();

            handler->initialize( _cb, _args );

            uint32_t id = globalHandleSystem->addGlobalHandler( handler, _cb.repr() );

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
                    LOGGER_ERROR( "PyGlobalKeyHandler %s return value %s not None"
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

            PyGlobalKeyHandlerPtr handler = m_factoryPyGlobalKeyHandler->createObject();

            handler->initialize( _cb, _args );

            uint32_t id = globalHandleSystem->addGlobalHandler( handler, _cb.repr() );

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
                    LOGGER_ERROR( "PyGlobalTextHandler %s return value %s not None"
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

            PyGlobalBaseHandlerPtr handler = m_factoryPyGlobalTextHandler->createObject();

            handler->initialize( _cb, _args );

            uint32_t id = globalHandleSystem->addGlobalHandler( handler, _cb.repr() );

            return id;
        }
        //////////////////////////////////////////////////////////////////////////
        bool s_removeGlobalHandler( uint32_t _id )
        {
            const GlobalInputHandlerInterfacePtr & globalHandleSystem = PLAYER_SERVICE()
                ->getGlobalInputHandler();

            InputHandlerInterfacePtr handler = globalHandleSystem->removeGlobalHandler( _id );

            PyGlobalBaseHandlerPtr py_handler = stdex::intrusive_dynamic_cast<PyGlobalBaseHandlerPtr>(handler);

            if( py_handler == nullptr )
            {
                LOGGER_ERROR( "s_removeKeyHandler %d handler invalid"
                    , _id
                );

                return false;
            }

            py_handler->finalize();

            return true;
        }
        //////////////////////////////////////////////////////////////////////////
        bool s_enableGlobalHandler( uint32_t _id, bool _value )
        {
            const GlobalInputHandlerInterfacePtr & globalHandleSystem = PLAYER_SERVICE()
                ->getGlobalInputHandler();

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
            if( GAME_SERVICE()
                ->hasParam( _paramName ) == false )
            {
                return false;
            }

            return true;
        }
        //////////////////////////////////////////////////////////////////////////
        PyObject * s_getGameParam( pybind::kernel_interface * _kernel, const ConstString & _paramName )
        {
            Char param_value[MENGINE_GAME_PARAM_MAXVALUE];
            if( GAME_SERVICE()
                ->getParam( _paramName, param_value ) == false )
            {
                return _kernel->ret_none();
            }

            PyObject * py_param = _kernel->unicode_from_utf8( param_value );

            return py_param;
        }
        //////////////////////////////////////////////////////////////////////////
        PyObject * s_getGameParamFloat( pybind::kernel_interface * _kernel, const ConstString & _paramName )
        {
            Char param_value[MENGINE_GAME_PARAM_MAXVALUE];
            if( GAME_SERVICE()
                ->getParam( _paramName, param_value ) == false )
            {
                return _kernel->ret_none();
            }

            float value;
            Helper::charsToFloat( param_value, value );

            return pybind::ptr( _kernel, value );
        }
        //////////////////////////////////////////////////////////////////////////
        PyObject * s_getGameParamInt( pybind::kernel_interface * _kernel, const ConstString & _paramName )
        {
            Char param_value[MENGINE_GAME_PARAM_MAXVALUE];
            if( GAME_SERVICE()
                ->getParam( _paramName, param_value ) == false )
            {
                return _kernel->ret_none();
            }

            int32_t value;
            Helper::charsToInt( param_value, value );

            return pybind::ptr( _kernel, value );
        }
        //////////////////////////////////////////////////////////////////////////
        PyObject * s_getGameParamBool( pybind::kernel_interface * _kernel, const ConstString & _paramName )
        {
            Char param_value[MENGINE_GAME_PARAM_MAXVALUE];
            if( GAME_SERVICE()
                ->getParam( _paramName, param_value ) == false )
            {
                return _kernel->ret_none();
            }

            int32_t value;
            Helper::charsToInt( param_value, value );

            bool b_value = value != 0;

            return _kernel->ret_bool( b_value );
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
            ResourceVisitorGetAlreadyCompiled( const pybind::object & _cb )
                : m_cb( _cb )
            {
            }

            ~ResourceVisitorGetAlreadyCompiled() override
            {
            }

        protected:
            void accept( Resource* _resource ) override
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
        void s_visitCompiledResources( const ConstString & _category, const ConstString & _groupName, const pybind::object & _cb )
        {
            const FileGroupInterfacePtr & fileGroup = FILE_SERVICE()
                ->getFileGroup( _category );

            if( fileGroup == nullptr )
            {
                return;
            }

            ResourceVisitorGetAlreadyCompiledPtr rv_gac = new FactorableUnique<ResourceVisitorGetAlreadyCompiled>( _cb );

            RESOURCE_SERVICE()
                ->visitGroupResources( fileGroup, _groupName, rv_gac );
        }
        //////////////////////////////////////////////////////////////////////////
        class MyResourceVisitor
            : public Visitor
            , public Factorable
            , public ConcreteVisitor<Resource>
        {
        public:
            MyResourceVisitor( const pybind::object & _cb )
                : m_cb( _cb )
            {
            }

            ~MyResourceVisitor() override
            {
            }

        protected:
            void accept( Resource* _resource ) override
            {
                m_cb.call( _resource );
            }

        protected:
            pybind::object m_cb;
        };
        //////////////////////////////////////////////////////////////////////////
        typedef IntrusivePtr<MyResourceVisitor> MyResourceVisitorPtr;
        //////////////////////////////////////////////////////////////////////////
        void s_visitResources( const ConstString & _category, const ConstString & _groupName, const pybind::object & _cb )
        {
            const FileGroupInterfacePtr & fileGroup = FILE_SERVICE()
                ->getFileGroup( _category );

            if( fileGroup == nullptr )
            {
                return;
            }

            MyResourceVisitorPtr rv_gac = new FactorableUnique<MyResourceVisitor>( _cb );

            RESOURCE_SERVICE()
                ->visitGroupResources( fileGroup, _groupName, rv_gac );
        }
        //////////////////////////////////////////////////////////////////////////
        class IncrefResourceVisitor
            : public Visitor
            , public Factorable
            , public ConcreteVisitor<Resource>
        {
        protected:
            void accept( Resource* _resource ) override
            {
                _resource->incrementReference();
            }
        };
        //////////////////////////////////////////////////////////////////////////
        typedef IntrusivePtr<IncrefResourceVisitor> IncrefResourceVisitorPtr;
        //////////////////////////////////////////////////////////////////////////		
        void s_incrementResources( const ConstString & _category, const ConstString & _groupName )
        {
            const FileGroupInterfacePtr & fileGroup = FILE_SERVICE()
                ->getFileGroup( _category );

            if( fileGroup == nullptr )
            {
                return;
            }

            IncrefResourceVisitorPtr rv_gac = new FactorableUnique<IncrefResourceVisitor>();

            RESOURCE_SERVICE()
                ->visitGroupResources( fileGroup, _groupName, rv_gac );
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
                _resource->decrementReference();
            }
        };
        //////////////////////////////////////////////////////////////////////////
        typedef IntrusivePtr<DecrementResourceVisitor> DecrementResourceVisitorPtr;
        //////////////////////////////////////////////////////////////////////////		
        void s_decrementResources( const ConstString & _category, const ConstString & _groupName )
        {
            const FileGroupInterfacePtr & fileGroup = FILE_SERVICE()
                ->getFileGroup( _category );

            if( fileGroup == nullptr )
            {
                return;
            }

            DecrementResourceVisitorPtr rv_gac = new FactorableUnique<DecrementResourceVisitor>();

            RESOURCE_SERVICE()
                ->visitGroupResources( fileGroup, _groupName, rv_gac );
        }
        //////////////////////////////////////////////////////////////////////////
        class GetResourceVisitor
            : public Visitor
            , public Factorable
            , public ConcreteVisitor<Resource>
        {
        public:
            GetResourceVisitor( pybind::kernel_interface * _kernel )
                : m_kernel( _kernel )
                , m_l( _kernel )
            {
                m_scope = m_kernel->class_scope<Resource>();
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
            void accept( Resource* _resource ) override
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
        pybind::list s_getResources( pybind::kernel_interface * _kernel, const ConstString & _category, const ConstString & _groupName )
        {
            const FileGroupInterfacePtr & fileGroup = FILE_SERVICE()
                ->getFileGroup( _category );

            if( fileGroup == nullptr )
            {
                return pybind::list( _kernel );
            }

            GetResourceVisitorPtr rv_gac = new FactorableUnique<GetResourceVisitor>( _kernel );

            RESOURCE_SERVICE()
                ->visitGroupResources( fileGroup, _groupName, rv_gac );

            const pybind::list & l = rv_gac->getResult();

            return l;
        }
        //////////////////////////////////////////////////////////////////////////
        bool s_validResource( const ConstString & _resourceName )
        {
            const ResourcePtr & resource = RESOURCE_SERVICE()
                ->getResourceReference( _resourceName );

            if( resource == nullptr )
            {
                LOGGER_ERROR( "s_validResource resource '%s' not found"
                    , _resourceName.c_str()
                );

                return false;
            }

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
    EngineScriptWrapper::EngineScriptWrapper()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    EngineScriptWrapper::~EngineScriptWrapper()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool EngineScriptWrapper::embedding()
    {
        EngineScriptMethod * nodeScriptMethod = new FactorableUnique<EngineScriptMethod>();

        pybind::kernel_interface * kernel = SCRIPT_SERVICE()
            ->getKernel();

        pybind::def_functor_args( kernel, "setCurrentScene", nodeScriptMethod, &EngineScriptMethod::setCurrentScene );
        pybind::def_functor( kernel, "getCurrentScene", nodeScriptMethod, &EngineScriptMethod::getCurrentScene );

        pybind::def_functor( kernel, "getGlobalScene", nodeScriptMethod, &EngineScriptMethod::getGlobalScene );

        pybind::def_functor( kernel, "createScene", nodeScriptMethod, &EngineScriptMethod::s_createScene );

        pybind::def_functor( kernel, "getCamera2DPosition", nodeScriptMethod, &EngineScriptMethod::s_getCamera2DPosition );

        pybind::def_functor( kernel, "createNode", nodeScriptMethod, &EngineScriptMethod::createNode );
        pybind::def_functor( kernel, "destroyNode", nodeScriptMethod, &EngineScriptMethod::destroyNode );

        pybind::def_functor( kernel, "createSurface", nodeScriptMethod, &EngineScriptMethod::createSurface );
        pybind::def_functor( kernel, "createSprite", nodeScriptMethod, &EngineScriptMethod::createSprite );

        pybind::def_functor( kernel, "generateRandomizer", nodeScriptMethod, &EngineScriptMethod::s_generateRandomizer );

        pybind::def_functor_args( kernel, "timing", nodeScriptMethod, &EngineScriptMethod::timing );
        pybind::def_functor( kernel, "timingRemove", nodeScriptMethod, &EngineScriptMethod::timingRemove );

        pybind::def_functor( kernel, "createScheduler", nodeScriptMethod, &EngineScriptMethod::createScheduler );
        pybind::def_functor( kernel, "destroyScheduler", nodeScriptMethod, &EngineScriptMethod::destroyScheduler );

        pybind::def_functor_args( kernel, "schedule", nodeScriptMethod, &EngineScriptMethod::schedule );
        pybind::def_functor( kernel, "scheduleRemove", nodeScriptMethod, &EngineScriptMethod::scheduleRemove );
        pybind::def_functor( kernel, "scheduleRemoveAll", nodeScriptMethod, &EngineScriptMethod::scheduleRemoveAll );
        pybind::def_functor( kernel, "scheduleFreeze", nodeScriptMethod, &EngineScriptMethod::s_scheduleFreeze );
        pybind::def_functor( kernel, "scheduleFreezeAll", nodeScriptMethod, &EngineScriptMethod::s_scheduleFreezeAll );
        pybind::def_functor( kernel, "scheduleResumeAll", nodeScriptMethod, &EngineScriptMethod::scheduleResumeAll );
        pybind::def_functor( kernel, "scheduleIsFreeze", nodeScriptMethod, &EngineScriptMethod::s_scheduleIsFreeze );
        pybind::def_functor( kernel, "scheduleTime", nodeScriptMethod, &EngineScriptMethod::s_scheduleTime );

        pybind::def_functor_args( kernel, "scheduleGlobal", nodeScriptMethod, &EngineScriptMethod::s_scheduleGlobal );
        pybind::def_functor( kernel, "scheduleGlobalRemove", nodeScriptMethod, &EngineScriptMethod::s_scheduleGlobalRemove );
        pybind::def_functor( kernel, "scheduleGlobalRemoveAll", nodeScriptMethod, &EngineScriptMethod::s_scheduleGlobalRemoveAll );
        pybind::def_functor( kernel, "scheduleGlobalFreeze", nodeScriptMethod, &EngineScriptMethod::s_scheduleGlobalFreeze );
        pybind::def_functor( kernel, "scheduleGlobalFreezeAll", nodeScriptMethod, &EngineScriptMethod::s_scheduleGlobalFreezeAll );
        pybind::def_functor( kernel, "scheduleGlobalResumeAll", nodeScriptMethod, &EngineScriptMethod::s_scheduleGlobalResumeAll );
        pybind::def_functor( kernel, "scheduleGlobalIsFreeze", nodeScriptMethod, &EngineScriptMethod::s_scheduleGlobalIsFreeze );
        pybind::def_functor_deprecated( kernel, "scheduleGlobalTime", nodeScriptMethod, &EngineScriptMethod::s_scheduleGlobalPassed, "use scheduleGlobalPassed" );
        pybind::def_functor( kernel, "scheduleGlobalPassed", nodeScriptMethod, &EngineScriptMethod::s_scheduleGlobalPassed );
        pybind::def_functor( kernel, "scheduleGlobalLeft", nodeScriptMethod, &EngineScriptMethod::s_scheduleGlobalLeft );

        pybind::def_functor( kernel, "getCursorPosition", nodeScriptMethod, &EngineScriptMethod::s_getCursorPosition );
        pybind::def_functor( kernel, "getTouchPosition", nodeScriptMethod, &EngineScriptMethod::s_getTouchPosition );
        pybind::def_functor( kernel, "getMousePosition", nodeScriptMethod, &EngineScriptMethod::s_getMousePosition );

        pybind::def_functor( kernel, "setArrow", nodeScriptMethod, &EngineScriptMethod::s_setArrow );
        pybind::def_functor( kernel, "getArrow", nodeScriptMethod, &EngineScriptMethod::s_getArrow );
        pybind::def_functor( kernel, "hideArrow", nodeScriptMethod, &EngineScriptMethod::s_hideArrow );

        pybind::def_functor( kernel, "setArrowLayer", nodeScriptMethod, &EngineScriptMethod::s_setArrowLayer );

        pybind::def_functor( kernel, "createResource", nodeScriptMethod, &EngineScriptMethod::createResource );
        pybind::def_functor( kernel, "directResourceCompile", nodeScriptMethod, &EngineScriptMethod::directResourceCompile );
        pybind::def_functor( kernel, "directResourceRelease", nodeScriptMethod, &EngineScriptMethod::directResourceRelease );
        pybind::def_functor( kernel, "getResourceReference", nodeScriptMethod, &EngineScriptMethod::s_getResourceReference );

        pybind::def_functor( kernel, "directFontCompile", nodeScriptMethod, &EngineScriptMethod::directFontCompile );
        pybind::def_functor( kernel, "directFontRelease", nodeScriptMethod, &EngineScriptMethod::directFontRelease );

        pybind::def_functor( kernel, "quitApplication", nodeScriptMethod, &EngineScriptMethod::quitApplication );
        pybind::def_functor( kernel, "setFullscreenMode", nodeScriptMethod, &EngineScriptMethod::s_setFullscreenMode );
        pybind::def_functor( kernel, "getFullscreenMode", nodeScriptMethod, &EngineScriptMethod::s_getFullscreenMode );
        pybind::def_functor( kernel, "setFixedContentResolution", nodeScriptMethod, &EngineScriptMethod::s_setFixedContentResolution );
        pybind::def_functor( kernel, "getFixedContentResolution", nodeScriptMethod, &EngineScriptMethod::s_getFixedContentResolution );
        pybind::def_functor( kernel, "setFixedDisplayResolution", nodeScriptMethod, &EngineScriptMethod::s_setFixedDisplayResolution );
        pybind::def_functor( kernel, "getFixedDisplayResolution", nodeScriptMethod, &EngineScriptMethod::s_getFixedDisplayResolution );


        pybind::def_functor( kernel, "renderOneFrame", nodeScriptMethod, &EngineScriptMethod::renderOneFrame );
        pybind::def_functor( kernel, "writeImageToFile", nodeScriptMethod, &EngineScriptMethod::s_writeImageToFile );
        pybind::def_functor( kernel, "createImageResource", nodeScriptMethod, &EngineScriptMethod::s_createImageResource );
        pybind::def_functor( kernel, "createImageSolidResource", nodeScriptMethod, &EngineScriptMethod::s_createImageSolidResource );
        pybind::def_functor( kernel, "minimizeWindow", nodeScriptMethod, &EngineScriptMethod::minimizeWindow );

        pybind::def_functor( kernel, "getCurrentResolution", nodeScriptMethod, &EngineScriptMethod::s_getCurrentResolution );
        pybind::def_functor( kernel, "getContentResolution", nodeScriptMethod, &EngineScriptMethod::s_getContentResolution );
        pybind::def_functor( kernel, "getHotSpotImageSize", nodeScriptMethod, &EngineScriptMethod::s_getHotSpotImageSize );

        pybind::def_functor( kernel, "setNopause", nodeScriptMethod, &EngineScriptMethod::s_setNopause );
        pybind::def_functor( kernel, "getNopause", nodeScriptMethod, &EngineScriptMethod::s_getNopause );

        pybind::def_functor( kernel, "isInViewport", nodeScriptMethod, &EngineScriptMethod::s_isInViewport );
        pybind::def_functor( kernel, "enableTextureFiltering", nodeScriptMethod, &EngineScriptMethod::s_enableTextureFiltering );
        pybind::def_functor( kernel, "isTextureFilteringEnabled", nodeScriptMethod, &EngineScriptMethod::s_isTextureFilteringEnabled );

        pybind::def_functor( kernel, "existText", nodeScriptMethod, &EngineScriptMethod::s_existText );

        pybind::def_functor_kernel( kernel, "pickHotspot", nodeScriptMethod, &EngineScriptMethod::s_pickHotspot );

        pybind::def_functor( kernel, "blockInput", nodeScriptMethod, &EngineScriptMethod::s_setMousePickerBlockInput );
        pybind::def_functor( kernel, "setMousePickerHandleValue", nodeScriptMethod, &EngineScriptMethod::s_setMousePickerHandleValue );

        pybind::def_functor( kernel, "setInputMouseButtonEventBlock", nodeScriptMethod, &EngineScriptMethod::s_setInputMouseButtonEventBlock );
        pybind::def_functor( kernel, "getInputMouseButtonEventBlock", nodeScriptMethod, &EngineScriptMethod::s_getInputMouseButtonEventBlock );

        pybind::def_functor( kernel, "loadPlugin", nodeScriptMethod, &EngineScriptMethod::s_loadPlugin );

        pybind::def_functor( kernel, "getRenderCamera2D", nodeScriptMethod, &EngineScriptMethod::s_getRenderCamera2D );

        pybind::def_functor( kernel, "testHotspot", nodeScriptMethod, &EngineScriptMethod::s_testHotspot );
        pybind::def_functor( kernel, "polygon_wm", nodeScriptMethod, &EngineScriptMethod::s_polygon_wm );
        pybind::def_functor( kernel, "polygon_anchor", nodeScriptMethod, &EngineScriptMethod::s_polygon_anchor );

        pybind::def_functor( kernel, "showKeyboard", nodeScriptMethod, &EngineScriptMethod::showKeyboard );
        pybind::def_functor( kernel, "hideKeyboard", nodeScriptMethod, &EngineScriptMethod::hideKeyboard );

        pybind::def_functor( kernel, "hasResource", nodeScriptMethod, &EngineScriptMethod::hasResource );

        pybind::def_functor_args( kernel, "removeCurrentScene", nodeScriptMethod, &EngineScriptMethod::removeCurrentScene );

        pybind::def_functor_args( kernel, "addMouseMoveHandler", nodeScriptMethod, &EngineScriptMethod::s_addMouseMoveHandler );
        pybind::def_functor_args( kernel, "addMouseButtonHandler", nodeScriptMethod, &EngineScriptMethod::s_addMouseButtonHandler );
        pybind::def_functor_args( kernel, "addMouseButtonHandlerBegin", nodeScriptMethod, &EngineScriptMethod::s_addMouseButtonHandlerBegin );
        pybind::def_functor_args( kernel, "addMouseButtonHandlerEnd", nodeScriptMethod, &EngineScriptMethod::s_addMouseButtonHandlerEnd );
        pybind::def_functor_args( kernel, "addMouseWheelHandler", nodeScriptMethod, &EngineScriptMethod::s_addMouseWheelHandler );
        pybind::def_functor_args( kernel, "addKeyHandler", nodeScriptMethod, &EngineScriptMethod::s_addKeyHandler );
        pybind::def_functor_args( kernel, "addTextHandler", nodeScriptMethod, &EngineScriptMethod::s_addTextHandler );

        pybind::def_functor( kernel, "removeGlobalHandler", nodeScriptMethod, &EngineScriptMethod::s_removeGlobalHandler );
        pybind::def_functor( kernel, "enableGlobalHandler", nodeScriptMethod, &EngineScriptMethod::s_enableGlobalHandler );

        pybind::def_functor( kernel, "visitChild", nodeScriptMethod, &EngineScriptMethod::s_visitChild );

        pybind::def_functor( kernel, "visitCompiledResources", nodeScriptMethod, &EngineScriptMethod::s_visitCompiledResources );
        pybind::def_functor( kernel, "visitResources", nodeScriptMethod, &EngineScriptMethod::s_visitResources );

        pybind::def_functor( kernel, "incrementResources", nodeScriptMethod, &EngineScriptMethod::s_incrementResources );
        pybind::def_functor( kernel, "decrementResources", nodeScriptMethod, &EngineScriptMethod::s_decrementResources );

        pybind::def_functor( kernel, "validResource", nodeScriptMethod, &EngineScriptMethod::s_validResource );

        pybind::def_functor_kernel( kernel, "getResources", nodeScriptMethod, &EngineScriptMethod::s_getResources );

        pybind::def_functor( kernel, "testPlatformTags", nodeScriptMethod, &EngineScriptMethod::s_testPlatformTags );
        pybind::def_functor( kernel, "hasPlatformTag", nodeScriptMethod, &EngineScriptMethod::s_hasPlatformTag );
        pybind::def_functor( kernel, "hasTouchpad", nodeScriptMethod, &EngineScriptMethod::s_hasTouchpad );
        pybind::def_functor_deprecated( kernel, "getTimingFactor", nodeScriptMethod, &EngineScriptMethod::s_getTimeFactor, "use getTimeFactor" );
        pybind::def_functor_deprecated( kernel, "setTimingFactor", nodeScriptMethod, &EngineScriptMethod::s_setTimeFactor, "use setTimeFactor" );
        pybind::def_functor( kernel, "getTimeFactor", nodeScriptMethod, &EngineScriptMethod::s_getTimeFactor );
        pybind::def_functor( kernel, "setTimeFactor", nodeScriptMethod, &EngineScriptMethod::s_setTimeFactor );


        pybind::def_functor( kernel, "addHomeless", nodeScriptMethod, &EngineScriptMethod::s_addHomeless );
        pybind::def_functor( kernel, "isHomeless", nodeScriptMethod, &EngineScriptMethod::s_isHomeless );

        pybind::def_functor( kernel, "cancelTask", nodeScriptMethod, &EngineScriptMethod::s_cancelTask );
        pybind::def_functor( kernel, "joinTask", nodeScriptMethod, &EngineScriptMethod::s_joinTask );

        pybind::def_functor( kernel, "getGameAspect", nodeScriptMethod, &EngineScriptMethod::s_getGameAspect );
        pybind::def_functor( kernel, "getGameViewport", nodeScriptMethod, &EngineScriptMethod::s_getGameViewport );

        pybind::def_functor_kernel( kernel, "getGameParam", nodeScriptMethod, &EngineScriptMethod::s_getGameParam );
        pybind::def_functor_kernel( kernel, "getGameParamFloat", nodeScriptMethod, &EngineScriptMethod::s_getGameParamFloat );
        pybind::def_functor_kernel( kernel, "getGameParamInt", nodeScriptMethod, &EngineScriptMethod::s_getGameParamInt );
        pybind::def_functor_kernel( kernel, "getGameParamBool", nodeScriptMethod, &EngineScriptMethod::s_getGameParamBool );

        pybind::def_functor( kernel, "hasGameParam", nodeScriptMethod, &EngineScriptMethod::s_hasGameParam );
        pybind::def_functor( kernel, "openUrlInDefaultBrowser", nodeScriptMethod, &EngineScriptMethod::s_openUrlInDefaultBrowser );

        pybind::def_functor( kernel, "getDefaultResourceFontName", nodeScriptMethod, &EngineScriptMethod::s_getDefaultResourceFontName );


        pybind::def_functor( kernel, "pushMouseMove", nodeScriptMethod, &EngineScriptMethod::s_pushMouseMove );
        pybind::def_functor( kernel, "pushMouseButtonEvent", nodeScriptMethod, &EngineScriptMethod::s_pushMouseButtonEvent );

        pybind::def_functor( kernel, "platformEvent", nodeScriptMethod, &EngineScriptMethod::s_platformEvent );

        pybind::def_functor( kernel, "getProjectCodename", nodeScriptMethod, &EngineScriptMethod::s_getProjectCodename );

        pybind::def_functor( kernel, "sleep", nodeScriptMethod, &EngineScriptMethod::s_sleep );




        pybind::def_functor( kernel, "mountResourcePak", nodeScriptMethod, &EngineScriptMethod::s_mountResourcePak );
        pybind::def_functor( kernel, "unmountResourcePak", nodeScriptMethod, &EngineScriptMethod::s_unmountResourcePak );

        pybind::def_functor( kernel, "existFile", nodeScriptMethod, &EngineScriptMethod::s_existFile );
        pybind::def_functor( kernel, "removeFile", nodeScriptMethod, &EngineScriptMethod::s_removeFile );
        pybind::def_functor_kernel( kernel, "parseXml", nodeScriptMethod, &EngineScriptMethod::s_parseXml );

        pybind::def_functor( kernel, "visitFonts", nodeScriptMethod, &EngineScriptMethod::s_visitFonts );
        pybind::def_functor_kernel( kernel, "getFonts", nodeScriptMethod, &EngineScriptMethod::s_getFonts );
        pybind::def_functor( kernel, "hasFont", nodeScriptMethod, &EngineScriptMethod::s_hasFont );
        pybind::def_functor( kernel, "validateFont", nodeScriptMethod, &EngineScriptMethod::s_validateFont );

        pybind::def_functor( kernel, "cacheResources", nodeScriptMethod, &EngineScriptMethod::s_cacheResources );
        pybind::def_functor( kernel, "uncacheResources", nodeScriptMethod, &EngineScriptMethod::s_uncacheResources );

        pybind::def_functor( kernel, "rotateToTrimetric", nodeScriptMethod, &EngineScriptMethod::s_rotateToTrimetric );
        pybind::def_functor( kernel, "rotateToTrimetric2", nodeScriptMethod, &EngineScriptMethod::s_rotateToTrimetric2 );

        pybind::def_functor( kernel, "hotspotCorrect", nodeScriptMethod, &EngineScriptMethod::s_hotspotCorrect );

        pybind::def_functor( kernel, "copyUserPicture", nodeScriptMethod, &EngineScriptMethod::s_copyUserPicture );
        pybind::def_functor( kernel, "copyUserMusic", nodeScriptMethod, &EngineScriptMethod::s_copyUserMusic );

        pybind::def_functor( kernel, "screenToWorldPoint", nodeScriptMethod, &EngineScriptMethod::s_screenToWorldPoint );
        pybind::def_functor( kernel, "screenToWorldClick", nodeScriptMethod, &EngineScriptMethod::s_screenToWorldClick );

        pybind::def_functor_args( kernel, "addMousePositionProvider", nodeScriptMethod, &EngineScriptMethod::s_addMousePositionProvider );
        pybind::def_functor( kernel, "removeMousePositionProvider", nodeScriptMethod, &EngineScriptMethod::s_removeMousePositionProvider );

        pybind::def_functor( kernel, "gridBurnTransparency", nodeScriptMethod, &EngineScriptMethod::s_gridBurnTransparency );

        pybind::def_functor( kernel, "setLocale", nodeScriptMethod, &EngineScriptMethod::s_setLocale );

        pybind::def_functor_args( kernel, "createAffector", nodeScriptMethod, &EngineScriptMethod::s_createAffector );
        pybind::def_functor_args( kernel, "addAffector", nodeScriptMethod, &EngineScriptMethod::s_addAffector );
        pybind::def_functor( kernel, "removeAffector", nodeScriptMethod, &EngineScriptMethod::s_removeAffector );

        pybind::interface_<EngineScriptMethod::AffectorFollower, pybind::bases<Affector> >( kernel, "AffectorFollower" )
            .def( "follow", &EngineScriptMethod::AffectorFollower::follow )
            ;

        pybind::def_functor( kernel, "addNodeFollowerLocalAlpha", nodeScriptMethod, &EngineScriptMethod::s_addNodeFollowerLocalAlpha );
        pybind::def_functor( kernel, "addShapeFollowerCustomSize", nodeScriptMethod, &EngineScriptMethod::s_addShapeFollowerCustomSize );
        pybind::def_functor( kernel, "addShapeFollowerTextureUVScale", nodeScriptMethod, &EngineScriptMethod::s_addShapeFollowerTextureUVScale );
        pybind::def_functor( kernel, "removeNodeFollower", nodeScriptMethod, &EngineScriptMethod::s_removeNodeFollower );

        pybind::def_functor( kernel, "moduleMessage", nodeScriptMethod, &EngineScriptMethod::s_moduleMessage );

        pybind::def_functor( kernel, "findNodeScene", nodeScriptMethod, &EngineScriptMethod::s_findNodeScene );

        pybind::def_functor( kernel, "getCameraPosition", nodeScriptMethod, &EngineScriptMethod::s_getCameraPosition );

        pybind::interface_<PythonValueFollower, pybind::bases<Affector, Scriptable> >( kernel, "PythonValueFollower" )
            ;

        pybind::interface_<PythonValueFollowerLinear, pybind::bases<PythonValueFollower> >( kernel, "PythonValueFollowerLinear" )
            .def( "setSpeed", &PythonValueFollowerLinear::setSpeed )
            .def( "getSpeed", &PythonValueFollowerLinear::getSpeed )
            .def( "setValue", &PythonValueFollowerLinear::setValue )
            .def( "getValue", &PythonValueFollowerLinear::getValue )
            .def( "setFollow", &PythonValueFollowerLinear::setFollow )
            .def( "getFollow", &PythonValueFollowerLinear::getFollow )
            .def( "resetValue", &PythonValueFollowerLinear::resetValue )
            ;

        pybind::interface_<PythonValueFollowerAcceleration, pybind::bases<PythonValueFollower> >( kernel, "PythonValueFollowerAcceleration" )
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

        pybind::def_functor_args( kernel, "createValueFollowerLinear", nodeScriptMethod, &EngineScriptMethod::s_createValueFollowerLinear );
        pybind::def_functor_args( kernel, "createValueFollowerAcceleration", nodeScriptMethod, &EngineScriptMethod::s_createValueFollowerAcceleration );
        pybind::def_functor( kernel, "destroyValueFollower", nodeScriptMethod, &EngineScriptMethod::s_destroyValueFollower );

        m_implement = nodeScriptMethod;

        VOCALUBARY_SET( ScriptWrapperInterface, STRINGIZE_STRING_LOCAL( "ClassWrapping" ), STRINGIZE_STRING_LOCAL( "PythonValueFollowerLinear" ), new FactorableUnique<PythonScriptWrapper<PythonValueFollowerLinear> >( kernel ) );
        VOCALUBARY_SET( ScriptWrapperInterface, STRINGIZE_STRING_LOCAL( "ClassWrapping" ), STRINGIZE_STRING_LOCAL( "PythonValueFollowerAcceleration" ), new FactorableUnique<PythonScriptWrapper<PythonValueFollowerAcceleration> >( kernel ) );

        if( PROTOTYPE_SERVICE()
            ->addPrototype( STRINGIZE_STRING_LOCAL( "Affector" ), STRINGIZE_STRING_LOCAL( "PythonValueFollowerLinear" ), new FactorableUnique<ScriptablePrototypeGenerator<PythonValueFollowerLinear, 32> >() ) == false )
        {
            return false;
        }

        if( PROTOTYPE_SERVICE()
            ->addPrototype( STRINGIZE_STRING_LOCAL( "Affector" ), STRINGIZE_STRING_LOCAL( "PythonValueFollowerAcceleration" ), new FactorableUnique<ScriptablePrototypeGenerator<PythonValueFollowerAcceleration, 32> >() ) == false )
        {
            return false;
        }

        pybind::interface_<RandomizerInterface, pybind::bases<Mixin> >( kernel, "Randomizer" )
            .def( "setSeed", &RandomizerInterface::setSeed )
            .def( "getRandom", &RandomizerInterface::getRandom )
            .def( "getRandomRange", &RandomizerInterface::getRandomRange )
            .def( "getRandomf", &RandomizerInterface::getRandomf )
            .def( "getRandomRangef", &RandomizerInterface::getRandomRangef )
            ;

        if( PROTOTYPE_SERVICE()
            ->addPrototype( STRINGIZE_STRING_LOCAL( "Randomizer" ), STRINGIZE_STRING_LOCAL( "MT19937Randomizer" ), new FactorableUnique<DefaultPrototypeGenerator<MT19937Randomizer, 8> >() ) == false )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void EngineScriptWrapper::ejecting()
    {
        m_implement = nullptr;

        VOCALUBARY_REMOVE( STRINGIZE_STRING_LOCAL( "ClassWrapping" ), STRINGIZE_STRING_LOCAL( "PythonValueFollowerLinear" ) );
        VOCALUBARY_REMOVE( STRINGIZE_STRING_LOCAL( "ClassWrapping" ), STRINGIZE_STRING_LOCAL( "PythonValueFollowerAcceleration" ) );

        PROTOTYPE_SERVICE()->removePrototype( STRINGIZE_STRING_LOCAL( "Affector" ), STRINGIZE_STRING_LOCAL( "PythonValueFollowerLinear" ) );
        PROTOTYPE_SERVICE()->removePrototype( STRINGIZE_STRING_LOCAL( "Affector" ), STRINGIZE_STRING_LOCAL( "PythonValueFollowerAcceleration" ) );
        PROTOTYPE_SERVICE()->removePrototype( STRINGIZE_STRING_LOCAL( "Randomizer" ), STRINGIZE_STRING_LOCAL( "MT19937Randomizer" ) );
    }
}
