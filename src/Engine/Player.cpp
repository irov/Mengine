#include "Player.h"

#include "Interface/RenderSystemInterface.h"
#include "Interface/ScriptSystemInterface.h"
#include "Interface/ParticleSystemInterface.h"
#include "Interface/ResourceInterface.h"
#include "Interface/GraveyardInterface.h"
#include "Interface/TimelineInterface.h"
#include "Interface/FactoryInterface.h"

#include "Interface/UnicodeInterface.h"

#include "Interface/NotificationServiceInterface.h"
#include "Interface/StringizeInterface.h"
#include "Interface/PrefetcherInterface.h"
#include "Interface/ModuleInterface.h"
#include "Interface/ApplicationInterface.h"

#include "Config/Stringstream.h"

#include "Kernel/Scene.h"

#include "Kernel/RenderViewport.h"
#include "Kernel/RenderClipplane.h"
#include "Kernel/RenderCameraOrthogonal.h"
#include "Kernel/RenderCameraHelper.h"
#include "Kernel/Arrow.h"
#include "Kernel/ThreadTask.h"

#include "Kernel/ScheduleManager.h"

#include "Factory/FactoryDefault.h"

#include "GlobalAffectorable.h"

#include "Logger/Logger.h"

#include "TextField.h"

#include "Kernel/ResourceReference.h"

#include "math/mat3.h"

#include "pybind/pybind.hpp"
#include "stdex/allocator_report.h"

#include <iomanip>
#include <algorithm>

