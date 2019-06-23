#include "GameService.h"

#include "Interface/PrototypeServiceInterface.h"
#include "Interface/AmplifierInterface.h"
#include "Interface/SoundServiceInterface.h"
#include "Interface/OptionsServiceInterface.h"
#include "Interface/WatchdogInterface.h"
#include "Interface/ArchivatorInterface.h"

#include "Interface/ScriptServiceInterface.h"
#include "Interface/ResourceServiceInterface.h"
#include "Interface/RenderServiceInterface.h"
#include "Interface/AccountServiceInterface.h"
#include "Interface/TextServiceInterface.h"
#include "Interface/StringizeServiceInterface.h"
#include "Interface/ConfigServiceInterface.h"
#include "Interface/PlayerServiceInterface.h"

#include "GameServiceAccountProvider.h"
#include "GameServiceSoundVolumeProvider.h"

#include "Kernel/Arrow.h"

#include "Kernel/Logger.h"
#include "Kernel/Document.h"

#include "Kernel/String.h"
#include "Kernel/Stream.h"

//////////////////////////////////////////////////////////////////////////
SERVICE_FACTORY( GameService, Mengine::GameService );
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{

    //////////////////////////////////////////////////////////////////////////
    GameService::GameService()
        : m_timingFactor( 1.f )
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
            mt::vec2f point( _event.x, _event.y );

            mt::vec2f wp;
            PLAYER_SERVICE()
                ->calcGlobalMouseWorldPosition( point, &wp );

            handle = EVENTABLE_METHODR( EVENT_GAME_KEY, handle )
                ->onGameKey( _event.code, wp.x, wp.y, _event.isDown, _event.isRepeat );
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
            mt::vec2f point( _event.x, _event.y );

            mt::vec2f wp;
            PLAYER_SERVICE()
                ->calcGlobalMouseWorldPosition( point, &wp );

            handle = EVENTABLE_METHODR( EVENT_GAME_TEXT, handle )
                ->onGameText( _event.key, wp.x, wp.y );
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
            mt::vec2f point( _event.x, _event.y );

            mt::vec2f wp;
            PLAYER_SERVICE()
                ->calcGlobalMouseWorldPosition( point, &wp );

            handle = EVENTABLE_METHODR( EVENT_GAME_MOUSE_BUTTON, handle )
                ->onGameMouseButton( _event.touchId, wp.x, wp.y, _event.button, _event.isDown );
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
            mt::vec2f point( _event.x, _event.y );

            mt::vec2f wp;
            PLAYER_SERVICE()
                ->calcGlobalMouseWorldPosition( point, &wp );

            handle = EVENTABLE_METHODR( EVENT_GAME_MOUSE_BUTTON_BEGIN, handle )
                ->onGameMouseButtonBegin( _event.touchId, wp.x, wp.y, _event.button, _event.isDown );
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
            mt::vec2f point( _event.x, _event.y );

            mt::vec2f wp;
            PLAYER_SERVICE()
                ->calcGlobalMouseWorldPosition( point, &wp );

            handle = EVENTABLE_METHODR( EVENT_GAME_MOUSE_BUTTON_END, handle )
                ->onGameMouseButtonEnd( _event.touchId, wp.x, wp.y, _event.button, _event.isDown );
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
            mt::vec2f point( _event.x, _event.y );
            mt::vec2f delta( _event.dx, _event.dy );

            mt::vec2f wp;
            PLAYER_SERVICE()
                ->calcGlobalMouseWorldPosition( point, &wp );

            mt::vec2f wd;
            PLAYER_SERVICE()
                ->calcGlobalMouseWorldDelta( delta, &wd );

            handle = EVENTABLE_METHODR( EVENT_GAME_MOUSE_MOVE, handle )
                ->onGameMouseMove( _event.touchId, wp.x, wp.y, wd.x, wd.y );
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
                ->onGameMouseWheel( _event.button, _event.x, _event.y, _event.wheel );
        }

        if( handle == false )
        {
            handle = PLAYER_SERVICE()
                ->handleMouseWheel( _event );
        }

        return handle;
    }
    //////////////////////////////////////////////////////////////////////////
    void GameService::mouseLeave( const InputMousePositionEvent & _event )
    {
        EVENTABLE_METHOD( EVENT_GAME_APP_MOUSE_LEAVE )
            ->onGameAppMouseLeave();

        PLAYER_SERVICE()
            ->onAppMouseLeave( _event );
    }
    //////////////////////////////////////////////////////////////////////////
    void GameService::mouseEnter( const InputMousePositionEvent & _event )
    {
        EVENTABLE_METHOD( EVENT_GAME_APP_MOUSE_ENTER )
            ->onGameAppMouseEnter( _event.x, _event.y );

        PLAYER_SERVICE()
            ->onAppMouseEnter( _event );
    }
    //////////////////////////////////////////////////////////////////////////
    void GameService::mousePosition( const InputMousePositionEvent & _event )
    {
        PLAYER_SERVICE()
            ->onAppMousePosition( _event );
    }
    //////////////////////////////////////////////////////////////////////////
    void GameService::update()
    {
#ifdef MENGINE_DEBUG
        const RenderServiceDebugInfo & debugInfo = RENDER_SERVICE()
            ->getDebugInfo();

        float limitFillrate = CONFIG_VALUE( "Limit", "Fillrate", 100.f );

        const Resolution & contentResolution = APPLICATION_SERVICE()
            ->getContentResolution();

        double sreenFillrate = debugInfo.fillrate / double( contentResolution.getWidth() * contentResolution.getHeight() );

        if( sreenFillrate > limitFillrate )
        {
            EVENTABLE_METHOD( EVENT_GAME_OVER_FILLRATE )
                ->onGameOverFillrate( sreenFillrate );
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
    void GameService::tick( const UpdateContext * _context )
    {
        UpdateContext gameContext = *_context;
        gameContext.time *= m_timingFactor;

        PLAYER_SERVICE()
            ->tick( &gameContext );
    }
    //////////////////////////////////////////////////////////////////////////
    void GameService::render()
    {
        PLAYER_SERVICE()
            ->render();

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

        LOGGER_MESSAGE( "personality preparation debug(%d)"
            , is_debug
        );

        bool result = EVENTABLE_METHODR( EVENT_GAME_PREPARATION, true )
            ->onGamePreparation( is_debug );

        if( result == false )
        {
            return false;
        }

        if( SCRIPT_SERVICE()
            ->initializeModules() == false )
        {
            return false;
        }

        ConstString defaultArrowPrototype = CONFIG_VALUE( "DefaultArrow", "Prototype", ConstString::none() );

        if( defaultArrowPrototype.empty() == false )
        {
            ArrowPtr defaultArrow = PROTOTYPE_SERVICE()
                ->generatePrototype( STRINGIZE_STRING_LOCAL( "Arrow" ), defaultArrowPrototype, MENGINE_DOCUMENT_FUNCTION );

            if( defaultArrow == nullptr )
            {
                LOGGER_WARNING( "failed create defaultArrow 'Default'"
                );
            }
            else
            {
                ConstString defaultArrowName = CONFIG_VALUE( "DefaultArrow", "Name", STRINGIZE_STRING_LOCAL( "Default" ) );

                defaultArrow->setName( defaultArrowName );

                PLAYER_SERVICE()
                    ->setArrow( defaultArrow );
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
            LOGGER_ERROR( "failed load accounts"
            );
        }

        bool hasGlobalAccount = ACCOUNT_SERVICE()
            ->hasGlobalAccount();

        if( hasGlobalAccount == false )
        {
            EVENTABLE_METHOD( EVENT_GAME_CREATE_GLOBAL_ACCOUNT )
                ->onGameCreateGlobalAccount();
        }

        bool hasCurrentAccount = ACCOUNT_SERVICE()
            ->hasCurrentAccount();

        if( hasCurrentAccount == false )
        {
            EVENTABLE_METHOD( EVENT_GAME_CREATE_DEFAULT_ACCOUNT )
                ->onGameCreateDefaultAccount();
        }

        EVENTABLE_METHOD( EVENT_GAME_LOAD_ACCOUNTS )
            ->onGameLoadAccounts();

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool GameService::_initializeService()
    {
        CONFIG_SECTION( "Params", m_params );

        GameAccountProviderPtr accountProvider = Helper::makeFactorableUnique<GameServiceAccountProvider>();
        accountProvider->setEventable( EventablePtr( this ) );

        ACCOUNT_SERVICE()
            ->setAccountProvider( accountProvider );

        GameSoundVolumeProviderPtr soundVolumeProvider = Helper::makeFactorableUnique<GameServiceSoundVolumeProvider>();
        soundVolumeProvider->setEventable( EventablePtr( this ) );

        SOUND_SERVICE()
            ->addSoundVolumeProvider( soundVolumeProvider );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void GameService::_finalizeService()
    {
        EVENTABLE_METHOD( EVENT_GAME_ACCOUNT_FINALIZE )
            ->onGameAccountFinalize();

        if( SERVICE_EXIST( ScriptServiceInterface ) == true )
        {
            SCRIPT_SERVICE()
                ->finalizeModules();
        }

        EVENTABLE_METHOD( EVENT_GAME_FINALIZE )
            ->onGameFinalize();

        EVENTABLE_METHOD( EVENT_GAME_DESTROY )
            ->onGameDestroy();

        this->removeEvents();

        m_userEventsAdd.clear();
        m_userEvents.clear();

        m_params.clear();

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
        if( _turn == true )
        {
            if( AMPLIFIER_SERVICE() )
            {
                AMPLIFIER_SERVICE()->resumeMusic();
            }
        }
        else
        {
            if( AMPLIFIER_SERVICE() )
            {
                AMPLIFIER_SERVICE()->pauseMusic();
            }
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void GameService::setFocus( bool _focus )
    {
        if( SERVICE_EXIST( Mengine::PlayerServiceInterface ) == true )
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
        if( SERVICE_EXIST( Mengine::PlayerServiceInterface ) == true )
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
        if( SERVICE_EXIST( Mengine::PlayerServiceInterface ) == true )
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
        if( SERVICE_EXIST( Mengine::PlayerServiceInterface ) == true )
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
    float GameService::getTimeFactor() const
    {
        return m_timingFactor;
    }
    //////////////////////////////////////////////////////////////////////////
    void GameService::setTimeFactor( float _timingFactor )
    {
        m_timingFactor = _timingFactor;

        EVENTABLE_METHOD( EVENT_GAME_ON_TIMING_FACTOR )
            ->onGameTimingFactor( _timingFactor );
    }
    //////////////////////////////////////////////////////////////////////////
    bool GameService::getParam( const ConstString & _paramName, Char * _param ) const
    {
        MapParams::const_iterator it_find = m_params.find( _paramName );

        if( it_find == m_params.end() )
        {
            LOGGER_ERROR( "not found param '%s'"
                , _paramName.c_str()
            );

            return false;
        }

        const String & param = it_find->second;

        strcpy( _param, param.c_str() );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool GameService::hasParam( const ConstString & _paramName ) const
    {
        MapParams::const_iterator it_find = m_params.find( _paramName );

        if( it_find == m_params.end() )
        {
            return false;
        }

        return true;
    }
}
