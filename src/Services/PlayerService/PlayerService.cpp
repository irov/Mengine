#include "PlayerService.h"

#include "Interface/PrototypeServiceInterface.h"
#include "Interface/RenderServiceInterface.h"
#include "Interface/RenderSystemInterface.h"
#include "Interface/ScriptServiceInterface.h"
#include "Interface/ResourceServiceInterface.h"
#include "Interface/GraveyardInterface.h"
#include "Interface/TimelineServiceInterface.h"
#include "Interface/FactoryServiceInterface.h"
#include "Interface/UpdateServiceInterface.h"
#include "Interface/UnicodeSystemInterface.h"
#include "Interface/NotificationServiceInterface.h"
#include "Interface/StringizeServiceInterface.h"
#include "Interface/PrefetcherServiceInterface.h"
#include "Interface/ModuleInterface.h"
#include "Interface/ApplicationInterface.h"
#include "Interface/MemoryServiceInterface.h"
#include "Interface/SceneServiceInterface.h"
#include "Interface/ModuleServiceInterface.h"
#include "Interface/EnumeratorServiceInterface.h"
#include "Interface/ChronometerServiceInterface.h"
#include "Interface/TimeSystemInterface.h"

#include "Plugins/AstralaxParticlePlugin/AstralaxInterface.h"
#include "Plugins/NodeDebugRenderPlugin/NodeDebugRenderServiceInterface.h"

#include "Config/Stringstream.h"

#include "Kernel/Scene.h"

#include "Kernel/NodeRenderHelper.h"
#include "Kernel/RenderViewport.h"
#include "Kernel/RenderScissor.h"
#include "Kernel/RenderCameraOrthogonal.h"
#include "Kernel/RenderCameraHelper.h"
#include "Kernel/Arrow.h"
#include "Kernel/ThreadTask.h"

#include "Kernel/Scheduler.h"

#include "Kernel/FactoryDefault.h"
#include "Kernel/FactoryPool.h"
#include "Kernel/AssertionFactory.h"

#include "PlayerGlobalInputHandler.h"
#include "PlayerGlobalAffectorable.h"

#include "Kernel/Logger.h"

#include "Kernel/Resource.h"

#include "math/mat3.h"

#include "pybind/pybind.hpp"
#include "stdex/allocator_report.h"

#include <iomanip>
#include <algorithm>

