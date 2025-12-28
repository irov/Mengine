#include "GameService.h"

#include "Interface/SoundServiceInterface.h"
#include "Interface/OptionsServiceInterface.h"
#include "Interface/WatchdogServiceInterface.h"
#include "Interface/ArchivatorInterface.h"
#include "Interface/StatisticInterface.h"
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

#include "Kernel/Logger.h"
#include "Kernel/StreamHelper.h"
#include "Kernel/AssertionMemoryPanic.h"
#include "Kernel/ConstStringHelper.h"
#include "Kernel/EventableHelper.h"
#include "Kernel/FactorableUnique.h"
#include "Kernel/ConfigHelper.h"
#include "Kernel/NotificationHelper.h"
#include "Kernel/StatisticHelper.h"
#include "Kernel/BuildMode.h"
#include "Kernel/PrototypeHelper.h"
#include "Kernel/Interender.h"

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

        NOTIFICATION_ADDOBSERVERMETHOD_THIS( NOTIFICATOR_TIME_FACTOR_CHANGE, &GameService::notifyTimeFactorChange_, MENGINE_DOCUMENT_FACTORABLE );
        NOTIFICATION_ADDOBSERVERMETHOD_THIS( NOTIFICATOR_SETTING_CHANGE, &GameService::notifySettingChange_, MENGINE_DOCUMENT_FACTORABLE );

        NOTIFICATION_ADDOBSERVERMETHOD_THIS( NOTIFICATOR_APPLICATION_DID_BECOME_ACTIVE, &GameService::notifyApplicationDidBecomeActive_, MENGINE_DOCUMENT_FACTORABLE );
        NOTIFICATION_ADDOBSERVERMETHOD_THIS( NOTIFICATOR_APPLICATION_WILL_ENTER_FOREGROUND, &GameService::notifyApplicationWillEnterForeground_, MENGINE_DOCUMENT_FACTORABLE );
        NOTIFICATION_ADDOBSERVERMETHOD_THIS( NOTIFICATOR_APPLICATION_DID_ENTER_BACKGROUND, &GameService::notifyApplicationDidEnterBackground_, MENGINE_DOCUMENT_FACTORABLE );
        NOTIFICATION_ADDOBSERVERMETHOD_THIS( NOTIFICATOR_APPLICATION_WILL_RESIGN_ACTIVE, &GameService::notifyApplicationWillResignActive_, MENGINE_DOCUMENT_FACTORABLE );
        NOTIFICATION_ADDOBSERVERMETHOD_THIS( NOTIFICATOR_APPLICATION_WILL_TERMINATE, &GameService::notifyApplicationWillTerminate_, MENGINE_DOCUMENT_FACTORABLE );

        ANALYTICS_SERVICE()
            ->addEventProvider( AnalyticsEventProviderInterfacePtr::from( this ) );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void GameService::_finalizeService()
    {
        NOTIFICATION_REMOVEOBSERVER_THIS( NOTIFICATOR_TIME_FACTOR_CHANGE );
        NOTIFICATION_REMOVEOBSERVER_THIS( NOTIFICATOR_SETTING_CHANGE );

        NOTIFICATION_REMOVEOBSERVER_THIS( NOTIFICATOR_APPLICATION_DID_BECOME_ACTIVE );
        NOTIFICATION_REMOVEOBSERVER_THIS( NOTIFICATOR_APPLICATION_WILL_ENTER_FOREGROUND );
        NOTIFICATION_REMOVEOBSERVER_THIS( NOTIFICATOR_APPLICATION_DID_ENTER_BACKGROUND );
        NOTIFICATION_REMOVEOBSERVER_THIS( NOTIFICATOR_APPLICATION_WILL_RESIGN_ACTIVE );
        NOTIFICATION_REMOVEOBSERVER_THIS( NOTIFICATOR_APPLICATION_WILL_TERMINATE );

        ANALYTICS_SERVICE()
            ->removeEventProvider( AnalyticsEventProviderInterfacePtr::from( this ) );

        this->removeEvents();

        m_userEventsAdd.clear();
        m_userEvents.clear();

        m_iconPath.clear();
        m_currentPackName.clear();
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
    bool GameService::handleAccelerometerEvent( const InputAccelerometerEvent & _event )
    {
        bool handle = false;

        if( handle == false )
        {
            handle = EVENTABLE_METHODR( EVENT_GAME_ACCELEROMETER, handle )
                ->onGameAccelerometer( _event );
        }

        if( handle == false )
        {
            handle = PLAYER_SERVICE()
                ->handleAccelerometerEvent( _event );
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
    void GameService::_update()
    {
#if defined(MENGINE_DEBUG)
        double Limit_Fillrate = CONFIG_VALUE_FLOAT( "Limit", "Fillrate", 100.0 );

        const Resolution & contentResolution = APPLICATION_SERVICE()
            ->getContentResolution();

        double fillrate = STATISTIC_GET_DOUBLE( STATISTIC_RENDER_PERFRAME_FILLRATE );

        double sreenFillrate = fillrate / double( contentResolution.getWidth() * contentResolution.getHeight() );

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
        if( SERVICE_IS_INITIALIZE( ScriptServiceInterface ) == true )
        {
            if( SCRIPT_SERVICE()
                ->initializeModules() == false )
            {
                return false;
            }
        }

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
            LOGGER_INFO( "game", "create global account" );

            EVENTABLE_METHOD( EVENT_GAME_CREATE_GLOBAL_ACCOUNT )
                ->onGameCreateGlobalAccount();
        }

        bool hasCurrentAccount = ACCOUNT_SERVICE()
            ->hasCurrentAccount();

        if( hasCurrentAccount == false )
        {
            LOGGER_INFO( "game", "create default account" );

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
        EVENTABLE_METHOD( EVENT_GAME_ACCOUNT_FINALIZE )
            ->onGameAccountFinalize();

        this->finalizeRenderResources();

        if( SERVICE_IS_INITIALIZE( ScriptServiceInterface ) == true )
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
    bool GameService::preparation()
    {
        bool is_debug = Helper::isDebugMode();

        bool result = EVENTABLE_METHODR( EVENT_GAME_PREPARATION, true )
            ->onGamePreparation( is_debug );

        if( result == false )
        {
            return false;
        }

        LOGGER_INFO( "game", "preparation [%s]"
            , is_debug == true ? "debug" : "release"
        );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void GameService::run()
    {
        LOGGER_INFO( "game", "run" );

        EVENTABLE_METHOD( EVENT_GAME_RUN )
            ->onGameRun();
    }
    //////////////////////////////////////////////////////////////////////////
    void GameService::interruption()
    {
        LOGGER_INFO( "game", "interruption" );

        EVENTABLE_METHOD( EVENT_GAME_INTERRUPTION )
            ->onGameInterruption();
    }
    //////////////////////////////////////////////////////////////////////////
    void GameService::stop()
    {
        LOGGER_INFO( "game", "stop" );

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
        if( SERVICE_IS_INITIALIZE( PlayerServiceInterface ) == true )
        {
            PLAYER_SERVICE()
                ->onFocus( _focus );
        }

        EVENTABLE_METHOD( EVENT_GAME_FOCUS )
            ->onGameFocus( _focus );
    }
    //////////////////////////////////////////////////////////////////////////
    void GameService::setFullscreen( bool _fullscreen )
    {
        EVENTABLE_METHOD( EVENT_GAME_FULLSCREEN )
            ->onGameFullscreen( _fullscreen );
    }
    //////////////////////////////////////////////////////////////////////////
    void GameService::setFixedContentResolution( bool _fixed )
    {
        EVENTABLE_METHOD( EVENT_GAME_FIXED_CONTENT_RESOLUTION )
            ->onGameFixedContentResolution( _fixed );
    }
    //////////////////////////////////////////////////////////////////////////
    void GameService::setFixedDisplayResolution( bool _fixed )
    {
        EVENTABLE_METHOD( EVENT_GAME_FIXED_DISPLAY_RESOLUTION )
            ->onGameFixedDisplayResolution( _fixed );
    }
    //////////////////////////////////////////////////////////////////////////
    void GameService::setFixedViewportResolution( bool _fixed )
    {
        EVENTABLE_METHOD( EVENT_GAME_FIXED_VIEWPORT_RESOLUTION )
            ->onGameFixedViewportResolution( _fixed );
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
    void GameService::userEvent( const ConstString & _id, const Params & _params )
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
    void GameService::notifyTimeFactorChange_( float _timeFactor )
    {
        EVENTABLE_METHOD( EVENT_GAME_TIME_FACTOR_CHANGE )
            ->onGameTimeFactorChange( _timeFactor );
    }
    //////////////////////////////////////////////////////////////////////////
    void GameService::notifySettingChange_( const SettingInterfacePtr & _setting, const Char * _key )
    {
        EVENTABLE_METHOD( EVENT_GAME_SETTING_CHANGE )
            ->onGameSettingChange( _setting, _key );
    }
    //////////////////////////////////////////////////////////////////////////
    void GameService::notifyApplicationDidBecomeActive_()
    {
        EVENTABLE_METHOD( EVENT_GAME_APPLICATION_DID_BECOME_ACTIVE )
            ->onGameApplicationDidBecomeActive();
    }
    void GameService::notifyApplicationWillEnterForeground_()
    {
        EVENTABLE_METHOD( EVENT_GAME_APPLICATION_WILL_ENTER_FOREGROUND )
            ->onGameApplicationWillEnterForeground();
    }
    //////////////////////////////////////////////////////////////////////////
    void GameService::notifyApplicationDidEnterBackground_()
    {
        EVENTABLE_METHOD( EVENT_GAME_APPLICATION_DID_ENTER_BACKGROUD )
            ->onGameApplicationDidEnterBackground();
    }
    //////////////////////////////////////////////////////////////////////////
    void GameService::notifyApplicationWillResignActive_()
    {
        EVENTABLE_METHOD( EVENT_GAME_APPLICATION_WILL_RESIGN_ACTIVE )
            ->onGameApplicationWillResignActive();
    }
    //////////////////////////////////////////////////////////////////////////
    void GameService::notifyApplicationWillTerminate_()
    {
        EVENTABLE_METHOD( EVENT_GAME_APPLICATION_WILL_TERMINATE )
            ->onGameApplicationWillTerminate();
    }
    //////////////////////////////////////////////////////////////////////////
    void GameService::onAnalyticsEvent( const AnalyticsEventInterfacePtr & _event )
    {
        EVENTABLE_METHOD( EVENT_GAME_ANALYTICS_EVENT )
            ->onGameAnalyticsEvent( _event );
    }
    //////////////////////////////////////////////////////////////////////////
    void GameService::onAnalyticsScreenView( const ConstString & _screenType, const ConstString & _screenName )
    {
        EVENTABLE_METHOD( EVENT_GAME_ANALYTICS_SCREENVIEW )
            ->onGameAnalyticsScreenView( _screenType, _screenName );
    }
    //////////////////////////////////////////////////////////////////////////
    void GameService::onAnalyticsFlush()
    {
        EVENTABLE_METHOD( EVENT_GAME_ANALYTICS_FLUSH )
            ->onGameAnalyticsFlush();
    }
    //////////////////////////////////////////////////////////////////////////
}
