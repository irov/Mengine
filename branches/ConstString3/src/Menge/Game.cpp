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

#	include "XmlEngine.h"
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
		, m_title( Consts::get()->c_Game )
		, m_fixedContentResolution( false )
		, m_fullScreen( true )
		, m_textureFiltering( true )
		, m_FSAAType( 0 )
		, m_FSAAQuality( 0 )
		, m_hasWindowPanel( true )
		, m_localizedTitle( false )
		, m_personalityHasOnClose( false )
		, m_player( NULL )
		, m_amplifier( NULL )
		, m_lightSystem( NULL )
	{
		m_player = new Player();
		m_amplifier = new Amplifier();
		m_lightSystem = new LightSystem();//?

		m_homeless = new Node;
	}
	//////////////////////////////////////////////////////////////////////////
	Game::~Game()
	{
		release();

		delete m_homeless;

		delete m_amplifier;
		delete m_player;
		delete m_lightSystem;
	}
	//////////////////////////////////////////////////////////////////////////
	void Game::loader( XmlElement* _xml )
	{
		XML_SWITCH_NODE( _xml )
		{
			XML_CASE_ATTRIBUTE_NODE( "Title", "Value", m_title );
			XML_CASE_ATTRIBUTE_NODE( "Title", "Localized", m_localizedTitle );

			XML_CASE_ATTRIBUTE_NODE( "ResourceResolution", "Value", m_contentResolution ); //depricated
			XML_CASE_ATTRIBUTE_NODE( "ContentResolution", "Value", m_contentResolution );
			XML_CASE_ATTRIBUTE_NODE( "FixedContentResolution", "Value", m_fixedContentResolution );
			XML_CASE_ATTRIBUTE_NODE( "PersonalityModule", "Value", m_personality );
			XML_CASE_ATTRIBUTE_NODE( "DefaultArrow", "Value", m_defaultArrowName );
			XML_CASE_ATTRIBUTE_NODE( "Screensaver", "Name", m_screensaverName );
			XML_CASE_NODE( "Window" )
			{
				bool vsync = false;
				XML_FOR_EACH_ATTRIBUTES()
				{
					XML_CASE_ATTRIBUTE( "Size", m_resolution );
					XML_CASE_ATTRIBUTE( "Bits", m_bits );
					XML_CASE_ATTRIBUTE( "Fullscreen", m_fullScreen );
					XML_CASE_ATTRIBUTE( "HasPanel", m_hasWindowPanel );
					XML_CASE_ATTRIBUTE( "VSync", vsync );
					XML_CASE_ATTRIBUTE( "TextureFiltering", m_textureFiltering );
				}
				RenderEngine::get()
					->setVSync( vsync );
			}

			XML_CASE_NODE( "ResourcePack" )
			{
				ResourcePakDesc pak;
				pak.preload = true;
				XML_FOR_EACH_ATTRIBUTES()
				{
					XML_CASE_ATTRIBUTE( "Name", pak.name );
					XML_CASE_ATTRIBUTE( "Path", pak.path );
					XML_CASE_ATTRIBUTE( "Description", pak.description );
					XML_CASE_ATTRIBUTE( "PreLoad", pak.preload );
				}

				m_paks.push_back( new ResourcePak(pak, m_baseDir) );
			}
			XML_CASE_NODE( "LanguagePack" )
			{
				ResourcePakDesc pak;
				pak.preload = true;
				XML_FOR_EACH_ATTRIBUTES()
				{
					XML_CASE_ATTRIBUTE( "Name", pak.name );
					XML_CASE_ATTRIBUTE( "Path", pak.path );
					XML_CASE_ATTRIBUTE( "Description", pak.description );
					XML_CASE_ATTRIBUTE( "PreLoad", pak.preload );
				}

				m_languagePaks.push_back( new ResourcePak(pak, m_baseDir) );
			}
		}
		XML_END_NODE()
		{
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
	}
	//////////////////////////////////////////////////////////////////////////
	void Game::parser( BinParser * _parser )
	{

	}
	//////////////////////////////////////////////////////////////////////////
	void Game::_loaded()
	{

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
	void Game::handleMouseLeave()
	{
		if( m_pyPersonality && ScriptEngine::get()
			->hasModuleFunction( m_pyPersonality, "onMouseLeave" ) )
		{
			ScriptEngine::get()
				->callModuleFunction( m_pyPersonality, "onMouseLeave", "()" );
		}

		m_player->onMouseLeave();
	}
	//////////////////////////////////////////////////////////////////////////
	void Game::handleMouseEnter()
	{
		if( m_pyPersonality && ScriptEngine::get()
			->hasModuleFunction( m_pyPersonality,  "onMouseEnter" ) )
		{
			ScriptEngine::get()
				->callModuleFunction( m_pyPersonality,  "onMouseEnter", "()" );
		}

		m_player->onMouseEnter();
	}
	//////////////////////////////////////////////////////////////////////////
	void Game::update()
	{
		m_player->updateChangeScene();
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
			->importModule( m_personality, Consts::get()->c_builtin_empty, Consts::get()->c_builtin_empty );

		if( m_pyPersonality == 0 )
		{
			return false;
		}

		registerEvent( EVENT_KEY, "onHandleKeyEvent", this->getPersonality() );
		registerEvent( EVENT_MOUSE_BUTTON, "onHandleMouseButtonEvent", this->getPersonality() );
		registerEvent( EVENT_MOUSE_BUTTON_END, "onHandleMouseButtonEventEnd", this->getPersonality() );
		registerEvent( EVENT_MOUSE_MOVE, "onHandleMouseMove", this->getPersonality() );

		m_personalityHasOnClose = 
			registerEvent( EVENT_CLOSE, "onCloseWindow", this->getPersonality() );

		AccountManager::get()
			->loadAccounts();

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	PyObject * Game::getPersonality()
	{
		ScriptEngine::incref( m_pyPersonality );
		return m_pyPersonality;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Game::init( const String& _scriptInitParams )
	{
		initPredefinedResources_();

		// check scripts
		//for( TMapDeclaration::iterator 
		//	it = m_mapScenesDeclaration.begin(), 
		//	it_end = m_mapScenesDeclaration.end();
		//it != it_end;
		//++it )
		//{
		//	const ConstString & sceneModule = it->first;
		//	
		//	sceneModule += ".Scene";

		//	if( ScriptEngine::get()
		//		->importModule( sceneModule ) == NULL )
		//	{
		//		return false;
		//	}
		//}

		m_defaultArrow = ArrowManager::get()
			->getArrow( m_defaultArrowName );

		if( m_player->init( m_defaultArrow, m_contentResolution ) == false )
		{
			return false;
		}

		if( m_pyPersonality && 
			ScriptEngine::get()
			->hasModuleFunction( m_pyPersonality, "init" ) )
		{
			PyObject * pyResult = ScriptEngine::get()
				->askModuleFunction( m_pyPersonality, "init", "(s)", _scriptInitParams.c_str() );

			bool result = false;
			if( pyResult != NULL )
			{
				result = ScriptEngine::get()
					->parseBool( pyResult );
			}

			if( result == false )
			{
				return false;
			}
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void Game::release()
	{
		if( m_pyPersonality )
		{
			ScriptEngine::get()
				->callModuleFunction( m_pyPersonality, "fini", "()" );
		}

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
			return m_title.str();
		}

		TextManager * textMgr = TextManager::get();

		if( textMgr == 0 )
		{
			return m_title.str();
		}

		const TextManager::TextEntry & entry = textMgr->getTextEntry( m_title );

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

		image->addImagePath( Consts::get()->c_CreateImage );
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
