#	include "Game.h"

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
#   include "Interface/PlayerInterface.h"

#   include "GameAccountProvider.h"
#   include "GameSoundVolumeProvider.h"

#   include "Kernel/ScriptEventReceiver.h"

#   include "Kernel/Arrow.h"

#	include "Logger/Logger.h"

#	include "Core/String.h"
#	include "Core/Stream.h"

//////////////////////////////////////////////////////////////////////////
SERVICE_EXTERN( AccountService );
//////////////////////////////////////////////////////////////////////////
SERVICE_FACTORY( GameService, Menge::Game );
//////////////////////////////////////////////////////////////////////////
namespace Menge
{

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
			mt::vec2f point( _event.x, _event.y );

			mt::vec2f wp;
			PLAYER_SERVICE( m_serviceProvider )
				->calcGlobalMouseWorldPosition( point, wp );

            handle = EVENTABLE_METHODR( this, EVENT_GAME_KEY, handle )
                ->onGameKey( _event.key, wp.x, wp.y, _event.code, _event.isDown, _event.isRepeat );
			//EVENTABLE_ASK( m_serviceProvider, this, EVENT_GAME_KEY, handle )((uint32_t)_event.key, wp.x, wp.y, _event.code, _event.isDown, _event.isRepeat);
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
			mt::vec2f point( _event.x, _event.y );

			mt::vec2f wp;
			PLAYER_SERVICE( m_serviceProvider )
				->calcGlobalMouseWorldPosition( point, wp );

            handle = EVENTABLE_METHODR( this, EVENT_GAME_MOUSE_BUTTON, handle )
                ->onGameMouseButton( _event.touchId, wp.x, wp.y, _event.button, _event.isDown );
			//EVENTABLE_ASK( m_serviceProvider, this, EVENT_GAME_MOUSE_BUTTON, handle )(_event.touchId, wp.x, wp.y, _event.button, _event.isDown);
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
			mt::vec2f point( _event.x, _event.y );

			mt::vec2f wp;
			PLAYER_SERVICE( m_serviceProvider )
				->calcGlobalMouseWorldPosition( point, wp );

            handle = EVENTABLE_METHODR( this, EVENT_GAME_MOUSE_BUTTON_BEGIN, handle )
                ->onGameMouseButtonBegin( _event.touchId, wp.x, wp.y, _event.button, _event.isDown );
			//EVENTABLE_ASK( m_serviceProvider, this, EVENT_GAME_MOUSE_BUTTON_BEGIN, handle )(_event.touchId, wp.x, wp.y, _event.button, _event.isDown);
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
			mt::vec2f point( _event.x, _event.y );

			mt::vec2f wp;
			PLAYER_SERVICE( m_serviceProvider )
				->calcGlobalMouseWorldPosition( point, wp );

            handle = EVENTABLE_METHODR( this, EVENT_GAME_MOUSE_BUTTON_END, handle )
                ->onGameMouseButtonEnd( _event.touchId, wp.x, wp.y, _event.button, _event.isDown );
			//EVENTABLE_ASK( m_serviceProvider, this, EVENT_GAME_MOUSE_BUTTON_END, handle )(_event.touchId, wp.x, wp.y, _event.button, _event.isDown);
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
			mt::vec2f point( _event.x, _event.y );
			mt::vec2f delta( _event.dx, _event.dy );

			mt::vec2f wp;
			PLAYER_SERVICE(m_serviceProvider)
				->calcGlobalMouseWorldPosition( point, wp );

			mt::vec2f wd;
			PLAYER_SERVICE( m_serviceProvider )
				->calcGlobalMouseWorldDelta( point, delta, wd );

