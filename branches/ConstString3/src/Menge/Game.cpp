#	include "Game.h"

#	include "Scene.h"
#	include "Player.h"
#	include "Arrow.h"
#	include "Amplifier.h"
#	include "ResourcePak.h"

#	include "LightSystem.h"
//#	include "ResourceImageDynamic.h"
#	include "ResourceImageDefault.h"

#	include "ScriptEngine.h"
#	include "FileEngine.h"
#	include "ResourceManager.h"
#	include "LogEngine.h"
#	include "RenderEngine.h"
#	include "ParticleEngine.h"
#	include "LoaderEngine.h"

#	include "AccountManager.h"
#	include "ArrowManager.h"
#	include "SceneManager.h"

#	include "BinParser.h"
#	include "ConfigLoader.h"

#	include "Consts.h"

#	include "TextManager.h"

#	include "NodeManager.h"
#	include "Application.h"

#	include "Core/String.h"
#	include "Core/File.h"

//////////////////////////////////////////////////////////////////////////
namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	namespace
	{
		struct FPakFinder
		{
			FPakFinder( const String & _pakName )
				: m_pakName(_pakName)
			{
			}

			bool operator()( ResourcePak * _pak ) const
			{
				const ConstString & pakName = _pak->getName();
				return pakName == m_pakName;
			}

			const String & m_pakName;
		};
	}
	//////////////////////////////////////////////////////////////////////////
	Game::Game( const WString & _baseDir, bool _developmentMode, const String & _platformName )
		: m_baseDir(_baseDir)
		, m_platformName(_platformName)
		, m_developmentMode(_developmentMode)
		, m_fixedContentResolution(false)
		, m_fullScreen(true)
		, m_textureFiltering(true)
		, m_FSAAType(0)
		, m_FSAAQuality(0)
		, m_hasWindowPanel(true)
		, m_vsync(false)
		, m_localizedTitle(false)
		, m_personalityHasOnClose(false)
		, m_player(NULL)
		, m_amplifier(NULL)
		, m_accountLister(0)
		, m_accountManager(0)
		, m_homeless(0)
		, m_defaultArrow(0)
		, m_timingFactor(1.f)
	{
	}
	//////////////////////////////////////////////////////////////////////////
	Game::~Game()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	void Game::loader( BinParser * _parser )
	{        
		BIN_SWITCH_ID( _parser )
		{
			BIN_CASE_NODE( Protocol::ResourcePack )
			{
				ResourcePakDesc pak_desc;
				pak_desc.preload = true;
				pak_desc.type = Consts::get()->c_dir;
                
                BIN_FOR_EACH_ATTRIBUTES()
                {
                    BIN_CASE_ATTRIBUTE( Protocol::ResourcePack_Type, pak_desc.type );
					BIN_CASE_ATTRIBUTE( Protocol::ResourcePack_Name, pak_desc.name );
                    BIN_CASE_ATTRIBUTE( Protocol::ResourcePack_Path, pak_desc.path );
					BIN_CASE_ATTRIBUTE( Protocol::ResourcePack_Locale, pak_desc.locale );
                    BIN_CASE_ATTRIBUTE( Protocol::ResourcePack_Description, pak_desc.filename );
                    BIN_CASE_ATTRIBUTE( Protocol::ResourcePack_PreLoad, pak_desc.preload );
                }
               
                ResourcePak * pak = new ResourcePak( pak_desc, m_baseDir );

				m_resourcePaks.push_back( pak );
			}

			BIN_CASE_NODE( Protocol::LanguagePack )
			{
				ResourcePakDesc pak_desc;
				pak_desc.preload = true;
				pak_desc.type = Consts::get()->c_dir;

				BIN_FOR_EACH_ATTRIBUTES()
				{
					BIN_CASE_ATTRIBUTE( Protocol::LanguagePack_Name, pak_desc.name );
					BIN_CASE_ATTRIBUTE( Protocol::LanguagePack_Type, pak_desc.type );
					BIN_CASE_ATTRIBUTE( Protocol::LanguagePack_Path, pak_desc.path );
					BIN_CASE_ATTRIBUTE( Protocol::LanguagePack_Locale, pak_desc.locale );
					BIN_CASE_ATTRIBUTE( Protocol::LanguagePack_Platform, pak_desc.platform );
					BIN_CASE_ATTRIBUTE( Protocol::LanguagePack_Description, pak_desc.filename );
					BIN_CASE_ATTRIBUTE( Protocol::LanguagePack_PreLoad, pak_desc.preload );
				}

				ResourcePak * pak = new ResourcePak(pak_desc, m_baseDir);

				m_languagePaks.push_back( pak );
			}
		}
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

					PyObject * py_result = ScriptEngine::get()
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

					PyObject * py_result = ScriptEngine::get()
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
		ParticleEngine::get()
			->beginParticlesCount();
				
		float timing = _timing * m_timingFactor;
		
		m_amplifier->update( timing );

		m_player->tick( timing );
	}
	//////////////////////////////////////////////////////////////////////////
	bool Game::render( unsigned int _debugMask )
	{
		m_player->render( _debugMask );

		return m_player->isChangedScene();
	}
	//////////////////////////////////////////////////////////////////////////
	bool Game::loadDescription( const ConstString & _gamePackName, const WString & _gameDescription )
	{
		m_gamePackName = _gamePackName;

		ServiceProviderInterface * serviceProvider = Application::get()
			->getServiceProvider();

		ConfigLoader cfg(serviceProvider);

		if( cfg.loadFile( m_gamePackName, _gameDescription ) == false )
		{
			MENGE_LOG_ERROR("Game::loadDescription invalid configure game %s:%S"
				, _gamePackName.c_str()
				, _gameDescription.c_str()
				);

			return false;
		}
		
		cfg.getSetting( L"Project", L"Name", m_projectName );
		
		cfg.getSetting( L"Game", L"Title", m_title );
		cfg.getSetting( L"Game", L"Localized", m_localizedTitle );
		cfg.getSetting( L"Game", L"ContentResolution", m_contentResolution );
		cfg.getSetting( L"Game", L"LowContentViewport", m_lowContentViewport );
		cfg.getSetting( L"Game", L"FixedContentResolution", m_fixedContentResolution );
		cfg.getSetting( L"Game", L"PersonalityModule", m_personalityModule );

		cfg.getSetting( L"DefaultArrow", L"Name", m_defaultArrowName );
		cfg.getSetting( L"DefaultArrow", L"Prototype", m_defaultArrowPrototype );

		cfg.getSetting( L"Window", L"Size", m_windowResolution );
		cfg.getSetting( L"Window", L"Bits", m_bits );
		cfg.getSetting( L"Window", L"Fullscreen", m_fullScreen );
		cfg.getSetting( L"Window", L"VSync", m_vsync );
		cfg.getSetting( L"Window", L"Icon", m_iconPath );


		cfg.getSetting( L"ResourcePack", L"File", m_resourcePakFile );

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Game::loadPersonality()
	{
		PyObject * personality = ScriptEngine::get()
			->importModule( m_personalityModule );

		if( personality == 0 )
		{
			return false;
		}

		this->setEmbed( personality );

		//pybind::decref( personality );

#	ifdef _DEBUG
		bool is_debug = true;
#	else
		bool is_debug = false;
#	endif
		
		bool result = true;
		//if( this->askEvent( result, EVENT_PREPARATION, "(O)", pybind::get_bool(is_debug) ) == false )
		//{
		//	return false;
		//}

		EVENTABLE_ASK(this, EVENT_PREPARATION)( result, false, "(O)", pybind::get_bool(is_debug) );

		return result;
	}
	//////////////////////////////////////////////////////////////////////////
	PyObject * Game::_embedded()
	{
		return 0;
	}
	//////////////////////////////////////////////////////////////////////////
	void Game::_embedding( PyObject * _embed )
	{	
		registerEvent( EVENT_FULLSCREEN, "onFullscreen", _embed );

		registerEvent( EVENT_KEY, "onHandleKeyEvent", _embed );
		registerEvent( EVENT_MOUSE_BUTTON, "onHandleMouseButtonEvent", _embed );
		registerEvent( EVENT_MOUSE_BUTTON_BEGIN, "onHandleMouseButtonEventBegin", _embed );
		registerEvent( EVENT_MOUSE_BUTTON_END, "onHandleMouseButtonEventEnd", _embed );
		registerEvent( EVENT_MOUSE_MOVE, "onHandleMouseMove", _embed );

		registerEvent( EVENT_APP_MOUSE_ENTER, "onAppMouseEnter", _embed );
		registerEvent( EVENT_APP_MOUSE_LEAVE, "onAppMouseLeave", _embed );

		registerEvent( EVENT_ON_TIMING_FACTOR, "onTimingFactor", _embed );

		registerEvent( EVENT_PREPARATION, "onPreparation", _embed );
		registerEvent( EVENT_INITIALIZE, "onInitialize", _embed );
		registerEvent( EVENT_FINALIZE, "onFinalize", _embed );
		registerEvent( EVENT_DESTROY, "onDestroy", _embed );

		registerEvent( EVENT_FOCUS, "onFocus", _embed );

		registerEvent( EVENT_CREATE_ACCOUNT, "onCreateAccount", _embed );

		registerEvent( EVENT_CURSOR_MODE, "onCursorMode", _embed );
		

		registerEvent( EVENT_CLOSE, "onCloseWindow", _embed, &m_personalityHasOnClose );
	}
	//////////////////////////////////////////////////////////////////////////
	namespace
	{
		class ApplicationAccountManagerListener
			: public AccountManagerListener
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

		protected:
			Game * m_game;
		};
	}
	//////////////////////////////////////////////////////////////////////////
	bool Game::initialize( const String& _scriptInitParams )
	{
		m_player = new Player;

		Player::keep( m_player );

		m_amplifier = new Amplifier;

		Amplifier::keep( m_amplifier );
		//m_lightSystem = new LightSystem();//?

		m_homeless = NodeManager::get()
			->createNode( Consts::get()->c_Homeless, Consts::get()->c_Node, Consts::get()->c_builtin_empty );

		m_accountLister = new ApplicationAccountManagerListener(this);

		m_accountManager = new AccountManager( L"accounts.ini", m_accountLister );

		AccountManager::keep(m_accountManager);

		m_accountManager->loadAccountsInfo();
		
		m_defaultArrow = ArrowManager::get()
			->createArrow( m_defaultArrowName, m_defaultArrowPrototype );

		const Resolution & currentResolution = Application::get()
			->getCurrentResolution();

		if( m_player->initialize( m_defaultArrow, m_contentResolution, currentResolution ) == false )
		{
			return false;
		}

		bool isMaster = !m_developmentMode;

		bool result = false;
		//if( this->askEvent( result, EVENT_INITIALIZE, "(ssO)", _scriptInitParams.c_str(), m_platformName.c_str(), pybind::get_bool(isMaster) ) == false )
		//{
		//	return true;
		//}

		EVENTABLE_ASK(this, EVENT_INITIALIZE)( result, true, "(ssO)", _scriptInitParams.c_str(), m_platformName.c_str(), pybind::get_bool(isMaster) );

		return result;
	}
	//////////////////////////////////////////////////////////////////////////
	void Game::finalize()
	{
		EVENTABLE_CALL(this, EVENT_FINALIZE)( "()" );

		if( m_defaultArrow )
		{
			m_defaultArrow->destroy();
			m_defaultArrow = NULL;
		}		
		
		if( m_homeless )
		{
			m_homeless->destroy();
			m_homeless = NULL;
		}

		if( m_amplifier )
		{
			delete m_amplifier;
			m_amplifier = NULL;
		}		

		if( m_player )
		{
			m_player->finalize();
			delete m_player;
		}
		//delete m_lightSystem;

		if( m_accountManager != NULL )
		{
			m_accountManager->saveAccountsInfo();

			delete m_accountManager;
		}

		if( m_accountLister != NULL )
		{
			delete static_cast<ApplicationAccountManagerListener*>(m_accountLister);
			m_accountLister = NULL;
		}

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
	void Game::addHomeless( Node * _homeless )
	{
		if( _homeless->hasParent() == true )
		{
			Node * parent = _homeless->getParent();

			MENGE_LOG_ERROR( "Error: addHomeless '%s' have parent '%s'"
				, _homeless->getName().c_str()
				, parent->getName().c_str()
				);

			return;
		}

		m_homeless->addChildren( _homeless );
	}
	//////////////////////////////////////////////////////////////////////////
	void Game::clearHomeless()
	{
		m_homeless->destroyAllChild();
	}
	//////////////////////////////////////////////////////////////////////////
	const Resolution & Game::getContentResolution() const
	{
		return m_contentResolution;
	}
	//////////////////////////////////////////////////////////////////////////
	const WString & Game::getTitle() const
	{
		if( m_localizedTitle == false )
		{
			return m_title;
		}

		TextManager * textMgr = TextManager::get();

		if( textMgr == 0 )
		{
			return m_title;
		}

		ConstString key("APPLICATION_TITLE");
		const TextEntry & entry = textMgr->getTextEntry(key);

		return entry.text;
	}
	//////////////////////////////////////////////////////////////////////////
	const ConstString & Game::getProjectName() const
	{
		return m_projectName;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Game::isContentResolutionFixed() const
	{
		return m_fixedContentResolution;
	}
	//////////////////////////////////////////////////////////////////////////
	const Viewport & Game::getLowContentViewport() const
	{
		return m_lowContentViewport;
	}
	//////////////////////////////////////////////////////////////////////////
	const Resolution & Game::getWindowResolution() const
	{
		return m_windowResolution;
	}
	//////////////////////////////////////////////////////////////////////////
	int Game::getBits() const
	{
		return m_bits;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Game::getFullscreen() const
	{
		return m_fullScreen;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Game::getVSync() const
	{
		return m_vsync;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Game::getTextureFiltering() const
	{
		return m_textureFiltering;
	}
	//////////////////////////////////////////////////////////////////////////
	int Game::getFSAAType() const
	{
		return m_FSAAType;
	}
	//////////////////////////////////////////////////////////////////////////
	int Game::getFSAAQuality() const
	{
		return m_FSAAQuality;
	}
	//////////////////////////////////////////////////////////////////////////
	void Game::onTurnSound( bool _turn )
	{
		if( m_amplifier )
		{
			m_amplifier->onTurnSound( _turn );
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
	bool Game::loadConfigPaks()
	{
		bool exist = false;

		if( LoaderEngine::get()
			->load( m_gamePackName, m_resourcePakFile, this, exist ) == false )
		{
			MENGE_LOG_ERROR( "Game::loadConfigPaks failed to load GamePak %s:%S"
				, m_gamePackName.c_str() 
				, m_resourcePakFile.c_str()
				);

			return false;
		}

		m_paks.insert( m_paks.begin(), m_resourcePaks.begin(), m_resourcePaks.end() );

		if( this->loadLocalePaksByName_( m_languagePak, m_platformName ) == false )			
		{
			ConstString c_eng("eng");
			if( this->loadLocalePaksByName_( c_eng, m_platformName ) == false )
			{
				if( m_languagePaks.empty() == false )
				{
					ResourcePak * firstPak = m_languagePaks.front();

					const ConstString & pakName = firstPak->getName();

					this->loadLocalePaksByName_( pakName, m_platformName );
				}
				else
				{
					MENGE_LOG_WARNING("Game::loadConfigPaks '%S' not set locale pak"
						, m_title.c_str()
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

			if( pak->preload() == false )
			{
				continue;
			}

			pak->load();
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void Game::applyConfigPaks()
	{
		for( TVectorResourcePak::iterator 
			it = m_paks.begin(), 
			it_end = m_paks.end();
		it != it_end;
		++it )
		{
			ResourcePak * pak = *it;

			if( pak->preload() == false )
			{
				continue;
			}

			pak->apply();
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void Game::setLanguagePack( const ConstString& _packName )
	{
		m_languagePak = _packName;
	}
	//////////////////////////////////////////////////////////////////////////
	void Game::setCursorMode( bool _mode )
	{
		EVENTABLE_CALL(this, EVENT_CURSOR_MODE)( "(O)", pybind::get_bool(_mode) );
	}
	//////////////////////////////////////////////////////////////////////////
	const WString& Game::getScreensaverName() const
	{
		return m_screensaverName;
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
		EVENTABLE_CALL(this, EVENT_ON_TIMING_FACTOR)( "(f)", _timingFactor );
	}
	//////////////////////////////////////////////////////////////////////////
}