//////////////////////////////////////////////////////////////////////////
SERVICE_FACTORY( PlayerService, Mengine::PlayerService );
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    PlayerService::PlayerService()
        : m_arrowHided( false )
        , m_focus( true )
        , m_fps( 0 )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    PlayerService::~PlayerService()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    void PlayerService::setArrow( const ArrowPtr & _arrow )
    {
        if( m_arrow != nullptr )
        {
            m_arrow->disable();
            m_arrow = nullptr;
        }

        m_arrow = _arrow;

        if( m_arrow != nullptr )
        {
            m_arrow->setRenderCamera( m_arrowCamera2D );
            m_arrow->setRenderViewport( m_renderViewport );
            m_arrow->setRenderScissor( m_renderScissor );

            m_arrow->enable();
        }

        PICKER_SERVICE()
            ->setArrow( m_arrow );
    }
    //////////////////////////////////////////////////////////////////////////
    const ArrowPtr & PlayerService::getArrow() const
    {
        return m_arrow;
    }
    //////////////////////////////////////////////////////////////////////////
    void PlayerService::calcGlobalMouseWorldPosition( const mt::vec2f & _screenPoint, mt::vec2f * _worldPoint )
    {
        Helper::screenToWorldPosition( m_renderCamera, _screenPoint, _worldPoint );
    }
    //////////////////////////////////////////////////////////////////////////
    void PlayerService::calcGlobalMouseWorldDelta( const mt::vec2f & _screenDeltha, mt::vec2f * _worldDeltha )
    {
        Helper::screenToWorldDelta( m_renderCamera, _screenDeltha, _worldDeltha );
    }
    //////////////////////////////////////////////////////////////////////////
    SchedulerInterfacePtr PlayerService::createSchedulerManager( const ConstString & _name )
    {
        SchedulerPtr sm = m_factoryScheduleManager->createObject();

        if( sm->initialize( _name ) == false )
        {
            return nullptr;
        }

        m_schedulers.emplace_back( sm );

        return sm;
    }
    //////////////////////////////////////////////////////////////////////////
    bool PlayerService::destroySchedulerManager( const SchedulerInterfacePtr & _scheduler )
    {
        VectorUserScheduler::iterator it_found = std::find( m_schedulers.begin(), m_schedulers.end(), _scheduler );

        if( it_found == m_schedulers.end() )
        {
            LOGGER_ERROR( "Player::destroySchedulerManager scheduler not found!"
            );

            return false;
        }

        m_schedulers.erase( it_found );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    const GlobalInputHandlerInterfacePtr & PlayerService::getGlobalInputHandler() const
    {
        return m_globalInputHandler;
    }
    //////////////////////////////////////////////////////////////////////////
    const SchedulerInterfacePtr & PlayerService::getScheduler() const
    {
        return m_scheduleManager;
    }
    //////////////////////////////////////////////////////////////////////////
    const SchedulerInterfacePtr & PlayerService::getGlobalScheduler() const
    {
        return m_scheduleManagerGlobal;
    }
    //////////////////////////////////////////////////////////////////////////
    const AffectorablePtr & PlayerService::getAffectorable() const
    {
        return m_affectorable;
    }
    //////////////////////////////////////////////////////////////////////////
    const AffectorablePtr & PlayerService::getGlobalAffectorable() const
    {
        return m_affectorableGlobal;
    }
    //////////////////////////////////////////////////////////////////////////
    bool PlayerService::_initializeService()
    {
        m_globalInputHandler = new FactorableUnique<PlayerGlobalInputHandler>();

        m_factoryScheduleManager = new FactoryPool<Scheduler, 16>();

        SchedulerPtr scheduleManager = m_factoryScheduleManager->createObject();

        if( scheduleManager->initialize( "LocalScheduleManager"_c ) == false )
        {
            return false;
        }

        m_scheduleManager = scheduleManager;

        SchedulerPtr scheduleManagerGlobal = m_factoryScheduleManager->createObject();

        if( scheduleManagerGlobal->initialize( "GlobalScheduleManager"_c ) == false )
        {
            return false;
        }

        m_scheduleManagerGlobal = scheduleManagerGlobal;

        m_affectorable = new FactorableUnique<PlayerGlobalAffectorable>();
        m_affectorableGlobal = new FactorableUnique<PlayerGlobalAffectorable>();

        NOTIFICATION_ADDOBSERVERMETHOD( NOTIFICATOR_CHANGE_SCENE_PREPARE_DESTROY, this, &PlayerService::notifyChangeScenePrepareDestroy );
        NOTIFICATION_ADDOBSERVERMETHOD( NOTIFICATOR_CHANGE_SCENE_DESTROY, this, &PlayerService::notifyChangeSceneDestroy );
        NOTIFICATION_ADDOBSERVERMETHOD( NOTIFICATOR_CHANGE_SCENE_INITIALIZE, this, &PlayerService::notifyChangeScenePrepareInitialize );
        NOTIFICATION_ADDOBSERVERMETHOD( NOTIFICATOR_CHANGE_SCENE_ENABLE, this, &PlayerService::notifyChangeSceneEnable );
        NOTIFICATION_ADDOBSERVERMETHOD( NOTIFICATOR_CHANGE_SCENE_PREPARE_COMPLETE, this, &PlayerService::notifyChangeScenePrepareComplete );
        NOTIFICATION_ADDOBSERVERMETHOD( NOTIFICATOR_RESTART_SCENE_PREPARE_DISABLE, this, &PlayerService::notifyRestartScenePrepareDisable );
        NOTIFICATION_ADDOBSERVERMETHOD( NOTIFICATOR_RESTART_SCENE_DISABLE, this, &PlayerService::notifyRestartSceneDisable );
        NOTIFICATION_ADDOBSERVERMETHOD( NOTIFICATOR_RESTART_SCENE_ENABLE, this, &PlayerService::notifyRestartSceneEnable );
        NOTIFICATION_ADDOBSERVERMETHOD( NOTIFICATOR_REMOVE_SCENE_PREPARE_DESTROY, this, &PlayerService::notifyRemoveScenePrepareDestroy );
        NOTIFICATION_ADDOBSERVERMETHOD( NOTIFICATOR_REMOVE_SCENE_DESTROY, this, &PlayerService::notifyRemoveSceneDestroy );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void PlayerService::_finalizeService()
    {
        NOTIFICATION_REMOVEOBSERVER( NOTIFICATOR_CHANGE_SCENE_PREPARE_DESTROY, this );
        NOTIFICATION_REMOVEOBSERVER( NOTIFICATOR_CHANGE_SCENE_DESTROY, this );
        NOTIFICATION_REMOVEOBSERVER( NOTIFICATOR_CHANGE_SCENE_INITIALIZE, this );

        if( m_arrow != nullptr )
        {
            m_arrow->release();
            m_arrow = nullptr;
        }

        if( m_camera2D != nullptr )
        {
            m_camera2D->release();
            m_camera2D = nullptr;
        }

        if( m_viewport2D != nullptr )
        {
            m_viewport2D->release();
            m_viewport2D = nullptr;
        }

        if( m_arrowCamera2D != nullptr )
        {
            m_arrowCamera2D->release();
            m_arrowCamera2D = nullptr;
        }

        m_renderViewport = nullptr;
        m_renderCamera = nullptr;
        m_renderScissor = nullptr;

        m_renderTarget = nullptr;

        m_globalInputHandler = nullptr;

        if( m_scheduleManager != nullptr )
        {
            m_scheduleManager->finalize();
            m_scheduleManager = nullptr;
        }

        if( m_scheduleManagerGlobal != nullptr )
        {
            m_scheduleManagerGlobal->finalize();
            m_scheduleManagerGlobal = nullptr;
        }

        m_schedulers.clear();

        m_affectorable = nullptr;
        m_affectorableGlobal = nullptr;

        m_switchSceneTo = nullptr;

        if( m_debugCamera2D != nullptr )
        {
            m_debugCamera2D->disable();
            m_debugCamera2D = nullptr;
        }

        MENGINE_ASSERTION_FACTORY_EMPTY( m_factoryScheduleManager );
        m_factoryScheduleManager = nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
    void PlayerService::initializeRenderResources()
    {
        const Resolution & contentResolution = APPLICATION_SERVICE()
            ->getContentResolution();

        mt::vec2f cr;
        contentResolution.calcSize( cr );
        Viewport vp( 0.f, 0.f, cr.x, cr.y );

        m_camera2D = PROTOTYPE_SERVICE()
            ->generatePrototype( STRINGIZE_STRING_LOCAL( "Node" ), STRINGIZE_STRING_LOCAL( "RenderCameraOrthogonal" ) );

        m_camera2D->setOrthogonalViewport( vp );

        m_camera2D->enable();

        this->setRenderCamera( m_camera2D );

        m_viewport2D = PROTOTYPE_SERVICE()
            ->generatePrototype( STRINGIZE_STRING_LOCAL( "Node" ), STRINGIZE_STRING_LOCAL( "RenderViewport" ) );

        m_viewport2D->setViewport( vp );
        m_viewport2D->enable();

        this->setRenderViewport( m_viewport2D );

        m_arrowCamera2D = PROTOTYPE_SERVICE()
            ->generatePrototype( STRINGIZE_STRING_LOCAL( "Node" ), STRINGIZE_STRING_LOCAL( "RenderCameraOrthogonal" ) );

        m_arrowCamera2D->setOrthogonalViewport( vp );
        m_arrowCamera2D->enable();

        if( m_arrow != nullptr )
        {
            m_arrow->setRenderCamera( m_arrowCamera2D );
            m_arrow->setRenderViewport( m_renderViewport );
            m_arrow->setRenderScissor( m_renderScissor );
        }

        m_debugCamera2D = PROTOTYPE_SERVICE()
            ->generatePrototype( STRINGIZE_STRING_LOCAL( "Node" ), STRINGIZE_STRING_LOCAL( "RenderCameraOrthogonal" ) );

        m_debugCamera2D->setOrthogonalViewport( vp );

        m_debugCamera2D->enable();
    }
    //////////////////////////////////////////////////////////////////////////
    void PlayerService::finalizeRenderResources()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool PlayerService::handleKeyEvent( const InputKeyEvent & _event )
    {
        bool handler = false;

        if( m_globalInputHandler != nullptr )
        {
            m_globalInputHandler->handleKeyEvent( _event );
        }

        if( handler == false )
        {
            handler = PICKER_SERVICE()
                ->handleKeyEvent( _event );
        }

        return handler;
    }
    //////////////////////////////////////////////////////////////////////////
    bool PlayerService::handleTextEvent( const InputTextEvent & _event )
    {
        bool handler = false;

        if( m_globalInputHandler != nullptr )
        {
            m_globalInputHandler->handleTextEvent( _event );
        }

        if( handler == false )
        {
            handler = PICKER_SERVICE()
                ->handleTextEvent( _event );
        }

        return handler;
    }
    //////////////////////////////////////////////////////////////////////////
    bool PlayerService::handleMouseButtonEvent( const InputMouseButtonEvent & _event )
    {
        if( m_globalInputHandler != nullptr )
        {
            m_globalInputHandler->handleMouseButtonEvent( _event );
        }

        bool handler = false;

        if( handler == false )
        {
            handler = PICKER_SERVICE()
                ->handleMouseButtonEvent( _event );
        }

        return handler;
    }
    //////////////////////////////////////////////////////////////////////////
    bool PlayerService::handleMouseButtonEventBegin( const InputMouseButtonEvent & _event )
    {
        if( m_globalInputHandler != nullptr )
        {
            m_globalInputHandler->handleMouseButtonEventBegin( _event );
        }

        bool handler = false;


        if( handler == false )
        {
            handler = PICKER_SERVICE()
                ->handleMouseButtonEventBegin( _event );
        }

        return handler;
    }
    //////////////////////////////////////////////////////////////////////////
    bool PlayerService::handleMouseButtonEventEnd( const InputMouseButtonEvent & _event )
    {
        if( m_globalInputHandler != nullptr )
        {
            m_globalInputHandler->handleMouseButtonEventEnd( _event );
        }

        bool handler = false;


        if( handler == false )
        {
            handler = PICKER_SERVICE()
                ->handleMouseButtonEventEnd( _event );
        }

        return handler;
    }
    //////////////////////////////////////////////////////////////////////////
    bool PlayerService::handleMouseMove( const InputMouseMoveEvent & _event )
    {
        if( m_globalInputHandler != nullptr )
        {
            m_globalInputHandler->handleMouseMove( _event );
        }

        bool handler = false;

        if( handler == false )
        {
            handler = PICKER_SERVICE()
                ->handleMouseMove( _event );
        }

        return handler;
    }
    //////////////////////////////////////////////////////////////////////////
    bool PlayerService::handleMouseWheel( const InputMouseWheelEvent & _event )
    {
        if( m_globalInputHandler != nullptr )
        {
            m_globalInputHandler->handleMouseWheel( _event );
        }

        bool handler = false;

        if( handler == false )
        {
            handler = PICKER_SERVICE()
                ->handleMouseWheel( _event );
        }

        return handler;
    }
    //////////////////////////////////////////////////////////////////////////
    void PlayerService::tick( const UpdateContext * _context )
    {
        UPDATE_SERVICE()
            ->update( _context );
    }
    //////////////////////////////////////////////////////////////////////////
    bool PlayerService::update()
    {
        PICKER_SERVICE()
            ->update();

        if( m_globalInputHandler != nullptr )
        {
            m_globalInputHandler->update();
        }

        CHRONOMETER_SERVICE()
            ->update();

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void PlayerService::setRenderCamera( const RenderCameraInterfacePtr & _camera )
    {
        m_renderCamera = _camera;

        PICKER_SERVICE()
            ->setRenderCamera( m_renderCamera );
    }
    //////////////////////////////////////////////////////////////////////////
    const RenderCameraInterfacePtr & PlayerService::getRenderCamera() const
    {
        return m_renderCamera;
    }
    //////////////////////////////////////////////////////////////////////////
    void PlayerService::setRenderViewport( const RenderViewportInterfacePtr & _viewport )
    {
        m_renderViewport = _viewport;

        PICKER_SERVICE()
            ->setRenderViewport( m_renderViewport );
    }
    //////////////////////////////////////////////////////////////////////////
    const RenderViewportInterfacePtr & PlayerService::getRenderViewport() const
    {
        return m_renderViewport;
    }
    //////////////////////////////////////////////////////////////////////////
    void PlayerService::setRenderScissor( const RenderScissorInterfacePtr & _scissor )
    {
        m_renderScissor = _scissor;


        PICKER_SERVICE()
            ->setRenderScissor( _scissor );
    }
    //////////////////////////////////////////////////////////////////////////
    const RenderScissorInterfacePtr & PlayerService::getRenderScissor() const
    {
        return m_renderScissor;
    }
    //////////////////////////////////////////////////////////////////////////
    void PlayerService::render()
    {
        //if( this->isChangedScene() == true )
        //{
        //	return;
        //}

        uint32_t debugMask = APPLICATION_SERVICE()
            ->getDebugMask();

        RenderContext context;
        context.viewport = m_renderViewport;
        context.camera = m_renderCamera;
        context.scissor = m_renderScissor;
        context.target = m_renderTarget;

        const ScenePtr & scene = SCENE_SERVICE()
            ->getCurrentScene();

        if( scene != nullptr )
        {
            if( debugMask == false )
            {
                //RenderInterface * render = m_scene->getRender();
                //render->renderWithChildren( &context, false );
                Helper::nodeRenderChildren( scene, &context, false );
            }
            else
            {
                if( SERVICE_EXIST( NodeDebugRenderServiceInterface ) == true )
                {
                    NODEDEBUGRENDER_SERVICE()
                        ->renderDebugNode( scene, &context, false );
                }
            }
        }

        MODULE_SERVICE()
            ->render( &context );

        RENDER_SERVICE()
            ->endLimitRenderObjects();

        if( m_arrow != nullptr )
        {
            if( debugMask == false )
            {
                //RenderInterface * render = m_arrow->getRender();
                //render->renderWithChildren( &context, false );
                Helper::nodeRenderChildren( m_arrow, &context, false );
            }
            else
            {
                if( SERVICE_EXIST( NodeDebugRenderServiceInterface ) == true )
                {
                    NODEDEBUGRENDER_SERVICE()
                        ->renderDebugNode( m_arrow, &context, false );
                }
            }
        }

        if( SERVICE_EXIST( NodeDebugRenderServiceInterface ) == true )
        {
            NODEDEBUGRENDER_SERVICE()
                ->renderDebugInfo( &context );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void PlayerService::onAppMouseLeave( const InputMousePositionEvent & _event )
    {
        if( m_arrow != nullptr )
        {
            m_arrow->onAppMouseLeave();
        }

        const ScenePtr & scene = SCENE_SERVICE()
            ->getCurrentScene();

        if( scene != nullptr && scene->isActivate() == true )
        {
            scene->onAppMouseLeave();
        }

        PICKER_SERVICE()
            ->handleMouseLeave( _event );
    }
    //////////////////////////////////////////////////////////////////////////
    void PlayerService::onAppMouseEnter( const InputMousePositionEvent & _event )
    {
        if( m_arrow != nullptr )
        {
            m_arrow->onAppMouseEnter();
        }

        const ScenePtr & scene = SCENE_SERVICE()
            ->getCurrentScene();

        if( scene != nullptr && scene->isActivate() == true )
        {
            scene->onAppMouseEnter();
        }

        PICKER_SERVICE()
            ->handleMouseEnter( _event );
    }
    //////////////////////////////////////////////////////////////////////////
    void PlayerService::onAppMousePosition( const InputMousePositionEvent & _event )
    {
        (void)_event;
    }
    //////////////////////////////////////////////////////////////////////////
    void PlayerService::onFullscreen( const Resolution & _resolution, bool _fullscreen )
    {
        (void)_resolution;
        (void)_fullscreen;
    }
    //////////////////////////////////////////////////////////////////////////
    void PlayerService::onFixedContentResolution( const Resolution & _resolution, bool _fixed )
    {
        (void)_resolution;
        (void)_fixed;
    }
    //////////////////////////////////////////////////////////////////////////
    void PlayerService::onFixedDisplayResolution( const Resolution & _resolution, bool _fixed )
    {
        (void)_resolution;
        (void)_fixed;
    }
    //////////////////////////////////////////////////////////////////////////
    void PlayerService::onFocus( bool _focus )
    {
        m_focus = _focus;

        const ScenePtr & scene = SCENE_SERVICE()
            ->getCurrentScene();

        if( scene != nullptr && scene->isActivate() == true )
        {
            scene->onFocus( _focus );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void PlayerService::notifyChangeScenePrepareDestroy( const ScenePtr & _oldScene, const ScenePtr & _newScene )
    {
        MENGINE_UNUSED( _oldScene );
        MENGINE_UNUSED( _newScene );

        MODULE_SERVICE()
            ->messageAll( STRINGIZE_STRING_LOCAL( "onSceneChange" ), MapWParams() );

        if( m_arrow != nullptr )
        {
            m_arrow->removeFromParent();
            m_arrow->disable();
        }

        m_scheduleManager->removeAll();
        m_affectorable->stopAllAffectors();
    }
    //////////////////////////////////////////////////////////////////////////
    void PlayerService::notifyChangeSceneDestroy( const ScenePtr & _scene )
    {
        MENGINE_UNUSED( _scene );

        if( SERVICE_EXIST( Mengine::GraveyardInterface ) == true )
        {
            GRAVEYARD_SERVICE()
                ->clearTextures();
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void PlayerService::notifyChangeScenePrepareInitialize( const ScenePtr & _scene )
    {
        PICKER_SERVICE()
            ->setScene( _scene );
    }
    //////////////////////////////////////////////////////////////////////////
    void PlayerService::notifyChangeSceneEnable( const ScenePtr & _scene )
    {
        MENGINE_UNUSED( _scene );

        if( m_arrow != nullptr )
        {
            m_arrow->enableForce();
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void PlayerService::notifyChangeScenePrepareComplete( const ScenePtr & _scene )
    {
        MENGINE_UNUSED( _scene );

        MEMORY_SERVICE()
            ->clearCacheBuffers();
    }
    //////////////////////////////////////////////////////////////////////////
    void PlayerService::notifyRestartScenePrepareDisable( const ScenePtr & _scene )
    {
        MENGINE_UNUSED( _scene );

        MODULE_SERVICE()
            ->messageAll( STRINGIZE_STRING_LOCAL( "onSceneRestart" ), MapWParams() );

        if( m_arrow != nullptr )
        {
            m_arrow->removeFromParent();
            m_arrow->disable();
        }

        m_scheduleManager->removeAll();
        m_affectorable->stopAllAffectors();
    }
    //////////////////////////////////////////////////////////////////////////
    void PlayerService::notifyRestartSceneDisable( const ScenePtr & _scene )
    {
        MENGINE_UNUSED( _scene );

        if( SERVICE_EXIST( Mengine::GraveyardInterface ) == true )
        {
            GRAVEYARD_SERVICE()
                ->clearTextures();
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void PlayerService::notifyRestartSceneEnable( const ScenePtr & _scene )
    {
        MENGINE_UNUSED( _scene );

        if( m_arrow != nullptr )
        {
            m_arrow->enableForce();
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void PlayerService::notifyRemoveScenePrepareDestroy( const ScenePtr & _scene )
    {
        MENGINE_UNUSED( _scene );

        if( m_arrow != nullptr )
        {
            m_arrow->removeFromParent();
            m_arrow->disable();
        }

        m_scheduleManager->removeAll();
        m_affectorable->stopAllAffectors();

        m_globalInputHandler->clear();
    }
    //////////////////////////////////////////////////////////////////////////
    void PlayerService::notifyRemoveSceneDestroy()
    {
        PICKER_SERVICE()
            ->setScene( nullptr );

        if( SERVICE_EXIST( Mengine::GraveyardInterface ) == true )
        {
            GRAVEYARD_SERVICE()
                ->clearTextures();
        }
    }
}
