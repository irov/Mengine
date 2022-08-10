#include "GameService.h"

#include "Interface/PrototypeServiceInterface.h"
#include "Interface/SoundServiceInterface.h"
#include "Interface/OptionsServiceInterface.h"
#include "Interface/WatchdogServiceInterface.h"
#include "Interface/ArchivatorInterface.h"
#include "Interface/StatisticServiceInterface.h"

#include "Interface/ScriptServiceInterface.h"
#include "Interface/ResourceServiceInterface.h"
#include "Interface/RenderServiceInterface.h"
#include "Interface/AccountServiceInterface.h"
#include "Interface/TextServiceInterface.h"
#include "Interface/PlayerServiceInterface.h"
#include "Interface/UpdateServiceInterface.h"
#include "Interface/GameEventReceiverInterface.h"

#include "GameServiceAccountProvider.h"
#include "GameServiceSoundVolumeProvider.h"

#include "Kernel/Arrow.h"
#include "Kernel/Logger.h"
#include "Kernel/DocumentHelper.h"
#include "Kernel/Stream.h"
#include "Kernel/AssertionMemoryPanic.h"
#include "Kernel/ConstStringHelper.h"
#include "Kernel/EventableHelper.h"
#include "Kernel/FactorableUnique.h"
#include "Kernel/ConfigHelper.h"
#include "Kernel/NotificationHelper.h"

