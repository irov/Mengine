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

#	include "AccountManager.h"
#	include "ArrowManager.h"
#	include "SceneManager.h"
#	include "ParamManager.h"

#	include "BinParser.h"

#	include "Consts.h"

#	include "ConfigFile.h"
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
			FPakFinder( const ConstString & _pakName )
				: m_pakName( _pakName )
			{
			}

			bool operator()( ResourcePak * _pak )
			{
				return _pak->getName() == m_pakName;
			}

			const ConstString & m_pakName;
		};
	}
	//////////////////////////////////////////////////////////////////////////
	Game::Game( const String & _baseDir )
		: m_baseDir(_baseDir)
		, m_title(Consts::get()->c_Game)
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
			BIN_CASE_ATTRIBUTE( Protocol::Title_Value, m_title );
			BIN_CASE_ATTRIBUTE( Protocol::Title_Localized, m_localizedTitle );

			BIN_CASE_ATTRIBUTE( Protocol::ResourceResolution_Value, m_contentResolution ); //depricated
			BIN_CASE_ATTRIBUTE( Protocol::ContentResolution_Value, m_contentResolution );
			BIN_CASE_ATTRIBUTE( Protocol::FixedContentResolution_Value, m_fixedContentResolution );
			BIN_CASE_ATTRIBUTE( Protocol::PersonalityModule_Value, m_personalityModule );
			BIN_CASE_ATTRIBUTE( Protocol::DefaultArrow_Name, m_defaultArrowName );
			BIN_CASE_ATTRIBUTE( Protocol::DefaultArrow_Prototype, m_defaultArrowPrototype );
			BIN_CASE_ATTRIBUTE( Protocol::Screensaver_Name, m_screensaverName );

			BIN_CASE_ATTRIBUTE( Protocol::Icon_Path, m_iconPath );
			
			BIN_CASE_NODE( Protocol::Window )
			{
				BIN_FOR_EACH_ATTRIBUTES()
				{
					BIN_CASE_ATTRIBUTE( Protocol::Window_Size, m_resolution );
					BIN_CASE_ATTRIBUTE( Protocol::Window_Bits, m_bits );
					BIN_CASE_ATTRIBUTE( Protocol::Window_Fullscreen, m_fullScreen );
					BIN_CASE_ATTRIBUTE( Protocol::Window_HasPanel, m_hasWindowPanel );
					BIN_CASE_ATTRIBUTE( Protocol::Window_VSync, m_vsync );
					BIN_CASE_ATTRIBUTE( Protocol::Window_TextureFiltering, m_textureFiltering );
				}
			}

			BIN_CASE_NODE( Protocol::ResourcePack )
			{
				ResourcePakDesc pak;
				pak.preload = true;
				pak.type = Consts::get()->c_dir;

				BIN_FOR_EACH_ATTRIBUTES()
				{
					BIN_CASE_ATTRIBUTE( Protocol::ResourcePack_Name, pak.name );
					BIN_CASE_ATTRIBUTE( Protocol::ResourcePack_Path, pak.path );
					//BIN_CASE_ATTRIBUTE( Protocol::ResourcePack_Type, pak.type );
					BIN_CASE_ATTRIBUTE( Protocol::ResourcePack_Description, pak.description );
					BIN_CASE_ATTRIBUTE( Protocol::ResourcePack_PreLoad, pak.preload );
				}

				m_resourcePaks.push_back( new ResourcePak(pak, m_baseDir) );
			}

			BIN_CASE_NODE( Protocol::LanguagePack )
			{
				ResourcePakDesc pak;
				pak.preload = true;
				pak.type = Consts::get()->c_dir;

				BIN_FOR_EACH_ATTRIBUTES()
				{
					BIN_CASE_ATTRIBUTE( Protocol::LanguagePack_Name, pak.name );
					BIN_CASE_ATTRIBUTE( Protocol::LanguagePack_Path, pak.path );
					//BIN_CASE_ATTRIBUTE( Protocol::LanguagePack_Type, pak.type );
					BIN_CASE_ATTRIBUTE( Protocol::LanguagePack_Description, pak.description );
					BIN_CASE_ATTRIBUTE( Protocol::LanguagePack_PreLoad, pak.preload );
				}

				m_languagePaks.push_back( new ResourcePak(pak, m_baseDir) );
			}
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void Game::_loaded()
	{
		RenderEngine::get()
			->setVSync( m_vsync );

		const Resolution& dres = Application::get()
			->getMaxClientAreaSize();

		float aspect = m_resolution.getAspectRatio();

		std::size_t resHeight = m_resolution.getHeight();
		std::size_t dresHeight = dres.getHeight();

		if( resHeight > dresHeight )
		{
			std::size_t new_witdh = static_cast<std::size_t>( float(resHeight) * aspect );
			m_resolution.setWidth( new_witdh );			
			m_resolution.setHeight( dresHeight );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	bool Game::handleKeyEvent( const mt::vec2f & _point, unsigned int _key, unsigned int _char, bool _isDown )
	{
		bool handle = false;

		if( !handle )
		{
			//PyObject * pychar = PyBuffer_FromMemory( &_char, sizeof(_char) );
			askEvent( handle, EVENT_KEY, "(IIb)", _key, _char, _isDown );
		}

		if( !handle )
		{
			handle = m_player->handleKeyEvent( _point, _key, _char, _isDown );
		}	

		return handle;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Game::handleMouseButtonEvent( const mt::vec2f & _point, unsigned int _button, bool _isDown )
	{
		bool handle = false;

		if( !handle )
		{
			askEvent( handle, EVENT_MOUSE_BUTTON, "(IO)", _button, pybind::get_bool(_isDown) );
		}

		if( !handle )
		{
			handle = m_player->handleMouseButtonEvent( _point, _button, _isDown );
		}	

		return handle;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Game::handleMouseButtonEventBegin( const mt::vec2f & _point, unsigned int _button, bool _isDown )
	{
		bool handle = false;

		if( !handle )
		{
			askEvent( handle, EVENT_MOUSE_BUTTON_BEGIN, "(IO)", _button, pybind::get_bool(_isDown) );
		}

		if( !handle )
		{
			handle = m_player->handleMouseButtonEventBegin( _point, _button, _isDown );
		}	

		return handle;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Game::handleMouseButtonEventEnd( const mt::vec2f & _point, unsigned int _button, bool _isDown )
	{
		bool handle = false;

		if( !handle )
		{
			askEvent( handle, EVENT_MOUSE_BUTTON_END, "(IO)", _button, pybind::get_bool(_isDown) );
		}

		if( !handle )
		{
			handle = m_player->handleMouseButtonEventEnd( _point, _button, _isDown );
		}	

		return handle;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Game::handleMouseMove( const mt::vec2f & _point, float _x, float _y, int _whell )
	{
		bool handle = false;

		if( !handle )
		{
			askEvent( handle, EVENT_MOUSE_MOVE, "(ffi)", _x, _y, _whell );
		}

		if( !handle )
		{
			handle = m_player->handleMouseMove( _point, _x, _y, _whell );
		}		

		return handle;
	}
	//////////////////////////////////////////////////////////////////////////
	void Game::onAppMouseLeave()
	{
		this->callEvent( EVENT_APP_MOUSE_LEAVE, "()" );

		m_player->onAppMouseLeave();
	}
	//////////////////////////////////////////////////////////////////////////
	void Game::onAppMouseEnter()
	{
		this->callEvent( EVENT_APP_MOUSE_ENTER, "()" );

		m_player->onAppMouseEnter();
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

		m_amplifier->update( _timing );

		m_player->tick( _timing );
	}
	//////////////////////////////////////////////////////////////////////////
	bool Game::render( unsigned int _debugMask )
	{
		m_player->render( _debugMask );

		return m_player->isChangedScene();
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
		if( this->askEvent( result, EVENT_PREPARATION, "(O)", pybind::get_bool(is_debug) ) == false )
		{
			return false;
		}

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

		registerEvent( EVENT_PREPARATION, "onPreparation", _embed );
		registerEvent( EVENT_INITIALIZE, "onInitialize", _embed );
		registerEvent( EVENT_FINALIZE, "onFinalize", _embed );
		registerEvent( EVENT_DESTROY, "onDestroy", _embed );

		registerEvent( EVENT_FOCUS, "onFocus", _embed );

		registerEvent( EVENT_CREATE_ACCOUNT, "onCreateAccount", _embed );

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
			void onCreateAccount( const String & _accountID ) override
			{
				m_game->callEvent( EVENT_CREATE_ACCOUNT, "(s)", _accountID.c_str() );
			}

		protected:
			Game * m_game;
		};
	}
	//////////////////////////////////////////////////////////////////////////
	bool Game::initialize( const String& _scriptInitParams, const String & _platformName )
	{
		m_player = new Player;

		Player::keep( m_player );

		m_amplifier = new Amplifier;

		Amplifier::keep( m_amplifier );
		//m_lightSystem = new LightSystem();//?

		m_homeless = NodeManager::get()
			->createNode( Consts::get()->c_Homeless, Consts::get()->c_Node, Consts::get()->c_builtin_empty );

		m_accountLister = new ApplicationAccountManagerListener(this);

		m_accountManager = new AccountManager(m_accountLister);

		AccountManager::keep(m_accountManager);

		m_accountManager->loadAccounts( "Accounts.ini");
		
		m_defaultArrow = ArrowManager::get()
			->createArrow( m_defaultArrowName, m_defaultArrowPrototype );

		const Resolution & currentResolution = Application::get()
			->getCurrentResolution();

		if( m_player->initialize( m_defaultArrow, m_contentResolution, currentResolution ) == false )
		{
			return false;
		}

		bool result = false;
		if( this->askEvent( result, EVENT_INITIALIZE, "(ss)", _scriptInitParams.c_str(), _platformName.c_str() ) == false )
		{
			return true;
		}

		//const TMapParams & params = ParamManager::get()
		//	->getParams();

		//for( TMapParams::const_iterator
		//	it = params.begin(),
		//	it_end = params.end();
		//it != it_end;
		//++it )
		//{
		//	const TVectorParams & param = it->second;

		//	PyObject * py_param = pybind::ptr( param );

		//	bool result = false;
		//	if( this->askEvent( result, EVENT_PARAMS, "(sO)", it->first.c_str(), py_param ) == false )
		//	{
		//		return true;
		//	}
		//}

		return result;
	}
	//////////////////////////////////////////////////////////////////////////
	void Game::finalize()
	{
		this->callEvent( EVENT_FINALIZE, "()" );

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

		delete m_amplifier;

		if( m_player )
		{
			m_player->finalize();
			delete m_player;
		}
		//delete m_lightSystem;

		delete m_accountManager;

		delete static_cast<ApplicationAccountManagerListener*>(m_accountLister);

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

		this->callEvent( EVENT_DESTROY, "()" );
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
		if( _homeless->getParent() )
		{
			MENGE_LOG_ERROR( "Error: addHomeless '%s' have parent '%s'"
				, _homeless->getName().c_str()
				, _homeless->getParent()->getName().c_str()
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
	const String & Game::getTitle() const
	{
		if( m_localizedTitle == false )
		{
			return Helper::to_str(m_title);
		}

		TextManager * textMgr = TextManager::get();

		if( textMgr == 0 )
		{
			return Helper::to_str(m_title);
		}

		const TextEntry & entry = textMgr->getTextEntry( m_title );

		return entry.text;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Game::isContentResolutionFixed() const
	{
		return m_fixedContentResolution;
	}
	//////////////////////////////////////////////////////////////////////////
	const Resolution & Game::getResolution() const
	{
		return m_resolution;
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
	void Game::onFocus( bool _focus )
	{
		if( m_player != NULL )
		{
			m_player->onFocus( _focus );
		}

		this->callEvent( EVENT_FOCUS, "(O)", pybind::get_bool(_focus) );
	}
	//////////////////////////////////////////////////////////////////////////
	bool Game::getHasWindowPanel() const
	{
		return m_hasWindowPanel;
	}
	//////////////////////////////////////////////////////////////////////////
	void Game::initPredefinedResources_()
	{
		ResourceImageDefault * image = ResourceManager::get()
			->createResourceT<ResourceImageDefault>( 
			Consts::get()->c_builtin_empty, 
			Consts::get()->c_builtin_empty, 
			Consts::get()->c_WhitePixel, 
			Consts::get()->c_ResourceImageDefault 
			);

		image->addImagePath( Consts::get()->c_CreateImage, mt::vec2f(1.f,1.f) );
		image->incrementReference();
	}
	//////////////////////////////////////////////////////////////////////////
	void Game::removePredefinedResources_()
	{
		ResourceManager::get()
			->directResourceRelease( Consts::get()->c_WhitePixel );
	}
	//////////////////////////////////////////////////////////////////////////
	void Game::onFullscreen( const Resolution & _resolution, bool _fullscreen )
	{
		m_player->onFullscreen( _resolution, _fullscreen );

		this->callEvent( EVENT_FULLSCREEN, "(O)", pybind::get_bool(_fullscreen) );
	}
	//////////////////////////////////////////////////////////////////////////
	bool Game::onClose()
	{
		bool needQuit = true;
		if( m_personalityHasOnClose == true )
		{
			askEvent( needQuit, EVENT_CLOSE, "()" );
		}
		return needQuit;
	}
	//////////////////////////////////////////////////////////////////////////
	void Game::setBaseDir( const String& _baseDir )
	{
		m_baseDir = _baseDir;
	}
	//////////////////////////////////////////////////////////////////////////
	void Game::loadConfigPaks()
	{
		m_paks.insert( m_paks.begin(), m_resourcePaks.begin(), m_resourcePaks.end() );

		ResourcePak * languagePak = NULL;

		TVectorResourcePak::iterator it_language_find = 
			std::find_if( m_languagePaks.begin(), m_languagePaks.end(), FPakFinder(m_languagePak) );

		if( it_language_find != m_languagePaks.end() )
		{
			languagePak = *it_language_find;
		}

		if( languagePak == NULL && m_languagePaks.empty() == false )
		{
			languagePak = m_languagePaks.front();
		}

		if( languagePak != NULL )
		{
			m_paks.push_back( languagePak );
		}

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
	void Game::loadPakFromName( const ConstString& _pakName )
	{
		TVectorResourcePak::iterator it_find = 
			std::find_if( m_paks.begin(), m_paks.end(), FPakFinder( _pakName )  );

		if( it_find == m_paks.end() )
		{
			return;
		}

		ResourcePak * pak = (*it_find);

		pak->load();
	}
	//////////////////////////////////////////////////////////////////////////
	const String & Game::getPakPath( const ConstString& _packName ) const
	{
		TVectorResourcePak::const_iterator it_find 
			= std::find_if( m_paks.begin(), m_paks.end(), FPakFinder( _packName ) );

		if( it_find == m_paks.end() )
		{
			return Utils::emptyString();
		}

		return (*it_find)->getPath();
	}
	//////////////////////////////////////////////////////////////////////////
	void Game::setCursorMode( bool _mode )
	{
		if( m_player != NULL )
		{
			m_player->setCursorMode( _mode );
		}
	}

	//////////////////////////////////////////////////////////////////////////
	const String& Game::getScreensaverName() const
	{
		return m_screensaverName;
	}
	//////////////////////////////////////////////////////////////////////////
}
