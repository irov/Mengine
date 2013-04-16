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
#   include "Interface/ArrowInterface.h"
#   include "Interface/SceneInterface.h"
#   include "Interface/NodeInterface.h"
#   include "Interface/TextInterface.h"
#   include "Interface/StringizeInterface.h"

#	include "Logger/Logger.h"

#	include "ConfigLoader.h"

#	include "Consts.h"

#	include "Core/String.h"
#	include "Core/File.h"

#   include "pybind/pybind.hpp"

//////////////////////////////////////////////////////////////////////////
SERVICE_EXTERN( AccountService, Menge::AccountServiceInterface );
//////////////////////////////////////////////////////////////////////////
namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	Game::Game( const String & _baseDir, bool _developmentMode, const String & _platformName )
		: m_baseDir(_baseDir)
		, m_platformName(_platformName)
		, m_developmentMode(_developmentMode)
        , m_serviceProvider(NULL)
		, m_hasWindowPanel(true)
		, m_personalityHasOnClose(false)
		, m_player(NULL)
		, m_accountLister(0)
		, m_accountService(0)
		, m_defaultArrow(0)
		, m_timingFactor(1.f)
        , m_embed(NULL)
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
    Player * Game::getPlayer() const
    {
        return m_player;
    }
	//////////////////////////////////////////////////////////////////////////
	bool Game::handleKeyEvent( const mt::vec2f & _point, unsigned int _key, unsigned int _char, bool _isDown )
	{
		bool handle = false;

		if( !handle )
		{
			//PyObject * pychar = PyBuffer_FromMemory( &_char, sizeof(_char) );
			//askEvent( handle, EVENT_KEY, "(IIO)", _key, _char, pybind::get_bool(_isDown) );
			EVENTABLE_ASK(this, EVENT_KEY)( handle, false, "(IIO)", _key, _char, pybind::get_bool(_isDown) );
		}

		if( !handle )
		{
			handle = m_player->handleKeyEvent( _point, _key, _char, _isDown );
		}	

		return handle;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Game::handleMouseButtonEvent( unsigned int _touchId, const mt::vec2f & _point, unsigned int _button, bool _isDown )
	{
		bool handle = false;

		if( !handle )
		{
			//askEvent( handle, EVENT_MOUSE_BUTTON, "(IIO)", _touchId, _button, pybind::get_bool(_isDown) );
			EVENTABLE_ASK(this, EVENT_MOUSE_BUTTON)( handle, false, "(IIO)", _touchId, _button, pybind::get_bool(_isDown) );
		}

		if( !handle )
		{
			handle = m_player->handleMouseButtonEvent( _touchId, _point, _button, _isDown );
		}

		if( !handle )
		{
			for( TVectorHandlers::iterator
				it = m_handlersMouseButton.begin(),
				it_end = m_handlersMouseButton.end();
			it != it_end;
			++it )
			{
				if( !handle )
				{
					PyObject * py_handle = *it;

					PyObject * py_result = SCRIPT_SERVICE(m_serviceProvider)
						->askFunction( py_handle, "(IO)", _button, pybind::get_bool(_isDown) );

					if( pybind::is_none(py_result) == true )
					{
						continue;
					}

					if( pybind::bool_check(py_result) == false )
					{
						continue;
					}

					handle = pybind::extract<bool>(py_handle);
				}
			}
		}

		return handle;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Game::handleMouseButtonEventBegin( unsigned int _touchId, const mt::vec2f & _point, unsigned int _button, bool _isDown )
	{
		bool handle = false;

		if( !handle )
		{
			//askEvent( handle, EVENT_MOUSE_BUTTON_BEGIN, "(IIO)", _touchId, _button, pybind::get_bool(_isDown) );
			EVENTABLE_ASK(this, EVENT_MOUSE_BUTTON_BEGIN)( handle, false, "(IIO)", _touchId, _button, pybind::get_bool(_isDown) );
		}

		if( !handle )
		{
			handle = m_player->handleMouseButtonEventBegin( _touchId, _point, _button, _isDown );
		}	

		return handle;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Game::handleMouseButtonEventEnd( unsigned int _touchId, const mt::vec2f & _point, unsigned int _button, bool _isDown )
	{
		bool handle = false;

		if( !handle )
		{
			//askEvent( handle, EVENT_MOUSE_BUTTON_END, "(IIO)", _touchId, _button, pybind::get_bool(_isDown) );
			EVENTABLE_ASK(this, EVENT_MOUSE_BUTTON_END)( handle, false, "(IIO)", _touchId, _button, pybind::get_bool(_isDown) );
		}

		if( !handle )
		{
			handle = m_player->handleMouseButtonEventEnd( _touchId, _point, _button, _isDown );
		}	

		return handle;
	}
	//////////////////////////////////////////////////////////////////////////
	template<class C>
	static void s_addHandler( C & _container, PyObject * _cb )
	{
		pybind::incref( _cb );

		_container.push_back( _cb );
	}	
	//////////////////////////////////////////////////////////////////////////
	template<class C>
	static bool s_removeHandler( C & _container, PyObject * _cb )
	{
		typename C::iterator it_found = std::find( _container.begin(), _container.end(), _cb );

		if( it_found == _container.end() )
		{
			return false;
		}

		PyObject * cb = *it_found;

		pybind::decref( cb );

		_container.erase( it_found );

		return true;
	}	
	//////////////////////////////////////////////////////////////////////////
	void Game::addMouseMoveHandler( PyObject * _cb )
	{
		s_addHandler( m_handlersMouseMove, _cb );
	}
	//////////////////////////////////////////////////////////////////////////
	bool Game::removeMouseMoveHandler( PyObject * _cb )
	{
		return s_removeHandler( m_handlersMouseMove, _cb );
	}
	//////////////////////////////////////////////////////////////////////////
	void Game::addMouseButtonHandler( PyObject * _cb )
	{
		s_addHandler( m_handlersMouseButton, _cb );
	}
	//////////////////////////////////////////////////////////////////////////
	bool Game::removeMouseButtonHandler( PyObject * _cb )
	{
		return s_removeHandler( m_handlersMouseButton, _cb );
	}
	//////////////////////////////////////////////////////////////////////////
	bool Game::handleMouseMove( unsigned int _touchId, const mt::vec2f & _point, float _x, float _y, int _whell )
	{
		bool handle = false;

		if( !handle )
		{
			//askEvent( handle, EVENT_MOUSE_MOVE, "(Iffi)", _touchId, _x, _y, _whell );
			EVENTABLE_ASK(this, EVENT_MOUSE_MOVE)( handle, false, "(Iffi)", _touchId, _x, _y, _whell );
		}

		if( !handle )
		{
			handle = m_player->handleMouseMove( _touchId, _point, _x, _y, _whell );
		}

		if( !handle )
		{
			for( TVectorHandlers::iterator
				it = m_handlersMouseMove.begin(),
				it_end = m_handlersMouseMove.end();
			it != it_end;
			++it )
			{
				if( !handle )
				{
					PyObject * py_handle = *it;

					PyObject * py_result = SCRIPT_SERVICE(m_serviceProvider)
						->askFunction( py_handle, "(ffi)", _x, _y, _whell );

					if( pybind::is_none(py_result) == true )
					{
						continue;
					}

					if( pybind::bool_check(py_result) == false )
					{
						continue;
					}

					handle = pybind::extract<bool>(py_result);
				}
			}
		}

		return handle;
	}
	//////////////////////////////////////////////////////////////////////////
	void Game::onAppMouseLeave()
	{		
		EVENTABLE_CALL(this, EVENT_APP_MOUSE_LEAVE)( "()" );

		m_player->onAppMouseLeave();
	}
	//////////////////////////////////////////////////////////////////////////
	void Game::onAppMouseEnter( const mt::vec2f & _point )
	{
		PyObject * py_point = pybind::ptr(_point);
		EVENTABLE_CALL(this, EVENT_APP_MOUSE_ENTER)( "(O)", py_point );
		pybind::decref(py_point);

		m_player->onAppMouseEnter( _point );
	}
	//////////////////////////////////////////////////////////////////////////
	bool Game::update()
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
		PARTICLE_SERVICE(m_serviceProvider)
			->beginParticlesCount();
				
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
        this->registerEvent( EVENT_FINALIZE, "onFinalize", _embed );
        this->registerEvent( EVENT_DESTROY, "onDestroy", _embed );

        this->registerEvent( EVENT_FOCUS, "onFocus", _embed );

        this->registerEvent( EVENT_CREATE_ACCOUNT, "onCreateAccount", _embed );
        this->registerEvent( EVENT_DELETE_ACCOUNT, "onDeleteAccount", _embed );
        this->registerEvent( EVENT_SELECT_ACCOUNT, "onSelectAccount", _embed );
        this->registerEvent( EVENT_UNSELECT_ACCOUNT, "onUnselectAccount", _embed );

        this->registerEvent( EVENT_CURSOR_MODE, "onCursorMode", _embed );

        this->registerEvent( EVENT_CLOSE, "onCloseWindow", _embed, &m_personalityHasOnClose );
    }
    //////////////////////////////////////////////////////////////////////////
    PyObject * Game::getEmbed() const
    {
        return m_embed;
    }
	//////////////////////////////////////////////////////////////////////////
	bool Game::loadPersonality( const ConstString & _module )
	{
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

        //return false;
		bool result = true;
		EVENTABLE_ASK(this, EVENT_PREPARATION)( result, false, "(O)", pybind::get_bool(is_debug) );

		return result;
	}
	//////////////////////////////////////////////////////////////////////////
	namespace
	{
		class ApplicationAccountManagerListener
			: public AccountServiceListener
		{
		public:
			ApplicationAccountManagerListener( Game * _game )
				: m_game(_game)
			{
			}

		protected:
			void onCreateAccount( const WString & _accountID ) override
			{
				EVENTABLE_CALL(m_game, EVENT_CREATE_ACCOUNT)( "(u)", _accountID.c_str() );
			}

            void onDeleteAccount( const WString & _accountID ) override
            {
                EVENTABLE_CALL(m_game, EVENT_DELETE_ACCOUNT)( "(u)", _accountID.c_str() );
            }

			void onSelectAccount( const WString & _accountID ) override
			{
				EVENTABLE_CALL(m_game, EVENT_SELECT_ACCOUNT)( "(u)", _accountID.c_str() );
			}

            void onUnselectAccount( const WString & _accountID ) override
            {
                EVENTABLE_CALL(m_game, EVENT_UNSELECT_ACCOUNT)( "(u)", _accountID.c_str() );
            }

		protected:
			Game * m_game;
		};
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
	bool Game::initialize( const TMapWString & _params )
	{
		m_params = _params;

		m_player = new Player(this);

        m_serviceProvider->registryService("PlayerService", m_player );		

		if( createAccountService( &m_accountService ) == false )
        {
            return false;
        }
		
        SERVICE_REGISTRY( m_serviceProvider, m_accountService );

        m_accountLister = new ApplicationAccountManagerListener(this);

        FilePath accountPath = Helper::stringizeString( m_serviceProvider, "accounts.ini" );

        m_accountService->initialize( accountPath, m_accountLister );

		if( m_accountService->loadAccountsInfo() == false )
        {
            LOGGER_ERROR(m_serviceProvider)("Game::initialize failed load accounts"
                );

            return false;
        }
		
		m_defaultArrow = ARROW_SERVICE(m_serviceProvider)
            ->createArrow( CONST_STRING(m_serviceProvider, Default) );

        if( m_defaultArrow == NULL )
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
        EVENTABLE_ASK(this, EVENT_INITIALIZE)( result, true, "(ssO)", _scriptInitParams.c_str(), m_platformName.c_str(), pybind::get_bool(isMaster) );

        return result;
    }
	//////////////////////////////////////////////////////////////////////////
	void Game::destroyArrow()
	{
		if( m_defaultArrow )
		{
			m_defaultArrow->destroy();
			m_defaultArrow = NULL;
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void Game::finalize()
	{	
        if( m_accountService != NULL )
        {
            m_accountService->finalize();
        }

        EVENTABLE_CALL(this, EVENT_FINALIZE)( "()" );

        if( m_accountService != NULL )
        {
            destroyAccountService( m_accountService );
            m_accountService = NULL;
        }

        if( m_accountLister != NULL )
        {
            delete static_cast<ApplicationAccountManagerListener*>(m_accountLister);
            m_accountLister = NULL;
        }

		if( m_player != NULL )
		{
			m_player->finalize();
			delete m_player;
			m_player = NULL;
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
				
		EVENTABLE_CALL(this, EVENT_DESTROY)( "()" );
	}
	//////////////////////////////////////////////////////////////////////////
	void Game::initializeRenderResources()
	{
		this->initPredefinedResources_();

		m_player->initializeRenderResources();

        EVENTABLE_CALL(this, EVENT_INITIALIZE_RENDER_RESOURCES)( "()" );
	}
	//////////////////////////////////////////////////////////////////////////
	void Game::finalizeRenderResources()
	{
		this->removePredefinedResources_();

		if( m_player )
		{
			m_player->finalizeRenderResources();
		}
	}	
	//////////////////////////////////////////////////////////////////////////
	void Game::onTurnSound( bool _turn )
	{
		if( AMPLIFIER_SERVICE(m_serviceProvider) )
		{
			AMPLIFIER_SERVICE(m_serviceProvider)->onTurnSound( _turn );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void Game::onFocus( bool _focus )
	{
		if( m_player != NULL )
		{
			m_player->onFocus( _focus );
		}				

		EVENTABLE_CALL(this, EVENT_FOCUS)( "(O)", pybind::get_bool(_focus) );
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

		EVENTABLE_CALL(this, EVENT_FULLSCREEN)( "(O)", pybind::get_bool(_fullscreen) );
	}
    //////////////////////////////////////////////////////////////////////////
    void Game::onFixedContentResolution( const Resolution & _resolution, bool _fixed )
    {
        m_player->onFixedContentResolution( _resolution, _fixed );

        EVENTABLE_CALL(this, EVENT_FIXED_CONTENT_RESOLUTION)( "(O)", pybind::get_bool(_fixed) );
    }
	//////////////////////////////////////////////////////////////////////////
	bool Game::onClose()
	{
		bool needQuit = true;

		if( m_personalityHasOnClose == true )
		{
			//askEvent( needQuit, EVENT_CLOSE, "()" );
			EVENTABLE_ASK(this, EVENT_CLOSE)( needQuit, true, "()" );
		}

		return needQuit;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Game::loadLocalePaksByName_( const ConstString & _locale, const String & _platform )
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
			const String & pakPlatform = pak->getPlatfrom();

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

			ResourcePak * pack = new ResourcePak( 
                m_serviceProvider
                , Helper::stringizeString(m_serviceProvider, desc.name)
                , Helper::stringizeString(m_serviceProvider, desc.type)
                , Helper::stringizeString(m_serviceProvider, desc.locale)
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

			ResourcePak * pack = new ResourcePak(
                m_serviceProvider
                , Helper::stringizeString(m_serviceProvider, desc.name)
                , Helper::stringizeString(m_serviceProvider, desc.type)
                , Helper::stringizeString(m_serviceProvider, desc.locale)
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

		//if( it_language_find != m_languagePaks.end() )
		//{
		//	languagePak = *it_language_find;
		//}

		//if( languagePak == NULL && m_languagePaks.empty() == false )
		//{
		//	languagePak = m_languagePaks.front();
		//}

		//if( languagePak != NULL )
		//{
		//	m_paks.push_back( languagePak );
		//}

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
		EVENTABLE_CALL(this, EVENT_CURSOR_MODE)( "(O)", pybind::get_bool(_mode) );
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

		EVENTABLE_CALL(this, EVENT_ON_TIMING_FACTOR)( "(f)", m_timingFactor );
	}
	//////////////////////////////////////////////////////////////////////////
	const WString & Game::getParam( const WString & _paramName )
	{
		TMapWString::iterator it_find = m_params.find( _paramName );

        const WString & param = it_find->second;

		return param;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Game::hasParam( const WString & _paramName ) const
	{
		TMapWString::const_iterator it_find = m_params.find( _paramName );

		if( it_find == m_params.end() )
		{
			return false;
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
}
