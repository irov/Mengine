#	include "Game.h"

#	include "Kernel/Scene.h"

#	include "Player.h"
#	include "Arrow.h"
#	include "ResourcePak.h"

#	include "Interface/AmplifierServiceInterface.h"

//#	include "LightSystem.h"
//#	include "ResourceImageDynamic.h"
#	include "ResourceImageDefault.h"

#	include "Interface/ScriptSystemInterface.h"
#	include "Interface/ResourceInterface.h"
#   include "Interface/RenderSystemInterface.h"
#   include "Interface/ParticleSystemInterface.h"
#   include "Interface/AccountInterface.h"
#   include "Interface/NodeInterface.h"
#   include "Interface/TextInterface.h"
#   include "Interface/StringizeInterface.h"

#	include "Logger/Logger.h"

#	include "Consts.h"

#	include "Core/String.h"
#	include "Core/File.h"

#   include "pybind/pybind.hpp"

//////////////////////////////////////////////////////////////////////////
SERVICE_EXTERN( AccountService, Menge::AccountServiceInterface );
//////////////////////////////////////////////////////////////////////////
SERVICE_FACTORY( GameService, Menge::GameServiceInterface, Menge::Game );
//////////////////////////////////////////////////////////////////////////
namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	Game::Game()
		: m_serviceProvider(nullptr)
		, m_developmentMode(false)
		, m_hasWindowPanel(true)
		, m_player(nullptr)
		, m_accountLister(nullptr)
		, m_accountService(nullptr)
		, m_defaultArrow(nullptr)
		, m_timingFactor(1.f)
        , m_embed(nullptr)
	{
	}
	//////////////////////////////////////////////////////////////////////////
	Game::~Game()
	{
        if( m_embed )
        {
            pybind::decref( m_embed );
            m_embed = NULL;
        }
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
    void Game::setBaseDir( const FilePath & _baseDir )
    {
        m_baseDir = _baseDir;
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
	bool Game::handleKeyEvent( const mt::vec2f & _point, unsigned int _key, unsigned int _char, bool _isDown )
	{
		bool handle = false;

		if( handle == false )
		{
			EVENTABLE_ASK(m_serviceProvider, this, EVENT_KEY)( handle, false, "(IIO)", _key, _char, pybind::get_bool(_isDown) );
		}

		if( handle == false )
		{
			handle = m_player->handleKeyEvent( _point, _key, _char, _isDown );
		}	

		return handle;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Game::handleMouseButtonEvent( unsigned int _touchId, const mt::vec2f & _point, unsigned int _button, bool _isDown )
	{
		bool handle = false;

		if( handle == false )
		{
			EVENTABLE_ASK(m_serviceProvider, this, EVENT_MOUSE_BUTTON)( handle, false, "(IIO)", _touchId, _button, pybind::get_bool(_isDown) );
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
			EVENTABLE_ASK(m_serviceProvider, this, EVENT_MOUSE_BUTTON_BEGIN)( handle, false, "(IIO)", _touchId, _button, pybind::get_bool(_isDown) );
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
			EVENTABLE_ASK(m_serviceProvider, this, EVENT_MOUSE_BUTTON_END)( handle, false, "(IIO)", _touchId, _button, pybind::get_bool(_isDown) );
		}

		if( handle == false )
		{
			handle = m_player->handleMouseButtonEventEnd( _touchId, _point, _button, _isDown );
		}	

		return handle;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Game::handleMouseMove( unsigned int _touchId, const mt::vec2f & _point, float _x, float _y, int _whell )
	{
		bool handle = false;

		if( handle == false )
		{
			EVENTABLE_ASK(m_serviceProvider, this, EVENT_MOUSE_MOVE)( handle, false, "(Iffi)", _touchId, _x, _y, _whell );
		}

		if( handle == false )
		{
			handle = m_player->handleMouseMove( _touchId, _point, _x, _y, _whell );
		}

		return handle;
	}
	//////////////////////////////////////////////////////////////////////////
	void Game::onAppMouseLeave()
	{		
		EVENTABLE_CALL(m_serviceProvider, this, EVENT_APP_MOUSE_LEAVE)( "()" );

		m_player->onAppMouseLeave();
	}
	//////////////////////////////////////////////////////////////////////////
	void Game::onAppMouseEnter( const mt::vec2f & _point )
	{
		PyObject * py_point = pybind::ptr(_point);
		EVENTABLE_CALL(m_serviceProvider, this, EVENT_APP_MOUSE_ENTER)( "(O)", py_point );
		pybind::decref(py_point);

		m_player->onAppMouseEnter( _point );
	}
	//////////////////////////////////////////////////////////////////////////
	bool Game::update()
	{
#   ifdef MENGE_PARTICLES
        PARTICLE_SERVICE(m_serviceProvider)
            ->update();
#   endif

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
	bool Game::render()
	{
		m_player->render();

		bool changed = m_player->isChangedScene();

        return changed;
	}
    //////////////////////////////////////////////////////////////////////////
    void Game::setEmbed( PyObject * _embed )
    {
        m_embed = _embed;

        pybind::incref( m_embed );

        this->registerEvent( EVENT_FULLSCREEN, "onFullscreen", _embed );
        this->registerEvent( EVENT_FIXED_CONTENT_RESOLUTION, "onFixedContentResolution", _embed );
        this->registerEvent( EVENT_RENDER_VIEWPORT, "onRenderViewport", _embed );

        this->registerEvent( EVENT_KEY, "onHandleKeyEvent", _embed );
        this->registerEvent( EVENT_MOUSE_BUTTON, "onHandleMouseButtonEvent", _embed );
        this->registerEvent( EVENT_MOUSE_BUTTON_BEGIN, "onHandleMouseButtonEventBegin", _embed );
        this->registerEvent( EVENT_MOUSE_BUTTON_END, "onHandleMouseButtonEventEnd", _embed );
        this->registerEvent( EVENT_MOUSE_MOVE, "onHandleMouseMove", _embed );

        this->registerEvent( EVENT_APP_MOUSE_ENTER, "onAppMouseEnter", _embed );
        this->registerEvent( EVENT_APP_MOUSE_LEAVE, "onAppMouseLeave", _embed );

        this->registerEvent( EVENT_ON_TIMING_FACTOR, "onTimingFactor", _embed );

        this->registerEvent( EVENT_PREPARATION, "onPreparation", _embed );
        this->registerEvent( EVENT_INITIALIZE, "onInitialize", _embed );
        this->registerEvent( EVENT_INITIALIZE_RENDER_RESOURCES, "onInitializeRenderResources", _embed );
        this->registerEvent( EVENT_ACCOUNT_FINALIZE, "onAccountFinalize", _embed );
        this->registerEvent( EVENT_FINALIZE, "onFinalize", _embed );
        this->registerEvent( EVENT_DESTROY, "onDestroy", _embed );

        this->registerEvent( EVENT_FOCUS, "onFocus", _embed );

        this->registerEvent( EVENT_CREATE_ACCOUNT, "onCreateAccount", _embed );
        this->registerEvent( EVENT_DELETE_ACCOUNT, "onDeleteAccount", _embed );
        this->registerEvent( EVENT_SELECT_ACCOUNT, "onSelectAccount", _embed );
        this->registerEvent( EVENT_UNSELECT_ACCOUNT, "onUnselectAccount", _embed );

        this->registerEvent( EVENT_CURSOR_MODE, "onCursorMode", _embed );

        this->registerEvent( EVENT_USER, "onUserEvent", _embed );
        this->registerEvent( EVENT_CLOSE, "onCloseWindow", _embed );
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

		PyObject * personality = SCRIPT_SERVICE(m_serviceProvider)
			->importModule( _module );

		if( personality == 0 )
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
	namespace
	{
		class ApplicationAccountManagerListener
			: public AccountServiceListener
		{
		public:
			ApplicationAccountManagerListener( ServiceProviderInterface * _serviceProvider, Game * _game )
                : m_serviceProvider(_serviceProvider)
				, m_game(_game)
			{
			}

		protected:
			void onCreateAccount( const WString & _accountID ) override
			{
				EVENTABLE_CALL(m_serviceProvider, m_game, EVENT_CREATE_ACCOUNT)( "(u)", _accountID.c_str() );
			}

            void onDeleteAccount( const WString & _accountID ) override
            {
                EVENTABLE_CALL(m_serviceProvider, m_game, EVENT_DELETE_ACCOUNT)( "(u)", _accountID.c_str() );
            }

			void onSelectAccount( const WString & _accountID ) override
			{
				EVENTABLE_CALL(m_serviceProvider, m_game, EVENT_SELECT_ACCOUNT)( "(u)", _accountID.c_str() );
			}

            void onUnselectAccount( const WString & _accountID ) override
            {
                EVENTABLE_CALL(m_serviceProvider, m_game, EVENT_UNSELECT_ACCOUNT)( "(u)", _accountID.c_str() );
            }

		protected:
            ServiceProviderInterface * m_serviceProvider;
			Game * m_game;
		};
	}
	//////////////////////////////////////////////////////////////////////////
	bool Game::initialize( const TMapParams & _params )
	{
		m_params = _params;

		m_player = new Player(this);

        if( SERVICE_REGISTRY( m_serviceProvider, m_player ) == false )
        {
            return false;
        }

		if( createAccountService( &m_accountService ) == false )
        {
            return false;
        }
		
        SERVICE_REGISTRY( m_serviceProvider, m_accountService );

        m_accountLister = new ApplicationAccountManagerListener(m_serviceProvider, this);

        FilePath accountPath = Helper::stringizeString( m_serviceProvider, "accounts.ini" );

        m_accountService->initialize( accountPath, m_accountLister );

		if( m_accountService->loadAccounts() == false )
        {
            LOGGER_ERROR(m_serviceProvider)("Game::initialize failed load accounts"
                );

            //return false;
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

		const Resolution & contentResolution = APPLICATION_SERVICE(m_serviceProvider)
            ->getContentResolution();

		const Resolution & currentResolution = APPLICATION_SERVICE(m_serviceProvider)
            ->getCurrentResolution();

		if( m_player->initialize( m_defaultArrow, contentResolution, currentResolution ) == false )
		{
            LOGGER_ERROR(m_serviceProvider)("Game::initialize failed initialize player"
                );

			return false;
		}

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

        if( m_accountLister != nullptr )
        {
            delete static_cast<ApplicationAccountManagerListener*>(m_accountLister);
            m_accountLister = nullptr;
        }

		if( m_player != nullptr )
		{
			m_player->finalize();

			delete m_player;
			m_player = nullptr;
		}

		this->destroyArrow();
		
		//delete m_lightSystem;        
		for( TVectorResourcePak::iterator
			it = m_resourcePaks.begin(),
			it_end = m_resourcePaks.end();
		it != it_end;
		++it )
		{
			delete *it;
		}

		m_resourcePaks.clear();

		for( TVectorResourcePak::iterator
			it = m_languagePaks.begin(),
			it_end = m_languagePaks.end();
		it != it_end;
		++it )
		{
			delete *it;
		}

		m_languagePaks.clear();

		m_paks.clear();
				
		EVENTABLE_CALL(m_serviceProvider, this, EVENT_DESTROY)( "()" );
	}
	//////////////////////////////////////////////////////////////////////////
	void Game::initializeRenderResources()
	{
		this->initPredefinedResources_();

		m_player->initializeRenderResources();

        EVENTABLE_CALL(m_serviceProvider, this, EVENT_INITIALIZE_RENDER_RESOURCES)( "()" );
	}
	//////////////////////////////////////////////////////////////////////////
	void Game::finalizeRenderResources()
	{
		this->removePredefinedResources_();

		if( m_player != nullptr )
		{
			m_player->finalizeRenderResources();
		}
	}	
	//////////////////////////////////////////////////////////////////////////
	void Game::onTurnSound( bool _turn )
	{
        (void)_turn;
		//if( AMPLIFIER_SERVICE(m_serviceProvider) )
		//{
		//	AMPLIFIER_SERVICE(m_serviceProvider)->onTurnSound( _turn );
		//}
	}
	//////////////////////////////////////////////////////////////////////////
	void Game::onFocus( bool _focus )
	{
		if( m_player != NULL )
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
	void Game::initPredefinedResources_()
	{
		//ResourceImageDefault * image = ResourceManager::get()
		//	->createResourceT<ResourceImageDefault>( 
		//	Consts::get()->c_builtin_empty, 
		//	Consts::get()->c_builtin_empty, 
		//	Consts::get()->c_WhitePixel, 
		//	Consts::get()->c_ResourceImageDefault 
		//	);

		//image->addImagePath( Consts::get()->c_CreateImage, mt::vec2f(1.f,1.f) );
		//image->incrementReference();
	}
	//////////////////////////////////////////////////////////////////////////
	void Game::removePredefinedResources_()
	{
		//ResourceManager::get()
		//	->directResourceRelease( Consts::get()->c_WhitePixel );
	}
	//////////////////////////////////////////////////////////////////////////
	void Game::onFullscreen( const Resolution & _resolution, bool _fullscreen )
	{
		m_player->onFullscreen( _resolution, _fullscreen );

		EVENTABLE_CALL(m_serviceProvider, this, EVENT_FULLSCREEN)( "(O)", pybind::get_bool(_fullscreen) );
	}
    //////////////////////////////////////////////////////////////////////////
    void Game::onFixedContentResolution( const Resolution & _resolution, bool _fixed )
    {
        m_player->onFixedContentResolution( _resolution, _fixed );

        EVENTABLE_CALL(m_serviceProvider, this, EVENT_FIXED_CONTENT_RESOLUTION)( "(O)", pybind::get_bool(_fixed) );
    }
    //////////////////////////////////////////////////////////////////////////
    void Game::onRenderViewport( const Viewport & _viewport, const Resolution & _contentResolution )
    {
        EVENTABLE_CALL(m_serviceProvider, this, EVENT_RENDER_VIEWPORT)( "(OO)", pybind::ptr(_viewport), pybind::ptr(_contentResolution) );
    }
	//////////////////////////////////////////////////////////////////////////
	bool Game::onClose()
	{
		bool needQuit = true;

        EVENTABLE_ASK(m_serviceProvider, this, EVENT_CLOSE)( needQuit, true, "()" );
	
		return needQuit;
	}
    //////////////////////////////////////////////////////////////////////////
    void Game::onUserEvent( const ConstString & _event, const TMapParams & _params )
    {
        EVENTABLE_CALL(m_serviceProvider, this, EVENT_USER)( "(OO)", pybind::ptr(_event), pybind::ptr(_params) );
    }
	//////////////////////////////////////////////////////////////////////////
	bool Game::loadLocalePaksByName_( const ConstString & _locale, const ConstString & _platform )
	{
		bool hasLocale = false;

		for( TVectorResourcePak::iterator 
			it = m_languagePaks.begin(),
			it_end = m_languagePaks.end();
		it != it_end;
		++it )
		{
			ResourcePak * pak = *it;

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
	bool Game::loadConfigPaks( const TVectorResourcePackDesc & _resourcePack, const TVectorResourcePackDesc & _languagePack )
	{
		for( TVectorResourcePackDesc::const_iterator
			it = _resourcePack.begin(),
			it_end = _resourcePack.end();
		it != it_end;
		++it )
		{
			const ResourcePackDesc & desc = *it;

#   ifdef MENGE_MASTER_RELEASE
            if( desc.dev == true )
            {
                continue;
            }
#   endif

			ResourcePak * pack = new ResourcePak( 
                m_serviceProvider
                , desc.name
                , desc.type
                , desc.locale
                , desc.platform
                , desc.description
                , desc.path
                , desc.preload
                , m_baseDir ); 
            
			m_resourcePaks.push_back( pack );
		}

		for( TVectorResourcePackDesc::const_iterator
			it = _languagePack.begin(),
			it_end = _languagePack.end();
		it != it_end;
		++it )
		{
			const ResourcePackDesc & desc = *it;

#   ifdef MENGE_MASTER_RELEASE
            if( desc.dev == true )
            {
                continue;
            }
#   endif

			ResourcePak * pack = new ResourcePak(
                m_serviceProvider
                , desc.name
                , desc.type
                , desc.locale
                , desc.platform
                , desc.description
                , desc.path
                , desc.preload
                , m_baseDir ); 

			m_languagePaks.push_back( pack );
		}
		//m_resourcePaks = _resourcePack;
		//m_languagePaks = _languagePack;

		m_paks.insert( m_paks.begin(), m_resourcePaks.begin(), m_resourcePaks.end() );

		if( this->loadLocalePaksByName_( m_languagePak, m_platformName ) == false )			
		{
			if( this->loadLocalePaksByName_( CONST_STRING(m_serviceProvider, eng), m_platformName ) == false )
			{
				if( m_languagePaks.empty() == false )
				{
					ResourcePak * firstPak = m_languagePaks.front();

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

		for( TVectorResourcePak::iterator 
			it = m_paks.begin(), 
			it_end = m_paks.end();
		it != it_end;
		++it )
		{
			ResourcePak * pak = *it;

			if( pak->isPreload() == false )
			{
				continue;
			}

			if( pak->load() == false )
			{
				return false;
			}
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Game::applyConfigPaks()
	{
		for( TVectorResourcePak::iterator 
			it = m_paks.begin(), 
			it_end = m_paks.end();
		it != it_end;
		++it )
		{
			ResourcePak * pak = *it;

			if( pak->isPreload() == false )
			{
				continue;
			}

			if( pak->apply() == false )
            {
                return false;
            }
		}

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
	const WString & Game::getParam( const ConstString & _paramName )
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
