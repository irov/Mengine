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
#	include "Logger/Logger.h"
#	include "RenderEngine.h"
#	include "ParticleEngine.h"

#	include "AccountManager.h"
#	include "ArrowManager.h"
#	include "SceneManager.h"

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
	Game::Game()
		: m_defaultArrow(0)
		, m_pyPersonality(0)
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
		, m_accountManager(0)
		//, m_lightSystem(NULL)
	{
		m_player = new Player();

		Player::keep( m_player );

		m_amplifier = new Amplifier();
		
		Amplifier::keep( m_amplifier );
		//m_lightSystem = new LightSystem();//?

		m_homeless = NodeManager::get()
			->createNode( Consts::get()->c_Homeless, Consts::get()->c_Node, Consts::get()->c_builtin_empty );
	}
	//////////////////////////////////////////////////////////////////////////
	Game::~Game()
	{
		release();

		delete m_homeless;

		delete m_amplifier;
		delete m_player;
		//delete m_lightSystem;

		delete m_accountManager;
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

				BIN_FOR_EACH_ATTRIBUTES()
				{
					BIN_CASE_ATTRIBUTE( Protocol::ResourcePack_Name, pak.name );
					BIN_CASE_ATTRIBUTE( Protocol::ResourcePack_Path, pak.path );
					BIN_CASE_ATTRIBUTE( Protocol::ResourcePack_Description, pak.description );
					BIN_CASE_ATTRIBUTE( Protocol::ResourcePack_PreLoad, pak.preload );
				}

				m_paks.push_back( new ResourcePak(pak, m_baseDir) );
			}

			BIN_CASE_NODE( Protocol::LanguagePack )
			{
				ResourcePakDesc pak;
				pak.preload = true;

				BIN_FOR_EACH_ATTRIBUTES()
				{
					BIN_CASE_ATTRIBUTE( Protocol::LanguagePack_Name, pak.name );
					BIN_CASE_ATTRIBUTE( Protocol::LanguagePack_Path, pak.path );
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

		float aspect = 
			static_cast<float>( m_resolution[0] ) / static_cast<float>( m_resolution[1] );

		if( m_resolution[1] > dres[1] )
		{
			m_resolution[1] = dres[1];
			m_resolution[0] = static_cast<size_t>( m_resolution[1] * aspect );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	bool Game::handleKeyEvent( unsigned int _key, unsigned int _char, bool _isDown )
	{
		bool handle = false;

		if( !handle )
		{
			//PyObject * pychar = PyBuffer_FromMemory( &_char, sizeof(_char) );
			askEvent( handle, EVENT_KEY, "(IIb)", _key, _char, _isDown );
		}

		if( !handle )
		{
			handle = m_player->handleKeyEvent( _key, _char, _isDown );
		}	

		return handle;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Game::handleMouseButtonEvent( unsigned int _button, bool _isDown )
	{
		bool handle = false;

		if( !handle )
		{
			askEvent( handle, EVENT_MOUSE_BUTTON, "(Ib)", _button, _isDown );
		}

		if( !handle )
		{
			handle = m_player->handleMouseButtonEvent( _button, _isDown );
		}	

		return handle;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Game::handleMouseButtonEventEnd( unsigned int _button, bool _isDown )
	{
		bool handle = false;

		if( !handle )
		{
			askEvent( handle, EVENT_MOUSE_BUTTON_END, "(Ib)", _button, _isDown );
		}

		if( !handle )
		{
			handle = m_player->handleMouseButtonEventEnd( _button, _isDown );
		}	

		return handle;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Game::handleMouseMove( float _x, float _y, int _whell )
	{
		bool handle = false;

		if( !handle )
		{
			askEvent( handle, EVENT_MOUSE_MOVE, "(ffi)", _x, _y, _whell );
		}

		if( !handle )
		{
			handle = m_player->handleMouseMove( _x, _y, _whell );
		}		

		return handle;
	}
	//////////////////////////////////////////////////////////////////////////
	void Game::onAppMouseLeave()
	{
		callEvent( EVENT_APP_MOUSE_LEAVE, "()" );

		m_player->onAppMouseLeave();
	}
	//////////////////////////////////////////////////////////////////////////
	void Game::onAppMouseEnter()
	{
		callEvent( EVENT_APP_MOUSE_ENTER, "()" );

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
		m_pyPersonality = ScriptEngine::get()
			->importModule( m_personalityModule );

		if( m_pyPersonality == 0 )
		{
			return false;
		}

		registerEvent( EVENT_KEY, "onHandleKeyEvent", this->getPersonality() );
		registerEvent( EVENT_MOUSE_BUTTON, "onHandleMouseButtonEvent", this->getPersonality() );
		registerEvent( EVENT_MOUSE_BUTTON_END, "onHandleMouseButtonEventEnd", this->getPersonality() );
		registerEvent( EVENT_MOUSE_MOVE, "onHandleMouseMove", this->getPersonality() );

		registerEvent( EVENT_APP_MOUSE_ENTER, "onAppMouseEnter", this->getPersonality() );
		registerEvent( EVENT_APP_MOUSE_LEAVE, "onAppMouseLeave", this->getPersonality() );

		registerEvent( EVENT_INITIALIZE, "onInitialize", this->getPersonality() );
		registerEvent( EVENT_FINALIZE, "onFinalize", this->getPersonality() );

		registerEvent( EVENT_CREATE_ACCOUNT, "onCreateAccount", this->getPersonality() );

		m_personalityHasOnClose = 
			registerEvent( EVENT_CLOSE, "onCloseWindow", this->getPersonality() );

		//AccountManager::get()
		//	->loadAccounts();

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	PyObject * Game::getPersonality()
	{
		ScriptEngine::incref( m_pyPersonality );
		return m_pyPersonality;
	}
	//////////////////////////////////////////////////////////////////////////
	namespace
	{
		class ApplicationAccountManagerListener
			: public AccountManagerListener
		{
		public:
			ApplicationAccountManagerListener( PyObject * _personality )
				: m_personality(_personality)
			{
			}

		protected:
			void onCreateAccount( const String & _accountID ) override
			{
				if( ScriptEngine::get()
					->hasModuleFunction( m_personality, ("onCreateAccount") ) == false )
				{
					MENGE_LOG_ERROR( "Warning: Personality module has no method 'onCreateAccount'. Ambigous using accounts" );

					return;
				}

				PyObject * pyfunction = ScriptEngine::get()
					->getModuleFunction( m_personality, ("onCreateAccount") );

				ScriptEngine::get()
					->callFunction( pyfunction, "(s)", _accountID.c_str() );
			}

		protected:
			PyObject * m_personality;
		};
	}
	//////////////////////////////////////////////////////////////////////////
	bool Game::init( const String& _scriptInitParams )
	{
		ApplicationAccountManagerListener * accountLister 
			= new ApplicationAccountManagerListener(m_pyPersonality);

		m_accountManager = new AccountManager(accountLister);

		AccountManager::keep(m_accountManager);

		m_accountManager->loadAccounts( "Accounts.ini");

		initPredefinedResources_();

		m_defaultArrow = ArrowManager::get()
			->createArrow( m_defaultArrowName, m_defaultArrowPrototype );

		if( m_player->init( m_defaultArrow, m_contentResolution ) == false )
		{
			return false;
		}

		bool result = false;
		if( this->askEvent( result, EVENT_INITIALIZE, "(s)", _scriptInitParams.c_str() ) == false )
		{
			return true;
		}

		return result;
	}
	//////////////////////////////////////////////////////////////////////////
	void Game::release()
	{
		callEvent( EVENT_FINALIZE, "()" );

		removePredefinedResources_();

		m_amplifier->stop();
	}
	//////////////////////////////////////////////////////////////////////////
	Arrow * Game::getDefaultArrow()
	{
		return m_defaultArrow;
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
	}
	//////////////////////////////////////////////////////////////////////////
	bool Game::getHasWindowPanel() const
	{
		return m_hasWindowPanel;
	}
	//////////////////////////////////////////////////////////////////////////
	void Game::initPredefinedResources_()
	{
		ResourceFactoryParam param = { Consts::get()->c_WhitePixel };

		ResourceImageDefault * image = ResourceManager::get()
			->createResourceWithParamT<ResourceImageDefault>( Consts::get()->c_ResourceImageDefault, param );

		image->addImagePath( Consts::get()->c_CreateImage, mt::vec2f(1.f,1.f) );
		image->incrementReference();

		ResourceManager::get()
			->registerResource( image );
	}
	//////////////////////////////////////////////////////////////////////////
	void Game::removePredefinedResources_()
	{
		ResourceManager::get()
			->directResourceRelease( Consts::get()->c_WhitePixel );
	}
	//////////////////////////////////////////////////////////////////////////
	void Game::onFullscreen( bool _fullscreen )
	{
		static String one = "1";
		static String zero = "0";

		AccountManager::get()
			->changeSetting( "FullScreen", _fullscreen ? one : zero );
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
		ResourcePak * languagePak = NULL;

		TVectorResourcePak::iterator it_language_find = 
			std::find_if( m_languagePaks.begin(), m_languagePaks.end(), FPakFinder( m_languagePak )  );

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