//////////////////////////////////////////////////////////////////////////
SERVICE_EXTERN( MousePickerSystem );
SERVICE_EXTERN( GlobalHandleSystem );
//////////////////////////////////////////////////////////////////////////
SERVICE_FACTORY( PlayerService, Mengine::Player );
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////    
    namespace
    {
        class PlayerResourceUselessCompile
            : public FactorableUnique<Factorable>
            , public ObserverInterface
        {
        public:
            PlayerResourceUselessCompile()
            {
            }

            ~PlayerResourceUselessCompile()
            {
            }

        public:
            void begin()
            {
                NOTIFICATION_SERVICE()
                    ->addObserverMethod( NOTIFICATOR_RESOURCE_COMPILE, this, &PlayerResourceUselessCompile::resourceCompile );

                NOTIFICATION_SERVICE()
                    ->addObserverMethod( NOTIFICATOR_RESOURCE_RELEASE, this, &PlayerResourceUselessCompile::resourceRelease );
            }

            void end()
            {
                NOTIFICATION_SERVICE()
                    ->removeObserver( NOTIFICATOR_RESOURCE_COMPILE, this );

                NOTIFICATION_SERVICE()
                    ->removeObserver( NOTIFICATOR_RESOURCE_RELEASE, this );
            }

        protected:
            void resourceCompile( ResourceReference * _resource )
            {
                m_resources.emplace_back( _resource );
            }

            void resourceRelease( ResourceReference * _resource )
            {
                TVectorResourceDesc::iterator it_remove =
                    std::find( m_resources.begin(), m_resources.end(), _resource );

                if( it_remove == m_resources.end() )
                {
                    return;
                }

                LOGGER_PERFORMANCE( "Useless Compile %s %s:%s"
                    , _resource->getType().c_str()
                    , _resource->getGroup().c_str()
                    , _resource->getName().c_str()
                );

                m_resources.erase( it_remove );
            }

        protected:
            typedef stdex::vector<ResourceReference *> TVectorResourceDesc;
            TVectorResourceDesc m_resources;
        };

        typedef IntrusivePtr<PlayerResourceUselessCompile> PlayerResourceUselessCompilePtr;
    }
    //////////////////////////////////////////////////////////////////////////
    Player::Player()
        : m_arrowCamera2D( nullptr )
        , m_renderCamera( nullptr )
        , m_renderViewport( nullptr )
        , m_renderClipplane( nullptr )
        , m_renderTarget( nullptr )
        , m_switchSceneTo( nullptr )
        , m_mousePickerSystem( nullptr )
        , m_arrowHided( false )
        , m_fps( 0 )
        , m_showDebugText( 0 )
        , m_debugText( nullptr )
        , m_camera2D( nullptr )
        , m_viewport2D( nullptr )
        , m_debugCamera2D( nullptr )
        , m_focus( true )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    Player::~Player()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool Player::setCurrentScene( const ScenePtr & _scene, bool _destroyOld, const SceneChangeCallbackInterfacePtr & _cb )
    {
        if( _scene == nullptr )
        {
            return false;
        }

        MODULE_SERVICE()
            ->messageAll( STRINGIZE_STRING_LOCAL( "onSceneChange" ), TMapWParams() );

        ScenePtr oldScene = m_scene;
        m_scene = nullptr;

        if( m_arrow != nullptr )
        {
            m_arrow->removeFromParent();
            m_arrow->disable();
        }

        m_scheduleManager->removeAll();
        m_affectorable->stopAllAffectors();

        if( oldScene != nullptr && _destroyOld == true )
        {
            oldScene->destroy();

            //NODE_SERVICE() 
            //    ->clearHomeless();

            if( SERVICE_EXIST( Mengine::GraveyardInterface ) == true )
            {
                GRAVEYARD_SERVICE()
                    ->clearTextures();
            }
        }

        if( _cb != nullptr )
        {
            _cb->onSceneChange( nullptr, false, false );
        }

        m_scene = _scene;

        if( m_mousePickerSystem != nullptr )
        {
            m_mousePickerSystem->setScene( m_scene );
        }

        if( _cb != nullptr )
        {
            _cb->onSceneChange( m_scene, false, false );
        }

#   ifndef MENGINE_MASTER_RELEASE
        PlayerResourceUselessCompilePtr unlessCompile = new PlayerResourceUselessCompile;
        unlessCompile->begin();
#   endif

        m_scene->enable();

#   ifndef MENGINE_MASTER_RELEASE
        unlessCompile->end();
#   endif

        if( m_arrow != nullptr )
        {
            m_arrow->enable();
        }

        if( _cb != nullptr )
        {
            _cb->onSceneChange( m_scene, true, false );
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Player::restartCurrentScene( const SceneChangeCallbackInterfacePtr & _cb )
    {
        MODULE_SERVICE()
            ->messageAll( STRINGIZE_STRING_LOCAL( "onSceneChange" ), TMapWParams() );

        if( m_arrow != nullptr )
        {
            m_arrow->removeFromParent();
            m_arrow->disable();
        }

        m_scheduleManager->removeAll();
        m_affectorable->stopAllAffectors();

        m_scene->disable();

        //NODE_SERVICE() 
        //    ->clearHomeless();

        if( SERVICE_EXIST( Mengine::GraveyardInterface ) == true )
        {
            GRAVEYARD_SERVICE()
                ->clearTextures();
        }

        if( _cb != nullptr )
        {
            _cb->onSceneChange( nullptr, false, false );
        }

        if( _cb != nullptr )
        {
            _cb->onSceneChange( m_scene, false, false );
        }

#   ifndef MENGINE_MASTER_RELEASE
        PlayerResourceUselessCompilePtr unlessCompile = new PlayerResourceUselessCompile;
        unlessCompile->begin();
#   endif

        m_scene->enable();

#   ifndef MENGINE_MASTER_RELEASE
        unlessCompile->end();
#   endif

        if( m_arrow != nullptr )
        {
            m_arrow->enable();
        }

        if( _cb != nullptr )
        {
            _cb->onSceneChange( m_scene, true, false );
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Player::removeCurrentScene( const SceneChangeCallbackInterfacePtr & _cb )
    {
        if( m_scene != nullptr )
        {
            if( m_arrow != nullptr )
            {
                m_arrow->removeFromParent();
                m_arrow->disable();
            }

            m_scheduleManager->removeAll();
            m_affectorable->stopAllAffectors();

            m_globalHandleSystem->clear();

            m_scene->release();

            //NODE_SERVICE() 
            //    ->clearHomeless();

            if( SERVICE_EXIST( Mengine::GraveyardInterface ) == true )
            {
                GRAVEYARD_SERVICE()
                    ->clearTextures();
            }

            m_scene = nullptr;
        }

        if( m_mousePickerSystem != nullptr )
        {
            m_mousePickerSystem->setScene( nullptr );
        }

        if( _cb != nullptr )
        {
            _cb->onSceneChange( nullptr, false, true );
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void Player::destroyCurrentScene()
    {
        if( m_scene != nullptr )
        {
            ScenePtr destroyScene = m_scene;
            m_scene = nullptr;

            destroyScene->destroy();
        }
    }
    //////////////////////////////////////////////////////////////////////////
    const ScenePtr & Player::getCurrentScene()
    {
        return m_scene;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Player::createGlobalScene()
    {
        ScenePtr scene = PROTOTYPE_SERVICE()
            ->generatePrototype( STRINGIZE_STRING_LOCAL( "Node" ), STRINGIZE_STRING_LOCAL( "Scene" ) );

        if( scene == nullptr )
        {
            return false;
        }

        m_globalScene = scene;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void Player::removeGlobalScene()
    {
        if( m_globalScene != nullptr )
        {
            m_globalScene->destroy();
            m_globalScene = nullptr;
        }
    }
    //////////////////////////////////////////////////////////////////////////
    const ScenePtr & Player::getGlobalScene()
    {
        return m_globalScene;
    }
    //////////////////////////////////////////////////////////////////////////
    void Player::setArrow( const ArrowPtr & _arrow )
    {
        if( m_arrow != nullptr )
        {
            m_arrow->disable();
        }

        m_arrow = _arrow;

        if( m_arrow != nullptr )
        {
            m_arrow->setRenderCamera( m_arrowCamera2D );
            m_arrow->setRenderViewport( m_renderViewport );
            m_arrow->setRenderClipplane( m_renderClipplane );

            if( m_scene != nullptr )
            {
                m_arrow->enable();
            }
        }

        if( m_mousePickerSystem != nullptr )
        {
            m_mousePickerSystem->setArrow( m_arrow );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    const ArrowPtr & Player::getArrow() const
    {
        return m_arrow;
    }
    //////////////////////////////////////////////////////////////////////////
    void Player::calcGlobalMouseWorldPosition( const mt::vec2f & _screenPoint, mt::vec2f & _worldPoint )
    {
        Helper::screenToWorldPosition( m_renderCamera, _screenPoint, _worldPoint );
    }
    //////////////////////////////////////////////////////////////////////////
    void Player::calcGlobalMouseWorldDelta( const mt::vec2f & _screenPoint, const mt::vec2f & _screenDeltha, mt::vec2f & _worldDeltha )
    {
        Helper::screenToWorldDelta( m_renderCamera, _screenPoint, _screenDeltha, _worldDeltha );
    }
    //////////////////////////////////////////////////////////////////////////
    ScheduleManagerInterfacePtr Player::createSchedulerManager()
    {
        ScheduleManagerInterfacePtr sm = m_factoryScheduleManager->createObject();

        m_schedulers.emplace_back( sm );

        return sm;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Player::destroySchedulerManager( const ScheduleManagerInterfacePtr & _scheduler )
    {
        TVectorUserScheduler::iterator it_found = std::find( m_schedulers.begin(), m_schedulers.end(), _scheduler );

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
    const MousePickerSystemInterfacePtr & Player::getMousePickerSystem() const
    {
        return m_mousePickerSystem;
    }
    //////////////////////////////////////////////////////////////////////////
    const GlobalHandleSystemInterfacePtr & Player::getGlobalHandleSystem() const
    {
        return m_globalHandleSystem;
    }
    //////////////////////////////////////////////////////////////////////////
    const ScheduleManagerInterfacePtr & Player::getScheduleManager() const
    {
        return m_scheduleManager;
    }
    //////////////////////////////////////////////////////////////////////////
    const ScheduleManagerInterfacePtr & Player::getScheduleManagerGlobal() const
    {
        return m_scheduleManagerGlobal;
    }
    //////////////////////////////////////////////////////////////////////////
    const AffectorablePtr & Player::getAffectorable() const
    {
        return m_affectorable;
    }
    //////////////////////////////////////////////////////////////////////////
    const AffectorablePtr & Player::getAffectorableGlobal() const
    {
        return m_affectorableGlobal;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Player::_initialize()
    {
        m_mousePickerSystem = SERVICE_GENERATE( MousePickerSystem, MousePickerSystemInterface );
        m_globalHandleSystem = SERVICE_GENERATE( GlobalHandleSystem, GlobalHandleSystemInterface );

        m_factoryScheduleManager = new FactoryDefault<ScheduleManager>();

        ScheduleManagerInterfacePtr scheduleManager = m_factoryScheduleManager->createObject();

        m_scheduleManager = scheduleManager;

        ScheduleManagerInterfacePtr scheduleManagerGlobal = m_factoryScheduleManager->createObject();

        m_scheduleManagerGlobal = scheduleManagerGlobal;

        m_affectorable = new GlobalAffectorable;
        m_affectorableGlobal = new GlobalAffectorable;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void Player::_finalize()
    {
        m_scene = nullptr;
        m_globalScene = nullptr;
        
        m_camera2D = nullptr;
        
        m_debugCamera2D = nullptr;
        
        m_arrowCamera2D = nullptr;
        m_viewport2D = nullptr;
        
        m_mousePickerSystem = nullptr;
        m_globalHandleSystem = nullptr;
        
        m_scheduleManager = nullptr;
        m_scheduleManagerGlobal = nullptr;

        m_schedulers.clear();

        m_affectorable = nullptr;
        m_affectorableGlobal = nullptr;

        MENGINE_ASSERTION_FACTORY_EMPTY( m_factoryScheduleManager );
        m_factoryScheduleManager = nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
    void Player::initializeRenderResources()
    {
        m_debugText = NODE_SERVICE()->
            createNode( STRINGIZE_STRING_LOCAL( "TextField" ) );

        m_debugText->setFontName( STRINGIZE_STRING_LOCAL( "__CONSOLE_FONT__" ) );
        m_debugText->setTextID( STRINGIZE_STRING_LOCAL( "__ID_TEXT_CONSOLE" ) );
        m_debugText->setLocalColor( ColourValue( 1.0, 0.0, 0.0, 1.0 ) );
        m_debugText->enable();

        const Resolution & contentResolution = APPLICATION_SERVICE()
            ->getContentResolution();

        mt::vec2f cr;
        contentResolution.calcSize( cr );
        Viewport vp( 0.f, 0.f, cr.x, cr.y );

        m_camera2D = NODE_SERVICE()
            ->createNode( STRINGIZE_STRING_LOCAL( "RenderCameraOrthogonal" ) );

        m_camera2D->setOrthogonalViewport( vp );

        m_camera2D->enable();

        this->setRenderCamera( m_camera2D );

        m_viewport2D = NODE_SERVICE()
            ->createNode( STRINGIZE_STRING_LOCAL( "RenderViewport" ) );

        m_viewport2D->setViewport( vp );
        m_viewport2D->enable();

        this->setRenderViewport( m_viewport2D );

        m_arrowCamera2D = NODE_SERVICE()
            ->createNode( STRINGIZE_STRING_LOCAL( "RenderCameraOrthogonal" ) );

        m_arrowCamera2D->setOrthogonalViewport( vp );
        m_arrowCamera2D->enable();

        if( m_arrow != nullptr )
        {
            m_arrow->setRenderCamera( m_arrowCamera2D );
            m_arrow->setRenderViewport( m_renderViewport );
            m_arrow->setRenderClipplane( m_renderClipplane );
        }

        m_debugCamera2D = NODE_SERVICE()
            ->createNode( STRINGIZE_STRING_LOCAL( "RenderCameraOrthogonal" ) );

        m_debugCamera2D->setOrthogonalViewport( vp );

        m_debugCamera2D->enable();
    }
    //////////////////////////////////////////////////////////////////////////
    void Player::finalizeRenderResources()
    {
        if( m_debugText != nullptr )
        {
            m_debugText->destroy();
            m_debugText = nullptr;
        }
    }
    //////////////////////////////////////////////////////////////////////////
    bool Player::handleKeyEvent( const InputKeyEvent & _event )
    {
        bool handler = false;

        if( m_globalHandleSystem != nullptr )
        {
            m_globalHandleSystem->handleKeyEvent( _event );
        }

        if( m_mousePickerSystem != nullptr )
        {
            if( handler == false )
            {
                handler = m_mousePickerSystem->handleKeyEvent( _event );
            }
        }

        return handler;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Player::handleTextEvent( const InputTextEvent & _event )
    {
        bool handler = false;

        if( m_globalHandleSystem != nullptr )
        {
            m_globalHandleSystem->handleTextEvent( _event );
        }

        if( m_mousePickerSystem != nullptr )
        {
            if( handler == false )
            {
                handler = m_mousePickerSystem->handleTextEvent( _event );
            }
        }

        return handler;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Player::handleMouseButtonEvent( const InputMouseButtonEvent & _event )
    {
        if( m_globalHandleSystem != nullptr )
        {
            m_globalHandleSystem->handleMouseButtonEvent( _event );
        }

        bool handler = false;

        if( m_mousePickerSystem != nullptr )
        {
            if( handler == false )
            {
                handler = m_mousePickerSystem->handleMouseButtonEvent( _event );
            }
        }

        return handler;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Player::handleMouseButtonEventBegin( const InputMouseButtonEvent & _event )
    {
        if( m_globalHandleSystem != nullptr )
        {
            m_globalHandleSystem->handleMouseButtonEventBegin( _event );
        }

        bool handler = false;

        if( m_mousePickerSystem != nullptr )
        {
            if( handler == false )
            {
                handler = m_mousePickerSystem->handleMouseButtonEventBegin( _event );
            }
        }

        return handler;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Player::handleMouseButtonEventEnd( const InputMouseButtonEvent & _event )
    {
        if( m_globalHandleSystem != nullptr )
        {
            m_globalHandleSystem->handleMouseButtonEventEnd( _event );
        }

        bool handler = false;

        if( m_mousePickerSystem != nullptr )
        {
            if( handler == false )
            {
                handler = m_mousePickerSystem->handleMouseButtonEventEnd( _event );
            }
        }

        return handler;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Player::handleMouseMove( const InputMouseMoveEvent & _event )
    {
        if( m_globalHandleSystem != nullptr )
        {
            m_globalHandleSystem->handleMouseMove( _event );
        }

        bool handler = false;

        if( m_mousePickerSystem != nullptr )
        {
            if( handler == false )
            {
                handler = m_mousePickerSystem->handleMouseMove( _event );
            }
        }

        return handler;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Player::handleMouseWheel( const InputMouseWheelEvent & _event )
    {
        if( m_globalHandleSystem != nullptr )
        {
            m_globalHandleSystem->handleMouseWheel( _event );
        }

        bool handler = false;

        if( m_mousePickerSystem != nullptr )
        {
            if( handler == false )
            {
                handler = m_mousePickerSystem->handleMouseWheel( _event );
            }
        }

        return handler;
    }
    //////////////////////////////////////////////////////////////////////////
    void Player::tick( float _time, float _timing )
    {
        static float fpsTiming = 0.0f;
        fpsTiming += _timing;
        if( fpsTiming >= 1000.0f )
        {
            const RenderServiceDebugInfo & debugInfo = RENDER_SERVICE()
                ->getDebugInfo();

            m_fps = debugInfo.frameCount;

            RENDER_SERVICE()
                ->resetFrameCount();

            while( fpsTiming >= 1000.0f )
            {
                fpsTiming -= 1000.0f;
            }
        }

        //if( PhysicEngine2D::get()->isWorldCreate() )
        //{
        //	const mt::vec2f & arrowPos = 
        //		m_arrow->getLocalPosition() + m_renderCamera2D->getViewport().begin;

        //	PhysicEngine2D::get()
        //		->onMouseMove( arrowPos );
        //}

        if( m_camera2D != nullptr )
        {
            m_camera2D->update( _time, _timing );
        }

        if( m_arrowCamera2D != nullptr )
        {
            m_arrowCamera2D->update( _time, _timing );
        }

        if( m_arrow != nullptr )
        {
            m_arrow->update( _time, _timing );
        }

        if( m_globalScene != nullptr )
        {
            m_globalScene->update( _time, _timing );
        }

        if( m_scene != nullptr )
        {
            m_scene->update( _time, _timing );
        }

        if( m_scheduleManager != nullptr )
        {
            m_scheduleManager->update( _time, _timing );
        }

        if( m_scheduleManagerGlobal != nullptr )
        {
            m_scheduleManagerGlobal->update( _time, _timing );
        }

        for( const ScheduleManagerInterfacePtr & sm : m_schedulers )
        {
            sm->update( _time, _timing );
        }

        if( m_affectorable != nullptr )
        {
            m_affectorable->updateAffectors( _time, _timing );
        }

        if( m_affectorableGlobal != nullptr )
        {
            m_affectorableGlobal->updateAffectors( _time, _timing );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    bool Player::update()
    {
        if( m_mousePickerSystem != nullptr )
        {
            m_mousePickerSystem->update();
        }

        if( m_globalHandleSystem != nullptr )
        {
            m_globalHandleSystem->update();
        }

        NODE_SERVICE()
            ->clearHomeless();

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void Player::setRenderCamera( const RenderCameraInterfacePtr & _camera )
    {
        m_renderCamera = _camera;

        if( m_mousePickerSystem != nullptr )
        {
            m_mousePickerSystem->setRenderCamera( m_renderCamera );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    const RenderCameraInterfacePtr & Player::getRenderCamera() const
    {
        return m_renderCamera;
    }
    //////////////////////////////////////////////////////////////////////////
    void Player::setRenderViewport( const RenderViewportInterfacePtr & _viewport )
    {
        m_renderViewport = _viewport;

        if( m_mousePickerSystem != nullptr )
        {
            m_mousePickerSystem->setRenderViewport( m_renderViewport );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    const RenderViewportInterfacePtr & Player::getRenderViewport() const
    {
        return m_renderViewport;
    }
    //////////////////////////////////////////////////////////////////////////
    void Player::setRenderClipplane( const RenderClipplaneInterfacePtr & _clipplane )
    {
        m_renderClipplane = _clipplane;

        if( m_mousePickerSystem != nullptr )
        {
            m_mousePickerSystem->setRenderClipplane( _clipplane );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    const RenderClipplaneInterfacePtr & Player::getRenderClipplane() const
    {
        return m_renderClipplane;
    }
    //////////////////////////////////////////////////////////////////////////
    namespace
    {

    }
    //////////////////////////////////////////////////////////////////////////
    void Player::render()
    {
        //if( this->isChangedScene() == true )
        //{
        //	return;
        //}

        uint32_t debugMask = APPLICATION_SERVICE()
            ->getDebugMask();

        RenderState state;
        state.viewport = m_renderViewport;
        state.camera = m_renderCamera;
        state.clipplane = m_renderClipplane;
        state.target = m_renderTarget;

        RenderServiceInterface * renderService = RENDER_SERVICE();

        if( m_scene != nullptr )
        {
            m_scene->render( renderService, &state, debugMask );
        }

        MODULE_SERVICE()
            ->render( &state, debugMask );

        RENDER_SERVICE()
            ->endLimitRenderObjects();

        if( m_arrow != nullptr )
        {
            m_arrow->render( renderService, &state, debugMask );
        }

        if( m_showDebugText != 0 )
        {
            const RenderServiceDebugInfo & rdi =
                RENDER_SERVICE()->getDebugInfo();

            Stringstream ss;

            ss << "FPS: " << m_fps << std::endl;

            if( m_showDebugText > 1 )
            {
                const Resolution & contentResolution = APPLICATION_SERVICE()
                    ->getContentResolution();

                double sreenfillrate = rdi.fillrate / double( contentResolution.getWidth() * contentResolution.getHeight() );

                ss << "Fillrate " << std::setiosflags( std::ios::fixed ) << std::setprecision( 2 ) << sreenfillrate << " (Object " << rdi.object << " Triangle " << rdi.triangle << ")" << std::endl;
                ss << "DIP: " << rdi.dips << std::endl;

                ERenderBatchMode mode = RENDER_SERVICE()
                    ->getBatchMode();

                ss << "Smart Batch: " << mode << " " << rdi.batch << std::endl;

                uint32_t textureMemoryUse = RENDER_SYSTEM()
                    ->getTextureMemoryUse();

                ss << "Texture Memory Usage: " << (float)textureMemoryUse / (1024.f*1024.f) << std::endl;

                uint32_t textureCount = RENDER_SYSTEM()
                    ->getTextureCount();

                ss << "Texture Count: " << textureCount << std::endl;

                uint32_t particlesCount = PARTICLE_SYSTEM2()
                    ->getEmitterCount();

                ss << "Particles: " << particlesCount << std::endl;
            }

            if( m_showDebugText == 2 )
            {
                class CompileResourceVisitor
                    : public Visitor
                    , public ConcreteVisitor<ResourceReference>
                {
                public:
                    CompileResourceVisitor()
                        : m_count( 0 )
                    {
                    }

                public:
                    uint32_t getCount() const
                    {
                        return m_count;
                    }

                protected:
                    void accept( ResourceReference * _resource )
                    {
                        if( _resource->isCompile() == false )
                        {
                            return;
                        }

                        ++m_count;
                    }

                protected:
                    uint32_t m_count;
                };

                CompileResourceVisitor crv;

                RESOURCE_SERVICE()
                    ->visitResources( &crv );

                ss << "Resources: " << crv.getCount() << std::endl;

                class CompleteThreadTaskVisitor
                    : public Visitor
                    , public ConcreteVisitor<ThreadTask>
                {
                public:
                    CompleteThreadTaskVisitor()
                        : m_count( 0 )
                    {
                    }

                public:
                    uint32_t getCount() const
                    {
                        return m_count;
                    }

                protected:
                    void accept( ThreadTask * _task )
                    {
                        if( _task->isComplete() == false )
                        {
                            return;
                        }

                        ++m_count;
                    }

                protected:
                    uint32_t m_count;
                };

                CompleteThreadTaskVisitor cttv;

                PREFETCHER_SERVICE()
                    ->visitPrefetches( &cttv );

                ss << "Prefetcher " << cttv.getCount() << std::endl;

                const MousePickerSystemInterfacePtr & mousePickerSystem = PLAYER_SERVICE()
                    ->getMousePickerSystem();

                ss << "PickerTrapCount:" << mousePickerSystem->getPickerTrapCount() << std::endl;
            }
            else if( m_showDebugText == 3 )
            {
                //////////////////////////////////////////////////////////////////////////
                class VisitorPlayerFactoryManager
                    : public VisitorPrototypeGenerator
                {
                public:
                    VisitorPlayerFactoryManager( const ConstString & _category )
                        : m_category( _category )
                    {
                    }

                private:
                    VisitorPlayerFactoryManager & operator = ( const VisitorPlayerFactoryManager & _vpfm )
                    {
                        (void)_vpfm;

                        return *this;
                    }

                protected:
                    void visit( const ConstString & _category, const ConstString & _type, const PrototypeGeneratorInterfacePtr & _generator ) override
                    {
                        if( m_category != _category )
                        {
                            return;
                        }

                        uint32_t count = _generator->count();

                        if( count == 0 )
                        {
                            return;
                        }

                        m_scopes[count].emplace_back( _type );
                    }

                public:
                    String getMsg() const
                    {
                        Stringstream ss;

                        uint32_t iterator = 0;

                        for( TMapPybindScope::const_reverse_iterator
                            it = m_scopes.rbegin(),
                            it_end = m_scopes.rend();
                            it != it_end;
                            ++it )
                        {
                            uint32_t c = it->first;
                            const stdex::vector<ConstString> & l = it->second;

                            for( stdex::vector<ConstString>::const_iterator
                                it_list = l.begin(),
                                it_list_end = l.end();
                                it_list != it_list_end;
                                ++it_list )
                            {
                                const ConstString & s = *it_list;

                                ss << "Prototype: " << s.c_str() << " " << c << std::endl;

                                if( ++iterator == 15 )
                                {
                                    break;
                                }
                            }

                            if( iterator == 15 )
                            {
                                break;
                            }
                        }

                        return ss.str();
                    }

                protected:
                    ConstString m_category;
                    typedef stdex::map<uint32_t, stdex::vector<ConstString>> TMapPybindScope;
                    TMapPybindScope m_scopes;
                };


                VisitorPlayerFactoryManager pfmv_node( STRINGIZE_STRING_LOCAL( "Node" ) );

                PROTOTYPE_SERVICE()
                    ->visitGenerators( &pfmv_node );

                ss << pfmv_node.getMsg() << std::endl;

                VisitorPlayerFactoryManager pfmv_surface( STRINGIZE_STRING_LOCAL( "Surface" ) );

                PROTOTYPE_SERVICE()
                    ->visitGenerators( &pfmv_surface );

                ss << pfmv_surface.getMsg() << std::endl;
            }
            else if( m_showDebugText == 4 )
            {
                class MyVisitorPythonClassTypeScope
                    : public pybind::visitor_class_type_scope
                {
                public:
                    MyVisitorPythonClassTypeScope()
                    {
                    }

                protected:
                    void operator = ( const MyVisitorPythonClassTypeScope & )
                    {
                    }

                protected:
                    void visit_scope( pybind::class_type_scope_interface * _scope ) override
                    {
                        uint32_t count = _scope->getObjectCount();

                        if( count == 0 )
                        {
                            return;
                        }

                        String name = _scope->get_name();

                        m_scopes[count].emplace_back( name );
                    }

                public:
                    String getMsg() const
                    {
                        Stringstream ss;

                        uint32_t iterator = 0;

                        for( TMapPybindScope::const_reverse_iterator
                            it = m_scopes.rbegin(),
                            it_end = m_scopes.rend();
                            it != it_end;
                            ++it )
                        {
                            uint32_t c = it->first;
                            const stdex::vector<String> & l = it->second;

                            for( stdex::vector<String>::const_iterator
                                it_list = l.begin(),
                                it_list_end = l.end();
                                it_list != it_list_end;
                                ++it_list )
                            {
                                const String & s = *it_list;

                                ss << "Python: " << s << " " << c << std::endl;

                                if( ++iterator == 15 )
                                {
                                    break;
                                }
                            }

                            if( iterator == 15 )
                            {
                                break;
                            }
                        }

                        return ss.str();
                    }

                protected:
                    typedef stdex::map<uint32_t, stdex::vector<String>> TMapPybindScope;
                    TMapPybindScope m_scopes;
                };

                pybind::kernel_interface * kernel = pybind::get_kernel();

                MyVisitorPythonClassTypeScope mvcts;
                kernel->visit_types_scope( &mvcts );

                String msg_python = mvcts.getMsg();
                ss << msg_python << std::endl;
            }
            else if( m_showDebugText == 5 )
            {
                class MyVisitorFactoryService
                    : public VisitorFactoryService
                {
                public:
                    MyVisitorFactoryService()
                    {
                    }

                protected:
                    void operator = ( const MyVisitorFactoryService & )
                    {
                    }

                protected:
                    void visit( const Factory * _factory )
                    {
                        uint32_t count = _factory->getCountObject();

                        if( count == 0 )
                        {
                            return;
                        }

                        String name = _factory->getName();

                        m_scopes[count].emplace_back( name );
                    }

                public:
                    String getMsg() const
                    {
                        Stringstream ss;

                        uint32_t iterator = 0;

                        for( TMapPybindScope::const_reverse_iterator
                            it = m_scopes.rbegin(),
                            it_end = m_scopes.rend();
                            it != it_end;
                            ++it )
                        {
                            uint32_t c = it->first;
                            const stdex::vector<String> & l = it->second;

                            for( stdex::vector<String>::const_iterator
                                it_list = l.begin(),
                                it_list_end = l.end();
                                it_list != it_list_end;
                                ++it_list )
                            {
                                const String & s = *it_list;

                                ss << "Factory: " << s << " " << c << std::endl;

                                if( ++iterator == 15 )
                                {
                                    break;
                                }
                            }

                            if( iterator == 15 )
                            {
                                break;
                            }
                        }

                        return ss.str();
                    }

                protected:
                    typedef stdex::map<uint32_t, stdex::vector<String>> TMapPybindScope;
                    TMapPybindScope m_scopes;
                };
                
                MyVisitorFactoryService mvcts;
                FACTORY_SERVICE()
                    ->visitFactories( &mvcts );

                String msg_python = mvcts.getMsg();
                ss << msg_python << std::endl;
            }
            else if( m_showDebugText == 6 )
            {
                typedef stdex::map<int32_t, stdex::vector<String>> TMapPybindScope;
                TMapPybindScope scopes;

                size_t count = stdex_allocator_report_count();

                for( size_t i = 0; i != count; ++i )
                {
                    stdex_memory_report_t * report = stdex_allocator_report_info( i );

                    scopes[report->count].emplace_back( report->name );
                }

                Stringstream ss2;

                uint32_t iterator = 0;

                for( TMapPybindScope::const_reverse_iterator
                    it = scopes.rbegin(),
                    it_end = scopes.rend();
                    it != it_end;
                    ++it )
                {
                    int32_t c = it->first;
                    const stdex::vector<String> & l = it->second;

                    for( stdex::vector<String>::const_iterator
                        it_list = l.begin(),
                        it_list_end = l.end();
                        it_list != it_list_end;
                        ++it_list )
                    {
                        const String & s = *it_list;

                        ss2 << "Memory: " << s << " " << c << std::endl;

                        if( iterator++ == 15 )
                        {
                            break;
                        }
                    }

                    if( iterator == 15 )
                    {
                        break;
                    }
                }

                String msg_python = ss2.str();
                ss << msg_python << std::endl;
            }

            String text = ss.str();

            TVectorString args;
            args.emplace_back( text );
            m_debugText->setTextFormatArgs( args );

            float gameViewportAspect;
            Viewport gameViewport;

            APPLICATION_SERVICE()
                ->getGameViewport( gameViewportAspect, gameViewport );

            m_debugText->setLocalPosition( mt::vec3f( gameViewport.begin, 0.f ) );

            const Resolution & resolution = APPLICATION_SERVICE()
                ->getCurrentResolution();

            const Resolution & content = APPLICATION_SERVICE()
                ->getContentResolution();

            mt::vec2f scale;
            content.calcScale( resolution, scale );

            m_debugText->setScale( mt::vec3f( scale, 1.f ) );

            m_debugText->render( renderService, &state, debugMask );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void Player::onAppMouseLeave( const InputMousePositionEvent & _event )
    {
        if( m_arrow != nullptr )
        {
            m_arrow->onAppMouseLeave();
        }

        if( m_scene != nullptr && m_scene->isActivate() == true )
        {
            m_scene->onAppMouseLeave();
        }

        if( m_mousePickerSystem != nullptr )
        {
            m_mousePickerSystem->handleMouseLeave( _event );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void Player::onAppMouseEnter( const InputMousePositionEvent & _event )
    {
        if( m_arrow != nullptr )
        {
            m_arrow->onAppMouseEnter();
        }

        if( m_scene != nullptr && m_scene->isActivate() == true )
        {
            m_scene->onAppMouseEnter();
        }

        if( m_mousePickerSystem != nullptr )
        {
            m_mousePickerSystem->handleMouseEnter( _event );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void Player::onAppMousePosition( const InputMousePositionEvent & _event )
    {
        (void)_event;
        //if( m_mousePickerSystem != nullptr )
        //{
        //	m_mousePickerSystem->handleMousePosition( _event );
        //}
    }
    //////////////////////////////////////////////////////////////////////////
    void Player::onFullscreen( const Resolution & _resolution, bool _fullscreen )
    {
        (void)_resolution;
        (void)_fullscreen;
    }
    //////////////////////////////////////////////////////////////////////////
    void Player::onFixedContentResolution( const Resolution & _resolution, bool _fixed )
    {
        (void)_resolution;
        (void)_fixed;
    }
    //////////////////////////////////////////////////////////////////////////
    void Player::onFixedDisplayResolution( const Resolution & _resolution, bool _fixed )
    {
        (void)_resolution;
        (void)_fixed;
    }
    //////////////////////////////////////////////////////////////////////////
    void Player::onFocus( bool _focus )
    {
        m_focus = _focus;

        if( m_scene != nullptr && m_scene->isActivate() == true )
        {
            m_scene->onFocus( _focus );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void Player::toggleDebugText()
    {
        ++m_showDebugText;
        m_showDebugText %= 7;
    }
    //////////////////////////////////////////////////////////////////////////
}
