#	include "Game.h"

#	include "Kernel/Scene.h"

#	include "Player.h"
#	include "Arrow.h"

#	include "Interface/AmplifierInterface.h"
#	include "Interface/WatchdogInterface.h"

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

#   include "pybind/pybind.hpp"

//////////////////////////////////////////////////////////////////////////
SERVICE_EXTERN( AccountService, Menge::AccountServiceInterface );
//////////////////////////////////////////////////////////////////////////
SERVICE_FACTORY( GameService, Menge::GameServiceInterface, Menge::Game );
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
		EVENTABLE_CALL(m_serviceProvider, m_game, EVENT_CREATE_ACCOUNT)( "(u)", _accountID.c_str() );
	}
	//////////////////////////////////////////////////////////////////////////
	void GameAccountProvider::onDeleteAccount( const WString & _accountID )
	{
		EVENTABLE_CALL(m_serviceProvider, m_game, EVENT_DELETE_ACCOUNT)( "(u)", _accountID.c_str() );
	}
	//////////////////////////////////////////////////////////////////////////
	void GameAccountProvider::onSelectAccount( const WString & _accountID )
	{
		EVENTABLE_CALL(m_serviceProvider, m_game, EVENT_SELECT_ACCOUNT)( "(u)", _accountID.c_str() );
	}
	//////////////////////////////////////////////////////////////////////////
	void GameAccountProvider::onUnselectAccount( const WString & _accountID )
	{
		EVENTABLE_CALL(m_serviceProvider, m_game, EVENT_UNSELECT_ACCOUNT)( "(u)", _accountID.c_str() );
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
		EVENTABLE_CALL(m_serviceProvider, m_game, EVENT_CHANGE_SOUND_VOLUME)( "(fff)", _sound, _music, _voice );
	}
	//////////////////////////////////////////////////////////////////////////
	Game::Game()
		: m_serviceProvider(nullptr)
		, m_developmentMode(false)
		, m_hasWindowPanel(true)
		, m_player(nullptr)
		, m_accountProvider(nullptr)
		, m_accountService(nullptr)
		, m_defaultArrow(nullptr)
		, m_timingFactor(1.f)
        , m_embed(nullptr)
	{
	}
	//////////////////////////////////////////////////////////////////////////
	Game::~Game()
	{
		pybind::decref( m_embed );
		m_embed = nullptr;
	}
    //////////////////////////////////////////////////////////////////////////
    void Game::setServiceProvider( ServiceProviderInterface * _serviceProvider )
    {
        m_serviceProvider = _serviceProvider;
    }
    //////////////////////////////////////////////////////////////////////////
    ServiceProviderInterface * Game::getServiceProvider() const
    {
        return m_serviceProvider;
    }
    //////////////////////////////////////////////////////////////////////////
    void Game::setDevelopmentMode( bool _developmentMode )
    {
        m_developmentMode = _developmentMode;
    }
    //////////////////////////////////////////////////////////////////////////
    void Game::setPlatformName( const ConstString & _platformName )
    {
        m_platformName = _platformName;
    }
    //////////////////////////////////////////////////////////////////////////
    PlayerServiceInterface * Game::getPlayer() const
    {
        return m_player;
    }
	//////////////////////////////////////////////////////////////////////////
	bool Game::handleKeyEvent( const mt::vec2f & _point, unsigned int _key, unsigned int _char, bool _isDown, bool _repeating )
	{
		bool handle = false;

		if( handle == false )
		{
			EVENTABLE_ASK(m_serviceProvider, this, EVENT_KEY)( handle, false, "(IffIOO)"
				, _key
				, _point.x
				, _point.y
				, _char
				, pybind::get_bool(_isDown)
				, pybind::get_bool(_repeating) 
				);
		}

		if( handle == false )
		{
			handle = m_player->handleKeyEvent( _point, _key, _char, _isDown, _repeating );
		}	

		return handle;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Game::handleMouseButtonEvent( unsigned int _touchId, const mt::vec2f & _point, unsigned int _button, bool _isDown )
	{
		bool handle = false;

		if( handle == false )
		{
			EVENTABLE_ASK(m_serviceProvider, this, EVENT_MOUSE_BUTTON)( handle, false, "(IffIO)"
				, _touchId
				, _point.x
				, _point.y
				, _button
				, pybind::get_bool(_isDown) 
				);
		}

		if( handle == false )
		{
			handle = m_player->handleMouseButtonEvent( _touchId, _point, _button, _isDown );
		}
                    
		return handle;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Game::handleMouseButtonEventBegin( unsigned int _touchId, const mt::vec2f & _point, unsigned int _button, bool _isDown )
	{
		bool handle = false;

		if( handle == false )
		{
			EVENTABLE_ASK(m_serviceProvider, this, EVENT_MOUSE_BUTTON_BEGIN)( handle, false, "(IffIO)"
				, _touchId
				, _point.x
				, _point.y
				, _button
				, pybind::get_bool(_isDown) 
				);
		}

		if( handle == false )
		{
			handle = m_player->handleMouseButtonEventBegin( _touchId, _point, _button, _isDown );
		}	

		return handle;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Game::handleMouseButtonEventEnd( unsigned int _touchId, const mt::vec2f & _point, unsigned int _button, bool _isDown )
	{
		bool handle = false;

		if( handle == false )
		{
			EVENTABLE_ASK(m_serviceProvider, this, EVENT_MOUSE_BUTTON_END)( handle, false, "(IffIO)"
				, _touchId
				, _point.x
				, _point.y
				, _button
				, pybind::get_bool(_isDown) 
				);
		}

		if( handle == false )
		{
			handle = m_player->handleMouseButtonEventEnd( _touchId, _point, _button, _isDown );
		}	

		return handle;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Game::handleMouseMove( unsigned int _touchId, const mt::vec2f & _point, float _x, float _y )
	{
		bool handle = false;

		if( handle == false )
		{
			EVENTABLE_ASK(m_serviceProvider, this, EVENT_MOUSE_MOVE)( handle, false, "(Iffff)"
				, _touchId
				, _point.x
				, _point.y
				, _x
				, _y 
				);
		}

		if( handle == false )
		{
			handle = m_player->handleMouseMove( _touchId, _point, _x, _y );
		}

		return handle;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Game::handleMouseWheel( unsigned int _touchId, const mt::vec2f & _point, int _wheel )
	{
		bool handle = false;

		if( handle == false )
		{
			EVENTABLE_ASK(m_serviceProvider, this, EVENT_MOUSE_WHEEL)( handle, false, "(Iffi)"
				, _touchId
				, _point.x
				, _point.y
				, _wheel 
				);
		}

		if( handle == false )
		{
			handle = m_player->handleMouseWheel( _touchId, _point, _wheel );
		}

		return handle;
	}
	//////////////////////////////////////////////////////////////////////////
	void Game::mouseLeave()
	{		
		EVENTABLE_CALL(m_serviceProvider, this, EVENT_APP_MOUSE_LEAVE)( "()" );

		m_player->onAppMouseLeave();
	}
	//////////////////////////////////////////////////////////////////////////
	void Game::mouseEnter( const mt::vec2f & _point )
	{
		EVENTABLE_CALL(m_serviceProvider, this, EVENT_APP_MOUSE_ENTER)( "(ff)"
			, _point.x
			, _point.y 
			);

		m_player->onAppMouseEnter();
	}
	//////////////////////////////////////////////////////////////////////////
	void Game::mousePosition( const mt::vec2f & _point )
	{
		m_player->onAppMousePosition( _point );
	}
	//////////////////////////////////////////////////////////////////////////
	bool Game::beginUpdate()
	{
		if( m_player->update() == false )
		{
			return false;
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void Game::tick( float _timing )
	{
		float timing = _timing * m_timingFactor;
		
		//m_amplifierService->update( timing );

		m_player->tick( timing );
	}
	//////////////////////////////////////////////////////////////////////////
	void Game::endUpdate()
	{
		m_player->updateChangeScene();
	}
	//////////////////////////////////////////////////////////////////////////
	void Game::render()
	{
		m_player->render();
	}
    //////////////////////////////////////////////////////////////////////////
    void Game::setEmbed( PyObject * _embed )
    {
        m_embed = _embed;

        pybind::incref( m_embed );

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
        this->registerEventMethod( EVENT_INITIALIZE, "onInitialize", _embed );
        this->registerEventMethod( EVENT_INITIALIZE_RENDER_RESOURCES, "onInitializeRenderResources", _embed );
        this->registerEventMethod( EVENT_ACCOUNT_FINALIZE, "onAccountFinalize", _embed );
        this->registerEventMethod( EVENT_FINALIZE, "onFinalize", _embed );
        this->registerEventMethod( EVENT_DESTROY, "onDestroy", _embed );

        this->registerEventMethod( EVENT_FOCUS, "onFocus", _embed );

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
    PyObject * Game::getEmbed() const
    {
        return m_embed;
    }
	//////////////////////////////////////////////////////////////////////////
	bool Game::loadPersonality( const ConstString & _module )
	{
        SCRIPT_SERVICE(m_serviceProvider)
            ->addGlobalModule( "_DEVELOPMENT", pybind::get_bool(m_developmentMode) );

#   ifdef MENGE_MASTER_RELEASE
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

		PyObject * personality = SCRIPT_SERVICE(m_serviceProvider)
			->importModule( _module );

		if( personality == nullptr )
		{
			return false;
		}

        pybind::incref( personality );

		this->setEmbed( personality );

#	ifdef _DEBUG
		bool is_debug = true;
#	else
		bool is_debug = false;
#	endif
		
        LOGGER_WARNING(m_serviceProvider)("Preparation debug(%d)"
            , is_debug
            );

		bool result = true;
		EVENTABLE_ASK(m_serviceProvider, this, EVENT_PREPARATION)( result, false, "(O)", pybind::get_bool(is_debug) );

		return result;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Game::initialize( const FilePath & _accountPath, size_t _projectVersion, bool _projectVersionCheck, const TMapParams & _params )
	{
		m_params = _params;

		m_player = new Player(this);

        if( SERVICE_REGISTRY( m_serviceProvider, m_player ) == false )
        {
            return false;
        }

		AccountServiceInterface * accountService;
		if( SERVICE_CREATE( AccountService, &accountService ) == false )
        {
            return false;
        }
		
        if( SERVICE_REGISTRY( m_serviceProvider, accountService ) == false )
		{
			return false;
		}

		m_accountService = accountService;

        m_accountProvider = new GameAccountProvider(m_serviceProvider, this);
		
        m_accountService->initialize( _accountPath, _projectVersion, _projectVersionCheck, m_accountProvider );

		bool noLoadAccount = CONFIG_VALUE(m_serviceProvider, "Development", "NoAccount", false);
		
		if( noLoadAccount == false )
		{
			if( m_accountService->loadAccounts() == false )
		    {
			    LOGGER_ERROR(m_serviceProvider)("Game::initialize failed load accounts"
				    );

				//return false;
			}
		}
		
		const Resolution & contentResolution = APPLICATION_SERVICE(m_serviceProvider)
            ->getContentResolution();

		const Resolution & currentResolution = APPLICATION_SERVICE(m_serviceProvider)
            ->getCurrentResolution();

		if( m_player->initialize( contentResolution, currentResolution ) == false )
		{
            LOGGER_ERROR(m_serviceProvider)("Game::initialize failed initialize player"
                );

			return false;
		}

		m_defaultArrow = PROTOTYPE_SERVICE(m_serviceProvider)
			->generatePrototypeT<Arrow>( CONST_STRING(m_serviceProvider, Arrow), CONST_STRING(m_serviceProvider, Default) );

		if( m_defaultArrow == nullptr )
		{
			LOGGER_ERROR(m_serviceProvider)("Game::initialize failed create defaultArrow 'Default'"
				);

			return false;
		}

		m_defaultArrow->setName( CONST_STRING(m_serviceProvider, Default) );

		m_player->setArrow( m_defaultArrow );

		m_soundVolumeProvider = new GameSoundVolumeProvider(m_serviceProvider, this);

		SOUND_SERVICE(m_serviceProvider)
			->addSoundVolumeProvider( m_soundVolumeProvider );

		return true;
	}
    //////////////////////////////////////////////////////////////////////////
    bool Game::run( const String& _scriptInitParams )
    {
        bool isMaster = !m_developmentMode;

        LOGGER_WARNING(m_serviceProvider)("Initialize params '%s' platform = '%s' master = '%d'"
            , _scriptInitParams.c_str()
            , m_platformName.c_str()
            , isMaster
            );


        bool result = false;
        EVENTABLE_ASK(m_serviceProvider, this, EVENT_INITIALIZE)( result, true, "(ssO)", _scriptInitParams.c_str(), m_platformName.c_str(), pybind::get_bool(isMaster) );

        return result;
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
	void Game::finalize()
	{	
        EVENTABLE_CALL(m_serviceProvider, this, EVENT_ACCOUNT_FINALIZE)( "()" );

        if( m_accountService != nullptr )
        {
            m_accountService->finalize();
        }

        EVENTABLE_CALL(m_serviceProvider, this, EVENT_FINALIZE)( "()" );

        if( m_accountService != nullptr )
        {
            SERVICE_DESTROY(AccountService, m_accountService);
            m_accountService = nullptr;
        }

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

		if( m_player != nullptr )
		{
			m_player->finalize();

			delete m_player;
			m_player = nullptr;
		}

		this->destroyArrow();
		      
		m_resourcePaks.clear();
		m_languagePaks.clear();

		m_paks.clear();
				
		EVENTABLE_CALL(m_serviceProvider, this, EVENT_DESTROY)( "()" );
	}
	//////////////////////////////////////////////////////////////////////////
	void Game::initializeRenderResources()
	{
		m_player->initializeRenderResources();

        EVENTABLE_CALL(m_serviceProvider, this, EVENT_INITIALIZE_RENDER_RESOURCES)( "()" );
	}
	//////////////////////////////////////////////////////////////////////////
	void Game::finalizeRenderResources()
	{
		if( m_player != nullptr )
		{
			m_player->finalizeRenderResources();
		}
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
		if( m_player != nullptr )
		{
			m_player->onFocus( _focus );
		}				

		EVENTABLE_CALL(m_serviceProvider, this, EVENT_FOCUS)( "(O)", pybind::get_bool(_focus) );
	}
	//////////////////////////////////////////////////////////////////////////
	bool Game::getHasWindowPanel() const
	{
		return m_hasWindowPanel;
	}
	//////////////////////////////////////////////////////////////////////////
	void Game::setFullscreen( const Resolution & _resolution, bool _fullscreen )
	{
		m_player->onFullscreen( _resolution, _fullscreen );

		EVENTABLE_CALL(m_serviceProvider, this, EVENT_FULLSCREEN)( "(O)", pybind::get_bool(_fullscreen) );
	}
    //////////////////////////////////////////////////////////////////////////
    void Game::setFixedContentResolution( const Resolution & _resolution, bool _fixed )
    {
        m_player->onFixedContentResolution( _resolution, _fixed );

        EVENTABLE_CALL(m_serviceProvider, this, EVENT_FIXED_CONTENT_RESOLUTION)( "(O)", pybind::get_bool(_fixed) );
    }
    //////////////////////////////////////////////////////////////////////////
    void Game::setRenderViewport( const Viewport & _viewport, const Resolution & _contentResolution )
    {
        EVENTABLE_CALL(m_serviceProvider, this, EVENT_RENDER_VIEWPORT)( "(OO)", pybind::ptr(_viewport), pybind::ptr(_contentResolution) );
    }
	//////////////////////////////////////////////////////////////////////////
	void Game::setGameViewport( const Viewport & _viewport, float _aspect )
	{
		EVENTABLE_CALL(m_serviceProvider, this, EVENT_GAME_VIEWPORT)( "(Of)", pybind::ptr(_viewport), _aspect );
	}
	//////////////////////////////////////////////////////////////////////////
	bool Game::close()
	{
		bool needQuit = true;

        EVENTABLE_ASK(m_serviceProvider, this, EVENT_CLOSE)( needQuit, true, "()" );
	
		return needQuit;
	}
    //////////////////////////////////////////////////////////////////////////
    void Game::userEvent( const ConstString & _event, const TMapParams & _params )
    {
        EVENTABLE_CALL(m_serviceProvider, this, EVENT_USER)( "(OO)", pybind::ptr(_event), pybind::ptr(_params) );
    }
	//////////////////////////////////////////////////////////////////////////
	bool Game::loadLocalePaksByName_( const ConstString & _locale, const ConstString & _platform )
	{
		bool hasLocale = false;

		for( TVectorResourcePak::const_iterator 
			it = m_languagePaks.begin(),
			it_end = m_languagePaks.end();
		it != it_end;
		++it )
		{
			const PakPtr & pak = *it;

			const ConstString & pakLocale = pak->getLocale();
			const ConstString & pakPlatform = pak->getPlatfrom();

			if( pakPlatform.empty() == false && pakPlatform != _platform )
			{
				continue;
			}
			
			if( pakLocale != _locale )
			{
				continue;
			}

			m_paks.push_back( pak );

			hasLocale = true;
		}

		return hasLocale;
	}
	//////////////////////////////////////////////////////////////////////////
	void Game::addResourcePak( const ResourcePackDesc & _desc )
	{
#   ifdef MENGE_MASTER_RELEASE
		if( _desc.dev == true )
		{
			return;
		}
#   endif

		PakPtr pack = m_factoryPak.createObjectT();
		
		pack->setup( m_serviceProvider
			, _desc.name
			, _desc.type
			, _desc.locale
			, _desc.platform
			, _desc.descriptionPath
			, _desc.path
			, _desc.preload
			); 

		m_resourcePaks.push_back( pack );
	}
	//////////////////////////////////////////////////////////////////////////
	void Game::addLanguagePak( const ResourcePackDesc & _desc )
	{
#   ifdef MENGE_MASTER_RELEASE
		if( _desc.dev == true )
		{
			return;
		}
#   endif

		PakPtr pack = m_factoryPak.createObjectT();

		pack->setup(
			m_serviceProvider
			, _desc.name
			, _desc.type
			, _desc.locale
			, _desc.platform
			, _desc.descriptionPath
			, _desc.path
			, _desc.preload
			);

		m_languagePaks.push_back( pack );
	}
	//////////////////////////////////////////////////////////////////////////
	bool Game::loadResourcePak( const ResourcePackDesc & _desc )
	{
		PakPtr pack = m_factoryPak.createObjectT();

		pack->setup(
			m_serviceProvider
			, _desc.name
			, _desc.type
			, _desc.locale
			, _desc.platform
			, _desc.descriptionPath
			, _desc.path
			, _desc.preload
			);

		if( pack->load() == false )
		{
			return false;
		}

		if( pack->apply() == false )
		{
			return false;
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Game::applyConfigPaks()
	{
		for( TVectorResourcePak::const_iterator 
			it = m_resourcePaks.begin(),
			it_end = m_resourcePaks.end();
		it != it_end;
		++it )
		{
			const PakPtr & pak = *it;

			const ConstString & pakPlatform = pak->getPlatfrom();

			if( pakPlatform.empty() == false && pakPlatform != m_platformName )
			{
				continue;
			}

			m_paks.push_back( pak );
		}

		if( this->loadLocalePaksByName_( m_languagePak, m_platformName ) == false )			
		{
			if( this->loadLocalePaksByName_( CONST_STRING(m_serviceProvider, eng), m_platformName ) == false )
			{
				if( m_languagePaks.empty() == false )
				{
					const PakPtr & firstPak = m_languagePaks.front();

					const ConstString & pakName = firstPak->getName();

					this->loadLocalePaksByName_( pakName, m_platformName );
				}
				else
				{
					LOGGER_WARNING(m_serviceProvider)("Game::loadConfigPaks not set locale pak"						
						);
				}
			}
		}

		for( TVectorResourcePak::const_iterator 
			it = m_paks.begin(), 
			it_end = m_paks.end();
		it != it_end;
		++it )
		{
			const PakPtr & pak = *it;

			if( pak->isPreload() == false )
			{
				continue;
			}

			if( pak->load() == false )
			{
				return false;
			}
		}

		BEGIN_WATCHDOG(m_serviceProvider, "pak apply");

		for( TVectorResourcePak::const_iterator 
			it = m_paks.begin(), 
			it_end = m_paks.end();
		it != it_end;
		++it )
		{
			const PakPtr & pak = *it;

			if( pak->isPreload() == false )
			{
				continue;
			}

			if( pak->apply() == false )
            {
                return false;
            }
		}

		END_WATCHDOG(m_serviceProvider, "pak apply", 0)("");

        return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void Game::setLanguagePack( const ConstString& _packName )
	{
		m_languagePak = _packName;
	}
    //////////////////////////////////////////////////////////////////////////
    const ConstString & Game::getLanguagePack() const
    {
        return m_languagePak;
    }
	//////////////////////////////////////////////////////////////////////////
	void Game::setCursorMode( bool _mode )
	{
		EVENTABLE_CALL(m_serviceProvider, this, EVENT_CURSOR_MODE)( "(O)", pybind::get_bool(_mode) );
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

		EVENTABLE_CALL(m_serviceProvider, this, EVENT_ON_TIMING_FACTOR)( "(f)", m_timingFactor );
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
	//////////////////////////////////////////////////////////////////////////
}