//////////////////////////////////////////////////////////////////////////
SERVICE_FACTORY( GameService, Mengine::GameService );
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    GameService::GameService()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    GameService::~GameService()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool GameService::handleKeyEvent( const InputKeyEvent & _event )
    {
        bool handle = false;

        if( handle == false )
        {
            handle = EVENTABLE_METHODR( EVENT_GAME_KEY, handle )
                ->onGameKey( _event );
        }

        if( handle == false )
        {
            handle = PLAYER_SERVICE()
                ->handleKeyEvent( _event );
        }

        return handle;
    }
    //////////////////////////////////////////////////////////////////////////
    bool GameService::handleTextEvent( const InputTextEvent & _event )
    {
        bool handle = false;

        if( handle == false )
        {
            handle = EVENTABLE_METHODR( EVENT_GAME_TEXT, handle )
                ->onGameText( _event );
        }

        if( handle == false )
        {
            handle = PLAYER_SERVICE()
                ->handleTextEvent( _event );
        }

        return handle;
    }
    //////////////////////////////////////////////////////////////////////////
    bool GameService::handleMouseButtonEvent( const InputMouseButtonEvent & _event )
    {
        bool handle = false;

        if( handle == false )
        {
            handle = EVENTABLE_METHODR( EVENT_GAME_MOUSE_BUTTON, handle )
                ->onGameMouseButton( _event );
        }

        if( handle == false )
        {
            handle = PLAYER_SERVICE()
                ->handleMouseButtonEvent( _event );
        }

        return handle;
    }
    //////////////////////////////////////////////////////////////////////////
    bool GameService::handleMouseButtonEventBegin( const InputMouseButtonEvent & _event )
    {
        bool handle = false;

        if( handle == false )
        {
            handle = EVENTABLE_METHODR( EVENT_GAME_MOUSE_BUTTON_BEGIN, handle )
                ->onGameMouseButtonBegin( _event );
        }

        if( handle == false )
        {
            handle = PLAYER_SERVICE()
                ->handleMouseButtonEventBegin( _event );
        }

        return handle;
    }
    //////////////////////////////////////////////////////////////////////////
    bool GameService::handleMouseButtonEventEnd( const InputMouseButtonEvent & _event )
    {
        bool handle = false;

        if( handle == false )
        {
            handle = EVENTABLE_METHODR( EVENT_GAME_MOUSE_BUTTON_END, handle )
                ->onGameMouseButtonEnd( _event );
        }

        if( handle == false )
        {
            handle = PLAYER_SERVICE()
                ->handleMouseButtonEventEnd( _event );
        }

        return handle;
    }
    //////////////////////////////////////////////////////////////////////////
    bool GameService::handleMouseMove( const InputMouseMoveEvent & _event )
    {
        bool handle = false;

        if( handle == false )
        {
            handle = EVENTABLE_METHODR( EVENT_GAME_MOUSE_MOVE, handle )
                ->onGameMouseMove( _event );
        }

        if( handle == false )
        {
            handle = PLAYER_SERVICE()
                ->handleMouseMove( _event );
        }

        return handle;
    }
    //////////////////////////////////////////////////////////////////////////
    bool GameService::handleMouseWheel( const InputMouseWheelEvent & _event )
    {
        bool handle = false;

        if( handle == false )
        {
            handle = EVENTABLE_METHODR( EVENT_GAME_MOUSE_WHEEL, handle )
                ->onGameMouseWheel( _event );
        }

        if( handle == false )
        {
            handle = PLAYER_SERVICE()
                ->handleMouseWheel( _event );
        }

        return handle;
    }
    //////////////////////////////////////////////////////////////////////////
    bool GameService::handleMouseEnter( const InputMouseEnterEvent & _event )
    {
        EVENTABLE_METHOD( EVENT_GAME_APP_MOUSE_ENTER )
            ->onGameAppMouseEnter( _event );

        PLAYER_SERVICE()
            ->handleMouseEnter( _event );

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    void GameService::handleMouseLeave( const InputMouseLeaveEvent & _event )
    {
        EVENTABLE_METHOD( EVENT_GAME_APP_MOUSE_LEAVE )
            ->onGameAppMouseLeave( _event );

        PLAYER_SERVICE()
            ->handleMouseLeave( _event );
    }
    //////////////////////////////////////////////////////////////////////////
    void GameService::update()
    {
#ifdef MENGINE_DEBUG
        double Limit_Fillrate = CONFIG_VALUE( "Limit", "Fillrate", 100.0 );

        const Resolution & contentResolution = APPLICATION_SERVICE()
            ->getContentResolution();

        double fillrate = STATISTIC_SERVICE()
            ->getStatisticDouble( STRINGIZE_STRING_LOCAL( "fillrate" ) );

        double sreenFillrate = fillrate / double( (uint64_t)contentResolution.getWidth() * (uint64_t)contentResolution.getHeight() );

        if( sreenFillrate > Limit_Fillrate )
        {
            EVENTABLE_METHOD( EVENT_GAME_OVER_FILLRATE )
                ->onGameOverFillrate( sreenFillrate, Limit_Fillrate );
        }
#endif

        m_userEvents.insert( m_userEvents.end(), m_userEventsAdd.begin(), m_userEventsAdd.end() );
        m_userEventsAdd.clear();

        for( const UserEvent & ev : m_userEvents )
        {
            EVENTABLE_METHOD( EVENT_GAME_USER )
                ->onGameUser( ev.id, ev.params );
        }

        m_userEvents.clear();
    }
    //////////////////////////////////////////////////////////////////////////
    void GameService::render( const RenderPipelineInterfacePtr & _renderPipeline )
    {
        PLAYER_SERVICE()
            ->render( _renderPipeline );

        EVENTABLE_METHOD( EVENT_GAME_FRAME_END )
            ->onGameFrameEnd();
    }
    //////////////////////////////////////////////////////////////////////////
    bool GameService::loadPersonality()
    {
#ifdef MENGINE_DEBUG
        bool is_debug = true;
#else
        bool is_debug = false;
#endif

        LOGGER_MESSAGE( "personality preparation [%s]"
            , is_debug == true ? "debug" : "release"
        );

        bool result = EVENTABLE_METHODR( EVENT_GAME_PREPARATION, true )
            ->onGamePreparation( is_debug );

        if( result == false )
        {
            return false;
        }

        if( SERVICE_EXIST( ScriptServiceInterface ) == true )
        {
            if( SCRIPT_SERVICE()
                ->initializeModules() == false )
            {
                return false;
            }
        }

        ConstString DefaultArrow_Prototype = CONFIG_VALUE( "DefaultArrow", "Prototype", ConstString::none() );

        ArrowPtr defaultArrow = PROTOTYPE_SERVICE()
            ->generatePrototype( STRINGIZE_STRING_LOCAL( "Arrow" ), DefaultArrow_Prototype, MENGINE_DOCUMENT_FACTORABLE );

        MENGINE_ASSERTION_MEMORY_PANIC( defaultArrow, "failed create defaultArrow 'Default'" );

        ConstString DefaultArrow_Name = CONFIG_VALUE( "DefaultArrow", "Name", STRINGIZE_STRING_LOCAL( "Default" ) );

        defaultArrow->setName( DefaultArrow_Name );

        PLAYER_SERVICE()
            ->setArrow( defaultArrow );

        bool EVENT_INITIALIZE_result = EVENTABLE_METHODR( EVENT_GAME_INITIALIZE, true )
            ->onGameInitialize();

        if( EVENT_INITIALIZE_result == false )
        {
            return false;
        }

        if( ACCOUNT_SERVICE()
            ->loadAccounts() == false )
        {
            LOGGER_ERROR( "failed load accounts" );
        }

        bool hasGlobalAccount = ACCOUNT_SERVICE()
            ->hasGlobalAccount();

        if( hasGlobalAccount == false )
        {
            LOGGER_INFO( "game", "game create global account" );

            EVENTABLE_METHOD( EVENT_GAME_CREATE_GLOBAL_ACCOUNT )
                ->onGameCreateGlobalAccount();
        }

        bool hasCurrentAccount = ACCOUNT_SERVICE()
            ->hasCurrentAccount();

        if( hasCurrentAccount == false )
        {
            LOGGER_INFO( "game", "game create default account" );

            EVENTABLE_METHOD( EVENT_GAME_CREATE_DEFAULT_ACCOUNT )
                ->onGameCreateDefaultAccount();
        }

        LOGGER_INFO( "game", "game load accounts" );

        EVENTABLE_METHOD( EVENT_GAME_LOAD_ACCOUNTS )
            ->onGameLoadAccounts();

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void GameService::removePersonality()
    {
        PLAYER_SERVICE()
            ->setArrow( nullptr );

        EVENTABLE_METHOD( EVENT_GAME_ACCOUNT_FINALIZE )
            ->onGameAccountFinalize();

        this->finalizeRenderResources();

        if( SERVICE_EXIST( ScriptServiceInterface ) == true )
        {
            SCRIPT_SERVICE()
                ->finalizeModules();
        }

        EVENTABLE_METHOD( EVENT_GAME_FINALIZE )
            ->onGameFinalize();

        EVENTABLE_METHOD( EVENT_GAME_DESTROY )
            ->onGameDestroy();
    }
    //////////////////////////////////////////////////////////////////////////
    bool GameService::_initializeService()
    {
        GameAccountProviderPtr accountProvider = Helper::makeFactorableUnique<GameServiceAccountProvider>( MENGINE_DOCUMENT_FACTORABLE );
        accountProvider->setEventable( EventablePtr( this ) );

        ACCOUNT_SERVICE()
            ->setAccountProvider( accountProvider );

        GameSoundVolumeProviderPtr soundVolumeProvider = Helper::makeFactorableUnique<GameServiceSoundVolumeProvider>( MENGINE_DOCUMENT_FACTORABLE );
        soundVolumeProvider->setEventable( EventablePtr( this ) );

        SOUND_SERVICE()
            ->addSoundVolumeProvider( soundVolumeProvider );

        NOTIFICATION_ADDOBSERVERMETHOD_THIS( NOTIFICATOR_TIME_FACTOR_CHANGE, &GameService::onTimeFactorChange_, MENGINE_DOCUMENT_FACTORABLE );

#ifdef MENGINE_PLATFORM_IOS
        NOTIFICATION_ADDOBSERVERMETHOD_THIS( NOTIFICATOR_IOS_APPLICATION_DID_BECOME_ACTIVE, &GameService::oniOSApplicationDidBecomeActive_, MENGINE_DOCUMENT_FACTORABLE );
        NOTIFICATION_ADDOBSERVERMETHOD_THIS( NOTIFICATOR_IOS_APPLICATION_WILL_ENTER_FOREGROUND, &GameService::oniOSApplicationWillEnterForeground_, MENGINE_DOCUMENT_FACTORABLE );
        NOTIFICATION_ADDOBSERVERMETHOD_THIS( NOTIFICATOR_IOS_APPLICATION_DID_ENTER_BACKGROUD, &GameService::oniOSApplicationDidEnterBackground_, MENGINE_DOCUMENT_FACTORABLE );
        NOTIFICATION_ADDOBSERVERMETHOD_THIS( NOTIFICATOR_IOS_APPLICATION_WILL_RESIGN_ACTIVE, &GameService::oniOSApplicationWillResignActive_, MENGINE_DOCUMENT_FACTORABLE );
        NOTIFICATION_ADDOBSERVERMETHOD_THIS( NOTIFICATOR_IOS_APPLICATION_WILL_TERMINATE, &GameService::oniOSApplicationWillTerminate_, MENGINE_DOCUMENT_FACTORABLE );
#endif

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void GameService::_finalizeService()
    {
        NOTIFICATION_REMOVEOBSERVER_THIS( NOTIFICATOR_TIME_FACTOR_CHANGE );

#ifdef MENGINE_PLATFORM_IOS
        NOTIFICATION_REMOVEOBSERVER_THIS( NOTIFICATOR_IOS_APPLICATION_DID_BECOME_ACTIVE );
        NOTIFICATION_REMOVEOBSERVER_THIS( NOTIFICATOR_IOS_APPLICATION_WILL_ENTER_FOREGROUND );
        NOTIFICATION_REMOVEOBSERVER_THIS( NOTIFICATOR_IOS_APPLICATION_DID_ENTER_BACKGROUD );
        NOTIFICATION_REMOVEOBSERVER_THIS( NOTIFICATOR_IOS_APPLICATION_WILL_RESIGN_ACTIVE );
        NOTIFICATION_REMOVEOBSERVER_THIS( NOTIFICATOR_IOS_APPLICATION_WILL_TERMINATE );
#endif

        this->removeEvents();

        m_userEventsAdd.clear();
        m_userEvents.clear();

        m_iconPath.clear();
        m_currentPackName.clear();
    }
    //////////////////////////////////////////////////////////////////////////
    void GameService::run()
    {
        LOGGER_MESSAGE( "Run game" );

        EVENTABLE_METHOD( EVENT_GAME_RUN )
            ->onGameRun();
    }
    //////////////////////////////////////////////////////////////////////////
    void GameService::interruption()
    {
        LOGGER_MESSAGE( "Interruption game" );

        EVENTABLE_METHOD( EVENT_GAME_INTERRUPTION )
            ->onGameInterruption();
    }
    //////////////////////////////////////////////////////////////////////////
    void GameService::stop()
    {
        LOGGER_MESSAGE( "Stop game" );

        EVENTABLE_METHOD( EVENT_GAME_STOP )
            ->onGameStop();
    }
    //////////////////////////////////////////////////////////////////////////
    void GameService::initializeRenderResources()
    {
        EVENTABLE_METHOD( EVENT_GAME_INITIALIZE_RENDER_RESOURCES )
            ->onGameInitializeRenderResources();
    }
    //////////////////////////////////////////////////////////////////////////
    void GameService::finalizeRenderResources()
    {
        EVENTABLE_METHOD( EVENT_GAME_FINALIZE_RENDER_RESOURCES )
            ->onGameFinalizeRenderResources();
    }
    //////////////////////////////////////////////////////////////////////////
    void GameService::turnSound( bool _turn )
    {
        EVENTABLE_METHOD( EVENT_GAME_TURN_SOUND )
            ->onGameTurnSound( _turn );
    }
    //////////////////////////////////////////////////////////////////////////
    void GameService::setFocus( bool _focus )
    {
        if( SERVICE_EXIST( PlayerServiceInterface ) == true )
        {
            PLAYER_SERVICE()
                ->onFocus( _focus );
        }

        EVENTABLE_METHOD( EVENT_GAME_FOCUS )
            ->onGameFocus( _focus );
    }
    //////////////////////////////////////////////////////////////////////////
    void GameService::setFullscreen( const Resolution & _resolution, bool _fullscreen )
    {
        if( SERVICE_EXIST( PlayerServiceInterface ) == true )
        {
            PLAYER_SERVICE()
                ->onFullscreen( _resolution, _fullscreen );
        }

        EVENTABLE_METHOD( EVENT_GAME_FULLSCREEN )
            ->onGameFullscreen( _fullscreen );
    }
    //////////////////////////////////////////////////////////////////////////
    void GameService::setFixedContentResolution( const Resolution & _resolution, bool _fixed )
    {
        if( SERVICE_EXIST( PlayerServiceInterface ) == true )
        {
            PLAYER_SERVICE()
                ->onFixedContentResolution( _resolution, _fixed );
        }

        EVENTABLE_METHOD( EVENT_GAME_FIXED_CONTENT_RESOLUTION )
            ->onGameFixedContentResolution( _fixed );
    }
    //////////////////////////////////////////////////////////////////////////
    void GameService::setFixedDisplayResolution( const Resolution & _resolution, bool _fixed )
    {
        if( SERVICE_EXIST( PlayerServiceInterface ) == true )
        {
            PLAYER_SERVICE()
                ->onFixedContentResolution( _resolution, _fixed );
        }

        EVENTABLE_METHOD( EVENT_GAME_FIXED_DISPLAY_RESOLUTION )
            ->onGameFixedDisplayResolution( _fixed );
    }
    //////////////////////////////////////////////////////////////////////////
    void GameService::setRenderViewport( const Viewport & _viewport, const Resolution & _contentResolution )
    {
        EVENTABLE_METHOD( EVENT_GAME_RENDER_VIEWPORT )
            ->onGameRenderViewport( _viewport, _contentResolution );
    }
    //////////////////////////////////////////////////////////////////////////
    void GameService::setGameViewport( const Viewport & _viewport, float _aspect )
    {
        EVENTABLE_METHOD( EVENT_GAME_VIEWPORT )
            ->onGameViewport( _viewport, _aspect );
    }
    //////////////////////////////////////////////////////////////////////////
    bool GameService::close()
    {
        bool needQuit = EVENTABLE_METHODR( EVENT_GAME_CLOSE, true )
            ->onGameClose();

        return needQuit;
    }
    //////////////////////////////////////////////////////////////////////////
    void GameService::userEvent( const ConstString & _id, const MapWParams & _params )
    {
        UserEvent ev;
        ev.id = _id;
        ev.params = _params;

        m_userEvents.emplace_back( ev );
    }
    //////////////////////////////////////////////////////////////////////////
    void GameService::setCursorMode( bool _mode )
    {
        EVENTABLE_METHOD( EVENT_GAME_CURSOR_MODE )
            ->onGameCursorMode( _mode );
    }
    //////////////////////////////////////////////////////////////////////////
    void GameService::onTimeFactorChange_( float _timeFactor )
    {
        EVENTABLE_METHOD( EVENT_GAME_ON_TIME_FACTOR )
            ->onGameTimeFactor( _timeFactor );
    }
    //////////////////////////////////////////////////////////////////////////
#ifdef MENGINE_PLATFORM_IOS
    //////////////////////////////////////////////////////////////////////////
    void GameService::oniOSApplicationDidBecomeActive_()
    {
        EVENTABLE_METHOD( EVENT_GAME_IOS_APPLICATION_DID_BECOME_ACTIVE )
            ->onGameiOSApplicationDidBecomeActive();
    }
    void GameService::oniOSApplicationWillEnterForeground_()
    {
        EVENTABLE_METHOD( EVENT_GAME_IOS_APPLICATION_WILL_ENTER_FOREGROUND )
            ->onGameiOSApplicationWillEnterForeground();
    }
    //////////////////////////////////////////////////////////////////////////
    void GameService::oniOSApplicationDidEnterBackground_()
    {
        EVENTABLE_METHOD( EVENT_GAME_IOS_APPLICATION_DID_ENTER_BACKGROUD )
            ->onGameiOSApplicationDidEnterBackground();
    }
    //////////////////////////////////////////////////////////////////////////
    void GameService::oniOSApplicationWillResignActive_()
    {
        EVENTABLE_METHOD( EVENT_GAME_IOS_APPLICATION_WILL_RESIGN_ACTIVE )
            ->onGameiOSApplicationWillResignActive();
    }
    //////////////////////////////////////////////////////////////////////////
    void GameService::oniOSApplicationWillTerminate_()
    {
        EVENTABLE_METHOD( EVENT_GAME_IOS_APPLICATION_WILL_TERMINATE )
            ->onGameiOSApplicationWillTerminate();
    }
    //////////////////////////////////////////////////////////////////////////
#endif
    //////////////////////////////////////////////////////////////////////////
}