            handle = EVENTABLE_METHODR( this, EVENT_GAME_MOUSE_MOVE, handle )
                ->onGameMouseMove( _event.touchId, wp.x, wp.y, wd.x, wd.y );
			//EVENTABLE_ASK( m_serviceProvider, this, EVENT_GAME_MOUSE_MOVE, handle )(_event.touchId, wp.x, wp.y, wd.x, wd.y);
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
            handle = EVENTABLE_METHODR( this, EVENT_GAME_MOUSE_WHEEL, handle )
                ->onGameMouseWheel( _event.button, _event.x, _event.y, _event.wheel );
			//EVENTABLE_ASK( m_serviceProvider, this, EVENT_GAME_MOUSE_WHEEL, handle )(_event.button, _event.x, _event.y, _event.wheel);
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
        EVENTABLE_METHOD( this, EVENT_GAME_APP_MOUSE_LEAVE )
            ->onGameAppMouseLeave();

		PLAYER_SERVICE( m_serviceProvider )
			->onAppMouseLeave( _event );
	}
	//////////////////////////////////////////////////////////////////////////
	void Game::mouseEnter( const InputMousePositionEvent & _event )
	{
        EVENTABLE_METHOD( this, EVENT_GAME_APP_MOUSE_ENTER )
            ->onGameAppMouseEnter( _event.x, _event.y );
		
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
	void Game::update()
	{
		m_events.insert( m_events.end(), m_eventsAdd.begin(), m_eventsAdd.end() );
		m_eventsAdd.clear();

		for( TVectorUserEvents::const_iterator
			it = m_events.begin(),
			it_end = m_events.end();
		it != it_end;
		++it )
		{
			const UserEvent & ev = *it;

            EVENTABLE_METHOD( this, EVENT_GAME_USER )
                ->onGameUser( ev.id, ev.params );
		}

		m_events.clear();
	}
	//////////////////////////////////////////////////////////////////////////
	void Game::tick( float _time, float _timing )
	{
		float timing = _timing * m_timingFactor;
		
		PLAYER_SERVICE( m_serviceProvider )
			->tick( _time, timing );
	}
	//////////////////////////////////////////////////////////////////////////
	void Game::render()
	{
		PLAYER_SERVICE( m_serviceProvider )
			->render();
	}
    //////////////////////////////////////////////////////////////////////////
    namespace
    {
        class PythonGameEventReceiver
            : public GameEventReceiver
            , public ScriptEventReceiver
        {
        protected:
            void onGameFullscreen( bool _fullscreen ) override
            {
                m_cb.call( _fullscreen );
            }

            void onGameFixedContentResolution( bool _fixed ) override
            {
                m_cb.call( _fixed );
            }

            void onGameFixedDisplayResolution( bool _fixed ) override
            {
                m_cb.call( _fixed );
            }

            void onGameRenderViewport( const Viewport & _viewport, const Resolution & _contentResolution ) override
            {
                m_cb.call( _viewport, _contentResolution );
            }

            void onGameViewport( const Viewport & _viewport, float _aspect ) override
            {
                m_cb.call( _viewport, _aspect );
            }

            bool onGameKey( KeyCode _key, float _x, float _y, WChar _code, bool _isDown, bool _isRepeat ) override
            {
                return m_cb.call( (uint32_t)_key, _x, _y, _code, _isDown, _isRepeat );
            }

            bool onGameMouseButton( uint32_t _touchId, float _x, float _y, uint32_t _button, bool _isDown ) override
            {
                return m_cb.call( _touchId, _x, _y, _button, _isDown );
            }

            bool onGameMouseButtonBegin( uint32_t _touchId, float _x, float _y, uint32_t _button, bool _isDown ) override
            {
                return m_cb.call( _touchId, _x, _y, _button, _isDown );
            }

            bool onGameMouseButtonEnd( uint32_t _touchId, float _x, float _y, uint32_t _button, bool _isDown ) override
            {
                return m_cb.call( _touchId, _x, _y, _button, _isDown );
            }

            bool onGameMouseMove( uint32_t _touchId, float _x, float _y, float _dx, float _dy ) override
            {
                return m_cb.call( _touchId, _x, _y, _dx, _dy );
            }

            bool onGameMouseWheel( uint32_t _button, float _x, float _y, int32_t _wheel ) override
            {
                return m_cb.call( _button, _x, _y, _wheel );
            }

            void onGameAppMouseEnter( float _x, float _y ) override
            {
                m_cb.call( _x, _y );
            }

            void onGameAppMouseLeave() override
            {
                m_cb.call();
            }

            void onGameTimingFactor( float _timingFactor ) override
            {
                m_cb.call( _timingFactor );
            }

            bool onGamePreparation( bool _debug ) override
            {
                return m_cb.call( _debug );
            }

            void onGameRun() override
            {
                m_cb.call();
            }

            bool onGameInitialize() override
            {
                return m_cb.call();
            }

            void onGameInitializeRenderResources() override
            {
                m_cb.call();
            }

            void onGameFinalizeRenderResources() override
            {
                m_cb.call();
            }

            void onGameAccountFinalize() override
            {
                m_cb.call();
            }

            void onGameFinalize() override
            {
                m_cb.call();
            }

            void onGameDestroy() override
            {
                m_cb.call();
            }

            void onGameFocus( bool _focus ) override
            {
                m_cb.call( _focus );
            }

            void onGameCreateDefaultAccount() override
            {
                m_cb.call();
            }

            void onGameLoadAccounts() override
            {
                m_cb.call();
            }

            void onGameCreateAccount( const WString & _accountID ) override
            {
                m_cb.call( _accountID);
            }

            void onGameDeleteAccount( const WString & _accountID ) override
            {
                m_cb.call( _accountID );
            }

            void onGameSelectAccount( const WString & _accountID ) override
            {
                m_cb.call( _accountID );
            }

            void onGameUselectAccount( const WString & _accountID ) override
            {
                m_cb.call( _accountID );
            }

            void onGameChangeSoundVolume( float _sound, float _music, float _voice ) override
            {
                m_cb.call( _sound, _music, _voice );
            }

            void onGameCursorMode( bool _mode ) override
            {
                m_cb.call( _mode );
            }

            void onGameUser( const ConstString & _event, const TMapParams & _params ) override
            {
                m_cb.call( _event, _params );
            }

            bool onGameClose() override
            {
                return m_cb.call();
            }
        };
    }
    //////////////////////////////////////////////////////////////////////////
	void Game::registerEventMethods_( const ScriptModuleInterfacePtr & _module )
    {	
        const pybind::base & base_module = _module->getModule();

        pybind::module py_module( base_module );

        Helper::registerScriptEventReceiverModule<PythonGameEventReceiver>( py_module, this, "onFullscreen", EVENT_GAME_FULLSCREEN );
        Helper::registerScriptEventReceiverModule<PythonGameEventReceiver>( py_module, this, "onFixedContentResolution", EVENT_GAME_FIXED_CONTENT_RESOLUTION );
        Helper::registerScriptEventReceiverModule<PythonGameEventReceiver>( py_module, this, "onFixedDisplayResolution", EVENT_GAME_FIXED_DISPLAY_RESOLUTION );
        Helper::registerScriptEventReceiverModule<PythonGameEventReceiver>( py_module, this, "onRenderViewport", EVENT_GAME_RENDER_VIEWPORT );
        Helper::registerScriptEventReceiverModule<PythonGameEventReceiver>( py_module, this, "onGameViewport", EVENT_GAME_VIEWPORT );

        Helper::registerScriptEventReceiverModule<PythonGameEventReceiver>( py_module, this, "onHandleKeyEvent", EVENT_GAME_KEY );
        Helper::registerScriptEventReceiverModule<PythonGameEventReceiver>( py_module, this, "onHandleMouseButtonEvent", EVENT_GAME_MOUSE_BUTTON );
        Helper::registerScriptEventReceiverModule<PythonGameEventReceiver>( py_module, this, "onHandleMouseButtonEventBegin", EVENT_GAME_MOUSE_BUTTON_BEGIN );
        Helper::registerScriptEventReceiverModule<PythonGameEventReceiver>( py_module, this, "onHandleMouseButtonEventEnd", EVENT_GAME_MOUSE_BUTTON_END );
        Helper::registerScriptEventReceiverModule<PythonGameEventReceiver>( py_module, this, "onHandleMouseMove", EVENT_GAME_MOUSE_MOVE );
        Helper::registerScriptEventReceiverModule<PythonGameEventReceiver>( py_module, this, "onHandleMouseWheel", EVENT_GAME_MOUSE_WHEEL );

        Helper::registerScriptEventReceiverModule<PythonGameEventReceiver>( py_module, this, "onAppMouseEnter", EVENT_GAME_APP_MOUSE_ENTER );
        Helper::registerScriptEventReceiverModule<PythonGameEventReceiver>( py_module, this, "onAppMouseLeave", EVENT_GAME_APP_MOUSE_LEAVE );
        Helper::registerScriptEventReceiverModule<PythonGameEventReceiver>( py_module, this, "onTimingFactor", EVENT_GAME_ON_TIMING_FACTOR );

        Helper::registerScriptEventReceiverModule<PythonGameEventReceiver>( py_module, this, "onPreparation", EVENT_GAME_PREPARATION );
        Helper::registerScriptEventReceiverModule<PythonGameEventReceiver>( py_module, this, "onRun", EVENT_GAME_RUN );
        Helper::registerScriptEventReceiverModule<PythonGameEventReceiver>( py_module, this, "onInitialize", EVENT_GAME_INITIALIZE );
        Helper::registerScriptEventReceiverModule<PythonGameEventReceiver>( py_module, this, "onInitializeRenderResources", EVENT_GAME_INITIALIZE_RENDER_RESOURCES );
        Helper::registerScriptEventReceiverModule<PythonGameEventReceiver>( py_module, this, "onAccountFinalize", EVENT_GAME_ACCOUNT_FINALIZE );
        Helper::registerScriptEventReceiverModule<PythonGameEventReceiver>( py_module, this, "onFinalize", EVENT_GAME_FINALIZE );
        Helper::registerScriptEventReceiverModule<PythonGameEventReceiver>( py_module, this, "onDestroy", EVENT_GAME_DESTROY );

        Helper::registerScriptEventReceiverModule<PythonGameEventReceiver>( py_module, this, "onFocus", EVENT_GAME_FOCUS );
        Helper::registerScriptEventReceiverModule<PythonGameEventReceiver>( py_module, this, "onCreateDefaultAccount", EVENT_GAME_CREATE_DEFAULT_ACCOUNT );
        Helper::registerScriptEventReceiverModule<PythonGameEventReceiver>( py_module, this, "onLoadAccounts", EVENT_GAME_LOAD_ACCOUNTS );

        Helper::registerScriptEventReceiverModule<PythonGameEventReceiver>( py_module, this, "onCreateAccount", EVENT_GAME_CREATE_ACCOUNT );
        Helper::registerScriptEventReceiverModule<PythonGameEventReceiver>( py_module, this, "onDeleteAccount", EVENT_GAME_DELETE_ACCOUNT );
        Helper::registerScriptEventReceiverModule<PythonGameEventReceiver>( py_module, this, "onSelectAccount", EVENT_GAME_SELECT_ACCOUNT );
        Helper::registerScriptEventReceiverModule<PythonGameEventReceiver>( py_module, this, "onUnselectAccount", EVENT_GAME_UNSELECT_ACCOUNT );

        Helper::registerScriptEventReceiverModule<PythonGameEventReceiver>( py_module, this, "onChangeSoundVolume", EVENT_GAME_CHANGE_SOUND_VOLUME );
        Helper::registerScriptEventReceiverModule<PythonGameEventReceiver>( py_module, this, "onCursorMode", EVENT_GAME_CURSOR_MODE );
        Helper::registerScriptEventReceiverModule<PythonGameEventReceiver>( py_module, this, "onUserEvent", EVENT_GAME_USER );
        Helper::registerScriptEventReceiverModule<PythonGameEventReceiver>( py_module, this, "onCloseWindow", EVENT_GAME_CLOSE );
    }
	//////////////////////////////////////////////////////////////////////////
	bool Game::loadPersonality()
	{
		bool developmentMode = HAS_OPTION( m_serviceProvider, "dev" );

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

		ScriptModuleInterfacePtr module = SCRIPT_SERVICE( m_serviceProvider )
			->importModule( personality );

		if( module == nullptr )
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

        bool result = EVENTABLE_METHODR( this, EVENT_GAME_PREPARATION, true )
            ->onGamePreparation( is_debug );
		//bool result = true;
		//EVENTABLE_ASK( m_serviceProvider, this, EVENT_GAME_PREPARATION, result )( is_debug );

		if( result == false )
		{
			return false;
		}

		if( SCRIPT_SERVICE( m_serviceProvider )
			->initializeModules() == false )
		{
			return false;
		}

		m_defaultArrow = PROTOTYPE_SERVICE( m_serviceProvider )
			->generatePrototypeT<Arrow *>( STRINGIZE_STRING_LOCAL( m_serviceProvider, "Arrow" ), STRINGIZE_STRING_LOCAL( m_serviceProvider, "Default" ) );

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

        bool EVENT_INITIALIZE_result = EVENTABLE_METHODR( this, EVENT_GAME_INITIALIZE, true )
            ->onGameInitialize();

		//bool EVENT_INITIALIZE_result = true;
		//EVENTABLE_ASK( m_serviceProvider, this, EVENT_GAME_INITIALIZE, EVENT_INITIALIZE_result )();

		if( EVENT_INITIALIZE_result == false )
		{
			return false;
		}

		if( ACCOUNT_SERVICE( m_serviceProvider )
			->loadAccounts() == false )
		{
			LOGGER_ERROR( m_serviceProvider )("Game::initialize failed load accounts"
				);
		}

		bool hasCurrentAccount = ACCOUNT_SERVICE( m_serviceProvider )
			->hasCurrentAccount();

		if( hasCurrentAccount == false )
		{
            EVENTABLE_METHOD( this, EVENT_GAME_CREATE_DEFAULT_ACCOUNT )
                ->onGameCreateDefaultAccount();
		}

        EVENTABLE_METHOD( this, EVENT_GAME_LOAD_ACCOUNTS )
            ->onGameLoadAccounts();

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Game::_initialize()
	{
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

        EVENTABLE_METHOD( this, EVENT_GAME_RUN )
            ->onGameRun();
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
        EVENTABLE_METHOD( this, EVENT_GAME_ACCOUNT_FINALIZE )
            ->onGameAccountFinalize();

		SERVICE_FINALIZE( m_serviceProvider, Menge::AccountServiceInterface );

		SCRIPT_SERVICE( m_serviceProvider )
			->finalizeModules();

        EVENTABLE_METHOD( this, EVENT_GAME_FINALIZE )
            ->onGameFinalize();

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
				
        EVENTABLE_METHOD( this, EVENT_GAME_DESTROY )
            ->onGameDestroy();
		
		this->removeEvents();
	}
	//////////////////////////////////////////////////////////////////////////
	void Game::initializeRenderResources()
	{
        EVENTABLE_METHOD( this, EVENT_GAME_INITIALIZE_RENDER_RESOURCES )
            ->onGameInitializeRenderResources();
	}
	//////////////////////////////////////////////////////////////////////////
	void Game::finalizeRenderResources()
	{
        EVENTABLE_METHOD( this, EVENT_GAME_FINALIZE_RENDER_RESOURCES )
            ->onGameFinalizeRenderResources();
	}	
	//////////////////////////////////////////////////////////////////////////
	void Game::turnSound( bool _turn )
	{
        (void)_turn;
		
		if( _turn == true )
		{
			if( AMPLIFIER_SERVICE( m_serviceProvider ) )
			{
				AMPLIFIER_SERVICE( m_serviceProvider )->resume();
			}
		}
		else
		{
			if( AMPLIFIER_SERVICE( m_serviceProvider ) )
			{
				AMPLIFIER_SERVICE( m_serviceProvider )->pause();
			}
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void Game::setFocus( bool _focus )
	{
		if( SERVICE_EXIST( m_serviceProvider, Menge::PlayerServiceInterface ) == true )
		{
			PLAYER_SERVICE( m_serviceProvider )
				->onFocus( _focus );
		}				

        EVENTABLE_METHOD( this, EVENT_GAME_FOCUS )
            ->onGameFocus( _focus );
	}
	//////////////////////////////////////////////////////////////////////////
	void Game::setFullscreen( const Resolution & _resolution, bool _fullscreen )
	{
		if( SERVICE_EXIST( m_serviceProvider, Menge::PlayerServiceInterface ) == true )
		{
			PLAYER_SERVICE( m_serviceProvider )
				->onFullscreen( _resolution, _fullscreen );
		}

        EVENTABLE_METHOD( this, EVENT_GAME_FULLSCREEN )
            ->onGameFullscreen( _fullscreen );
	}
    //////////////////////////////////////////////////////////////////////////
    void Game::setFixedContentResolution( const Resolution & _resolution, bool _fixed )
    {
		if( SERVICE_EXIST( m_serviceProvider, Menge::PlayerServiceInterface ) == true )
		{
			PLAYER_SERVICE( m_serviceProvider )
				->onFixedContentResolution( _resolution, _fixed );
		}

        EVENTABLE_METHOD( this, EVENT_GAME_FIXED_CONTENT_RESOLUTION )
            ->onGameFixedContentResolution( _fixed );
    }
	//////////////////////////////////////////////////////////////////////////
	void Game::setFixedDisplayResolution( const Resolution & _resolution, bool _fixed )
	{
		if( SERVICE_EXIST( m_serviceProvider, Menge::PlayerServiceInterface ) == true )
		{
			PLAYER_SERVICE( m_serviceProvider )
				->onFixedContentResolution( _resolution, _fixed );
		}

        EVENTABLE_METHOD( this, EVENT_GAME_FIXED_DISPLAY_RESOLUTION )
            ->onGameFixedDisplayResolution( _fixed );
	}
    //////////////////////////////////////////////////////////////////////////
    void Game::setRenderViewport( const Viewport & _viewport, const Resolution & _contentResolution )
    {
        EVENTABLE_METHOD( this, EVENT_GAME_RENDER_VIEWPORT )
            ->onGameRenderViewport( _viewport, _contentResolution );
    }
	//////////////////////////////////////////////////////////////////////////
	void Game::setGameViewport( const Viewport & _viewport, float _aspect )
	{
        EVENTABLE_METHOD( this, EVENT_GAME_VIEWPORT )
            ->onGameViewport( _viewport, _aspect );
	}
	//////////////////////////////////////////////////////////////////////////
	bool Game::close()
	{
		bool needQuit = EVENTABLE_METHODR( this, EVENT_GAME_VIEWPORT, true )
            ->onGameClose();
	
		return needQuit;
	}
    //////////////////////////////////////////////////////////////////////////
    void Game::userEvent( const ConstString & _id, const TMapParams & _params )
    {
		UserEvent ev;
		ev.id = _id;
		ev.params = _params;

		m_events.push_back( ev );
    }
	//////////////////////////////////////////////////////////////////////////
	void Game::setCursorMode( bool _mode )
	{
        EVENTABLE_METHOD( this, EVENT_GAME_CURSOR_MODE )
            ->onGameCursorMode( _mode );
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

        EVENTABLE_METHOD( this, EVENT_GAME_ON_TIMING_FACTOR )
            ->onGameTimingFactor( _timingFactor );
	}
	//////////////////////////////////////////////////////////////////////////
	WString Game::getParam( const ConstString & _paramName ) const
	{
		TMapParams::const_iterator it_find = m_params.find( _paramName );

        if( it_find == m_params.end() )
        {
            LOGGER_ERROR(m_serviceProvider)("Game::getParam not found param '%s'"
                , _paramName.c_str()
                );

            return WString();
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
