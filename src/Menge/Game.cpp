#	include "Game.h"

#	include "Kernel/Scene.h"

#	include "Player.h"
#	include "Arrow.h"

#	include "Interface/AmplifierInterface.h"
#	include "Interface/OptionsInterface.h"
#	include "Interface/WatchdogInterface.h"
#	include "Interface/ArchiveInterface.h"

#	include "Interface/ScriptSystemInterface.h"
#	include "Interface/ResourceInterface.h"
#   include "Interface/RenderSystemInterface.h"
#   include "Interface/AccountInterface.h"
#   include "Interface/NodeInterface.h"
#   include "Interface/TextInterface.h"
#   include "Interface/StringizeInterface.h"
#   include "Interface/ConfigInterface.h"

#	include "Logger/Logger.h"

#	include "Consts.h"

#	include "Core/String.h"
#	include "Core/Stream.h"

#   include "pybind/pybind.hpp"

//////////////////////////////////////////////////////////////////////////
SERVICE_FACTORY( GameService, Menge::Game );
//////////////////////////////////////////////////////////////////////////
namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	GameAccountProvider::GameAccountProvider( ServiceProviderInterface * _serviceProvider, Game * _game )
		: m_serviceProvider(_serviceProvider)
		, m_game(_game)
	{
	}
	//////////////////////////////////////////////////////////////////////////
	void GameAccountProvider::onCreateAccount( const WString & _accountID )
	{
		EVENTABLE_CALL(m_serviceProvider, m_game, EVENT_CREATE_ACCOUNT)( _accountID );
	}
	//////////////////////////////////////////////////////////////////////////
	void GameAccountProvider::onDeleteAccount( const WString & _accountID )
	{
		EVENTABLE_CALL(m_serviceProvider, m_game, EVENT_DELETE_ACCOUNT)( _accountID );
	}
	//////////////////////////////////////////////////////////////////////////
	void GameAccountProvider::onSelectAccount( const WString & _accountID )
	{
		EVENTABLE_CALL(m_serviceProvider, m_game, EVENT_SELECT_ACCOUNT)( _accountID );
	}
	//////////////////////////////////////////////////////////////////////////
	void GameAccountProvider::onUnselectAccount( const WString & _accountID )
	{
		EVENTABLE_CALL(m_serviceProvider, m_game, EVENT_UNSELECT_ACCOUNT)( _accountID );
	}
	//////////////////////////////////////////////////////////////////////////
	GameSoundVolumeProvider::GameSoundVolumeProvider( ServiceProviderInterface * _serviceProvider, Game * _game )
		: m_serviceProvider(_serviceProvider)
		, m_game(_game)
	{
	}
	//////////////////////////////////////////////////////////////////////////
	void GameSoundVolumeProvider::onSoundChangeVolume( float _sound, float _music, float _voice )
	{
		EVENTABLE_CALL(m_serviceProvider, m_game, EVENT_CHANGE_SOUND_VOLUME)( _sound, _music, _voice );
	}
	//////////////////////////////////////////////////////////////////////////
	Game::Game()
		: m_accountProvider(nullptr)
		, m_soundVolumeProvider(nullptr)
		, m_defaultArrow(nullptr)
		, m_timingFactor(1.f)
	{
	}
	//////////////////////////////////////////////////////////////////////////
	Game::~Game()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	bool Game::handleKeyEvent( const InputKeyEvent & _event )
	{
		bool handle = false;

		if( handle == false )
		{
			EVENTABLE_ASK( m_serviceProvider, this, EVENT_KEY, handle )((uint32_t)_event.key, _event.x, _event.y, _event.code, _event.isDown, _event.isRepeat);
		}

		if( handle == false )
		{
			handle = PLAYER_SERVICE(m_serviceProvider)
				->handleKeyEvent( _event );
		}	

		return handle;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Game::handleMouseButtonEvent( const InputMouseButtonEvent & _event )
	{
		bool handle = false;

		if( handle == false )
		{
			EVENTABLE_ASK( m_serviceProvider, this, EVENT_MOUSE_BUTTON, handle )(_event.touchId, _event.x, _event.y, _event.button, _event.isDown);
		}

		if( handle == false )
		{
			handle = PLAYER_SERVICE( m_serviceProvider )
				->handleMouseButtonEvent( _event );
		}
                    
		return handle;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Game::handleMouseButtonEventBegin( const InputMouseButtonEvent & _event )
	{
		bool handle = false;

		if( handle == false )
		{
			EVENTABLE_ASK( m_serviceProvider, this, EVENT_MOUSE_BUTTON_BEGIN, handle )(_event.touchId, _event.x, _event.y, _event.button, _event.isDown);
		}

		if( handle == false )
		{
			handle = PLAYER_SERVICE( m_serviceProvider )
				->handleMouseButtonEventBegin( _event );
		}	

		return handle;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Game::handleMouseButtonEventEnd( const InputMouseButtonEvent & _event )
	{
		bool handle = false;

		if( handle == false )
		{
			EVENTABLE_ASK( m_serviceProvider, this, EVENT_MOUSE_BUTTON_END, handle )(_event.touchId, _event.x, _event.y, _event.button, _event.isDown);
		}

		if( handle == false )
		{
			handle = PLAYER_SERVICE( m_serviceProvider )
				->handleMouseButtonEventEnd( _event );
		}	

		return handle;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Game::handleMouseMove( const InputMouseMoveEvent & _event )
	{
		bool handle = false;

		if( handle == false )
		{
			EVENTABLE_ASK( m_serviceProvider, this, EVENT_MOUSE_MOVE, handle )(_event.touchId, _event.x, _event.y, _event.dx, _event.dy);
		}

		if( handle == false )
		{
			handle = PLAYER_SERVICE( m_serviceProvider )
				->handleMouseMove( _event );
		}

		return handle;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Game::handleMouseWheel( const InputMouseWheelEvent & _event )
	{
		bool handle = false;

		if( handle == false )
		{
			EVENTABLE_ASK( m_serviceProvider, this, EVENT_MOUSE_WHEEL, handle )(_event.x, _event.y, _event.wheel);
		}

		if( handle == false )
		{
			handle = PLAYER_SERVICE( m_serviceProvider )
				->handleMouseWheel( _event );
		}

		return handle;
	}
	//////////////////////////////////////////////////////////////////////////
	void Game::mouseLeave( const InputMousePositionEvent & _event )
	{		
		EVENTABLE_CALL(m_serviceProvider, this, EVENT_APP_MOUSE_LEAVE)();

		PLAYER_SERVICE( m_serviceProvider )
			->onAppMouseLeave( _event );
	}
	//////////////////////////////////////////////////////////////////////////
	void Game::mouseEnter( const InputMousePositionEvent & _event )
	{
		EVENTABLE_CALL( m_serviceProvider, this, EVENT_APP_MOUSE_ENTER )(_event.x, _event.y);

		PLAYER_SERVICE( m_serviceProvider )
			->onAppMouseEnter( _event );
	}
	//////////////////////////////////////////////////////////////////////////
	void Game::mousePosition( const InputMousePositionEvent & _event )
	{
		PLAYER_SERVICE( m_serviceProvider )
			->onAppMousePosition( _event );
	}
	//////////////////////////////////////////////////////////////////////////
	void Game::tick( float _timing )
	{
		float timing = _timing * m_timingFactor;
		
		PLAYER_SERVICE( m_serviceProvider )
			->tick( timing );
	}
	//////////////////////////////////////////////////////////////////////////
	void Game::render()
	{
		PLAYER_SERVICE( m_serviceProvider )
			->render();
	}
    //////////////////////////////////////////////////////////////////////////
	void Game::registerEventMethods_( const pybind::object & _embed )
    {	
        this->registerEventMethod( EVENT_FULLSCREEN, "onFullscreen", _embed );
        this->registerEventMethod( EVENT_FIXED_CONTENT_RESOLUTION, "onFixedContentResolution", _embed );
        this->registerEventMethod( EVENT_RENDER_VIEWPORT, "onRenderViewport", _embed );
		this->registerEventMethod( EVENT_GAME_VIEWPORT, "onGameViewport", _embed );
		
        this->registerEventMethod( EVENT_KEY, "onHandleKeyEvent", _embed );
        this->registerEventMethod( EVENT_MOUSE_BUTTON, "onHandleMouseButtonEvent", _embed );
        this->registerEventMethod( EVENT_MOUSE_BUTTON_BEGIN, "onHandleMouseButtonEventBegin", _embed );
        this->registerEventMethod( EVENT_MOUSE_BUTTON_END, "onHandleMouseButtonEventEnd", _embed );
        this->registerEventMethod( EVENT_MOUSE_MOVE, "onHandleMouseMove", _embed );
		this->registerEventMethod( EVENT_MOUSE_WHEEL, "onHandleMouseWheel", _embed );

        this->registerEventMethod( EVENT_APP_MOUSE_ENTER, "onAppMouseEnter", _embed );
        this->registerEventMethod( EVENT_APP_MOUSE_LEAVE, "onAppMouseLeave", _embed );

        this->registerEventMethod( EVENT_ON_TIMING_FACTOR, "onTimingFactor", _embed );

        this->registerEventMethod( EVENT_PREPARATION, "onPreparation", _embed );
		this->registerEventMethod( EVENT_RUN, "onRun", _embed );
        this->registerEventMethod( EVENT_INITIALIZE, "onInitialize", _embed );
        this->registerEventMethod( EVENT_INITIALIZE_RENDER_RESOURCES, "onInitializeRenderResources", _embed );
        this->registerEventMethod( EVENT_ACCOUNT_FINALIZE, "onAccountFinalize", _embed );
        this->registerEventMethod( EVENT_FINALIZE, "onFinalize", _embed );
        this->registerEventMethod( EVENT_DESTROY, "onDestroy", _embed );

        this->registerEventMethod( EVENT_FOCUS, "onFocus", _embed );

		this->registerEventMethod( EVENT_CREATE_DEFAULT_ACCOUNT, "onCreateDefaultAccount", _embed );
		this->registerEventMethod( EVENT_LOAD_ACCOUNTS, "onLoadAccounts", _embed );		

        this->registerEventMethod( EVENT_CREATE_ACCOUNT, "onCreateAccount", _embed );
        this->registerEventMethod( EVENT_DELETE_ACCOUNT, "onDeleteAccount", _embed );
        this->registerEventMethod( EVENT_SELECT_ACCOUNT, "onSelectAccount", _embed );
        this->registerEventMethod( EVENT_UNSELECT_ACCOUNT, "onUnselectAccount", _embed );

		this->registerEventMethod( EVENT_CHANGE_SOUND_VOLUME, "onChangeSoundVolume", _embed );

        this->registerEventMethod( EVENT_CURSOR_MODE, "onCursorMode", _embed );

        this->registerEventMethod( EVENT_USER, "onUserEvent", _embed );
        this->registerEventMethod( EVENT_CLOSE, "onCloseWindow", _embed );
    }
	//////////////////////////////////////////////////////////////////////////
	bool Game::loadPersonality()
	{
		bool developmentMode = HAS_OPTIONS( m_serviceProvider, "dev" );

        SCRIPT_SERVICE(m_serviceProvider)
			->addGlobalModule( "_DEVELOPMENT", pybind::get_bool( developmentMode ) );

#   ifdef MENGINE_MASTER_RELEASE
        SCRIPT_SERVICE(m_serviceProvider)
            ->addGlobalModule( "_MASTER_RELEASE", pybind::get_bool(true) );
#   else
        SCRIPT_SERVICE(m_serviceProvider)
            ->addGlobalModule( "_MASTER_RELEASE", pybind::get_bool(false) );
#   endif

		if( SCRIPT_SERVICE(m_serviceProvider)
			->bootstrapModules() == false )
		{
			LOGGER_ERROR(m_serviceProvider)("Game::loadPersonality invalid bootstrap modules"
				);

			return false;
		}

		ConstString personality = CONFIG_VALUE( m_serviceProvider, "Game", "PersonalityModule", STRINGIZE_STRING_LOCAL( m_serviceProvider, "Personality" ) );

		pybind::object module = SCRIPT_SERVICE( m_serviceProvider )
			->importModule( personality );

		if( module.is_invalid() == true )
		{
			LOGGER_ERROR( m_serviceProvider )("Game::loadPersonality invalid import module '%s'"
				, personality.c_str()
				);

			return false;
		}

		this->registerEventMethods_( module );

#	ifdef _DEBUG
		bool is_debug = true;
#	else
		bool is_debug = false;
#	endif
		
        LOGGER_WARNING(m_serviceProvider)("Preparation debug(%d)"
            , is_debug
            );

		bool result = true;
		EVENTABLE_ASK( m_serviceProvider, this, EVENT_PREPARATION, result )( is_debug );

		if( result == false )
		{
			return false;
		}

		if( SCRIPT_SERVICE( m_serviceProvider )
			->initializeModules() == false )
		{
			return false;
		}

		if( ACCOUNT_SERVICE( m_serviceProvider )
			->loadAccounts() == false )
		{
			LOGGER_ERROR( m_serviceProvider )("Game::initialize failed load accounts"
				);
		}

		m_defaultArrow = PROTOTYPE_SERVICE( m_serviceProvider )
			->generatePrototypeT<Arrow>( STRINGIZE_STRING_LOCAL( m_serviceProvider, "Arrow" ), STRINGIZE_STRING_LOCAL( m_serviceProvider, "Default" ) );

		if( m_defaultArrow == nullptr )
		{
			LOGGER_WARNING( m_serviceProvider )("Game::initialize failed create defaultArrow 'Default'"
				);
		}
		else
		{
			m_defaultArrow->setName( STRINGIZE_STRING_LOCAL( m_serviceProvider, "Default" ) );

			PLAYER_SERVICE( m_serviceProvider )
				->setArrow( m_defaultArrow );
		}

		bool EVENT_INITIALIZE_result = true;
		EVENTABLE_ASK( m_serviceProvider, this, EVENT_INITIALIZE, EVENT_INITIALIZE_result )();

		if( EVENT_INITIALIZE_result == false )
		{
			return false;
		}

		bool hasCurrentAccount = ACCOUNT_SERVICE( m_serviceProvider )
			->hasCurrentAccount();

		if( hasCurrentAccount == false )
		{
			EVENTABLE_CALL( m_serviceProvider, this, EVENT_CREATE_DEFAULT_ACCOUNT )();
		}

		EVENTABLE_CALL( m_serviceProvider, this, EVENT_LOAD_ACCOUNTS )();

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Game::_initialize()
	{
		m_archivator = ARCHIVE_SERVICE( m_serviceProvider )
			->getArchivator( STRINGIZE_STRING_LOCAL( m_serviceProvider, "lz4" ) );

		if( m_archivator == nullptr )
		{
			return false;
		}

		CONFIG_SECTION( m_serviceProvider, "Params", m_params );

		SERVICE_CREATE( m_serviceProvider, AccountService );

		m_accountProvider = new GameAccountProvider( m_serviceProvider, this );

		ACCOUNT_SERVICE( m_serviceProvider )
			->setAccountProviderInterface( m_accountProvider );

		m_soundVolumeProvider = new GameSoundVolumeProvider( m_serviceProvider, this );

		SOUND_SERVICE( m_serviceProvider )
			->addSoundVolumeProvider( m_soundVolumeProvider );

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void Game::run()
	{
		LOGGER_WARNING(m_serviceProvider)("Run game"
			);

		EVENTABLE_CALL( m_serviceProvider, this, EVENT_RUN )();
	}
	//////////////////////////////////////////////////////////////////////////
	void Game::destroyArrow()
	{
		if( m_defaultArrow != nullptr )
		{
			m_defaultArrow->destroy();
			m_defaultArrow = nullptr;
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void Game::_finalize()
	{	
		EVENTABLE_CALL( m_serviceProvider, this, EVENT_ACCOUNT_FINALIZE )();

		SERVICE_FINALIZE( m_serviceProvider, Menge::AccountServiceInterface );

		EVENTABLE_CALL( m_serviceProvider, this, EVENT_FINALIZE )();

        if( m_accountProvider != nullptr )
        {
            delete m_accountProvider;
            m_accountProvider = nullptr;
        }

		if( m_soundVolumeProvider != nullptr )
		{
			delete m_soundVolumeProvider;
			m_soundVolumeProvider = nullptr;
		}

		this->destroyArrow();		      
				
		EVENTABLE_CALL( m_serviceProvider, this, EVENT_DESTROY )();
	}
	//////////////////////////////////////////////////////////////////////////
	void Game::initializeRenderResources()
	{
		EVENTABLE_CALL( m_serviceProvider, this, EVENT_INITIALIZE_RENDER_RESOURCES )();
	}
	//////////////////////////////////////////////////////////////////////////
	void Game::finalizeRenderResources()
	{
		EVENTABLE_CALL( m_serviceProvider, this, EVENT_FINALIZE_RENDER_RESOURCES )();
	}	
	//////////////////////////////////////////////////////////////////////////
	void Game::turnSound( bool _turn )
	{
        (void)_turn;
		//if( AMPLIFIER_SERVICE(m_serviceProvider) )
		//{
		//	AMPLIFIER_SERVICE(m_serviceProvider)->onTurnSound( _turn );
		//}
	}
	//////////////////////////////////////////////////////////////////////////
	void Game::setFocus( bool _focus )
	{
		if( SERVICE_EXIST( m_serviceProvider, Menge::PlayerServiceInterface ) == true )
		{
			PLAYER_SERVICE( m_serviceProvider )
				->onFocus( _focus );
		}				

		EVENTABLE_CALL(m_serviceProvider, this, EVENT_FOCUS)( _focus );
	}
	//////////////////////////////////////////////////////////////////////////
	void Game::setFullscreen( const Resolution & _resolution, bool _fullscreen )
	{
		if( SERVICE_EXIST( m_serviceProvider, Menge::PlayerServiceInterface ) == true )
		{
			PLAYER_SERVICE( m_serviceProvider )
				->onFullscreen( _resolution, _fullscreen );
		}

		EVENTABLE_CALL(m_serviceProvider, this, EVENT_FULLSCREEN)( _fullscreen );
	}
    //////////////////////////////////////////////////////////////////////////
    void Game::setFixedContentResolution( const Resolution & _resolution, bool _fixed )
    {
		if( SERVICE_EXIST( m_serviceProvider, Menge::PlayerServiceInterface ) == true )
		{
			PLAYER_SERVICE( m_serviceProvider )
				->onFixedContentResolution( _resolution, _fixed );
		}

		EVENTABLE_CALL( m_serviceProvider, this, EVENT_FIXED_CONTENT_RESOLUTION )(_fixed);
    }
    //////////////////////////////////////////////////////////////////////////
    void Game::setRenderViewport( const Viewport & _viewport, const Resolution & _contentResolution )
    {
		EVENTABLE_CALL( m_serviceProvider, this, EVENT_RENDER_VIEWPORT )(_viewport, _contentResolution);
    }
	//////////////////////////////////////////////////////////////////////////
	void Game::setGameViewport( const Viewport & _viewport, float _aspect )
	{
		EVENTABLE_CALL( m_serviceProvider, this, EVENT_GAME_VIEWPORT )(_viewport, _aspect);
	}
	//////////////////////////////////////////////////////////////////////////
	bool Game::close()
	{
		bool needQuit = true;

		EVENTABLE_ASK( m_serviceProvider, this, EVENT_CLOSE, needQuit )();
	
		return needQuit;
	}
    //////////////////////////////////////////////////////////////////////////
    void Game::userEvent( const ConstString & _event, const TMapParams & _params )
    {
		EVENTABLE_CALL( m_serviceProvider, this, EVENT_USER )(_event, _params);
    }
	//////////////////////////////////////////////////////////////////////////
	void Game::setCursorMode( bool _mode )
	{
		EVENTABLE_CALL( m_serviceProvider, this, EVENT_CURSOR_MODE )(_mode);
	}
	//////////////////////////////////////////////////////////////////////////
	float Game::getTimingFactor() const
	{
		return m_timingFactor;
	}
	//////////////////////////////////////////////////////////////////////////
	void Game::setTimingFactor( float _timingFactor )
	{
		m_timingFactor = _timingFactor;

		EVENTABLE_CALL( m_serviceProvider, this, EVENT_ON_TIMING_FACTOR )(m_timingFactor);
	}
	//////////////////////////////////////////////////////////////////////////
	const WString & Game::getParam( const ConstString & _paramName ) const
	{
		TMapParams::const_iterator it_find = m_params.find( _paramName );

        if( it_find == m_params.end() )
        {
            LOGGER_ERROR(m_serviceProvider)("Game::getParam not found param '%s'"
                , _paramName.c_str()
                );

            return Utils::emptyWString();
        }

        const WString & param = it_find->second;

		return param;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Game::hasParam( const ConstString & _paramName ) const
	{
		TMapParams::const_iterator it_find = m_params.find( _paramName );

		if( it_find == m_params.end() )
		{
			return false;
		}

		return true;
	}
}
