#	include "Game.h"

#	include "Scene.h"
#	include "Player.h"
#	include "Arrow.h"
#	include "Amplifier.h"

#	include "LightSystem.h"
//#	include "ResourceImageDynamic.h"
#	include "ResourceImageDefault.h"

#	include "ScriptEngine.h"
#	include "FileEngine.h"
#	include "ResourceManager.h"
#	include "Logger/Logger.h"
#	include "RenderEngine.h"
#	include "ParticleEngine.h"

#	include "XmlEngine.h"
#	include "BinParser.h"

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
	Game::Game()
		: m_defaultArrow(0)
		, m_pyPersonality(0)
		, m_title( "Game" )
		, m_fixedContentResolution( false )
		, m_physicSystemName( "None" )
		, m_fullScreen( true )
		, m_textureFiltering( true )
		, m_FSAAType( 0 )
		, m_FSAAQuality( 0 )
		, m_currentAccount( 0 )
		, m_hasWindowPanel( true )
		, m_localizedTitle( false )
		, m_personalityHasOnClose( false )
		, m_player( NULL )
		, m_amplifier( NULL )
		, m_lightSystem( NULL )
		, m_playerNumberCounter( 0 )
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

		for( TAccountMap::iterator it = m_accounts.begin(), it_end = m_accounts.end();
			it != it_end;
			it++ )
		{
			delete it->second;
		}

		m_accounts.clear();

		for( TMapScene::iterator 
			it = m_mapScene.begin(),
			it_end = m_mapScene.end();
		it != it_end;
		++it)
		{
			Scene * scene = it->second;

			if( scene->isSubScene() == false )
			{
				//scene->release();
				scene->destroy();
			}
		}

		for( TMapArrow::iterator
			it = m_mapArrow.begin(),
			it_end = m_mapArrow.end();
		it != it_end;
		++it)
		{
			it->second->destroy();
		}

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
				RenderEngine::hostage()
					->setVSync( vsync );
			}

			XML_CASE_NODE( "ResourcePack" )
			{
				ResourcePak pak;
				pak.preload = true;
				XML_FOR_EACH_ATTRIBUTES()
				{
					XML_CASE_ATTRIBUTE( "Name", pak.name );
					XML_CASE_ATTRIBUTE( "Path", pak.path );
					XML_CASE_ATTRIBUTE( "Description", pak.description );
					XML_CASE_ATTRIBUTE( "PreLoad", pak.preload );
				}
				m_paks.push_back( pak );
			}
			XML_CASE_NODE( "LanguagePack" )
			{
				//m_languagePack.preload = true;
				ResourcePak pak;
				pak.preload = true;
				XML_FOR_EACH_ATTRIBUTES()
				{
					XML_CASE_ATTRIBUTE( "Name", pak.name );
					XML_CASE_ATTRIBUTE( "Path", pak.path );
					XML_CASE_ATTRIBUTE( "Description", pak.description );
					XML_CASE_ATTRIBUTE( "PreLoad", pak.preload );
				}
				//m_paks.push_back( m_languagePack );
				m_languagePaks.push_back( pak );

			}
		}
		XML_END_NODE()
		{
			const Resolution& dres = Application::hostage()
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
	void Game::parser( BinParser* _parser )
	{
		BIN_SWITCH_ID( _parser )
		{
			BIN_CASE_ATTRIBUTE( Protocol::Title_Value, m_title );
			BIN_CASE_ATTRIBUTE( Protocol::Title_Localized, m_localizedTitle );

			BIN_CASE_ATTRIBUTE( Protocol::ResourceResolution_Value, m_contentResolution ); //depricated
			BIN_CASE_ATTRIBUTE( Protocol::ContentResolution_Value, m_contentResolution );
			BIN_CASE_ATTRIBUTE( Protocol::FixedContentResolution_Value, m_fixedContentResolution );
			BIN_CASE_ATTRIBUTE( Protocol::PersonalityModule_Value, m_personality );
			BIN_CASE_ATTRIBUTE( Protocol::DefaultArrow_Value, m_defaultArrowName );
			//BIN_CASE_ATTRIBUTE( Protocol::Screensaver_Name, m_screensaverName ); //BinNew

			BIN_CASE_NODE( Protocol::Window, &Game::parserWindow_ );
			BIN_CASE_NODE( Protocol::ResourcePack, &Game::parserResourcePack_ );
			BIN_CASE_NODE( Protocol::LanguagePack, &Game::parserLanguagePack_ );
		}
		//BIN_END_NODE()
		//{
		//	const Resolution& dres = Application::hostage()
		//		->getMaxClientAreaSize();

		//	float aspect = 
		//		static_cast<float>( m_resolution[0] ) / static_cast<float>( m_resolution[1] );

		//	if( m_resolution[1] > dres[1] )
		//	{
		//		m_resolution[1] = dres[1];
		//		m_resolution[0] = static_cast<size_t>( m_resolution[1] * aspect );
		//	}
		//}
	}
	//////////////////////////////////////////////////////////////////////////
	void Game::parserWindow_( BinParser * _parser )
	{
		bool vsync = false;

		BIN_FOR_EACH_ATTRIBUTES( _parser )
		{
			BIN_CASE_ATTRIBUTE( Protocol::Window_Size, m_resolution );
			BIN_CASE_ATTRIBUTE( Protocol::Window_Bits, m_bits );
			BIN_CASE_ATTRIBUTE( Protocol::Window_Fullscreen, m_fullScreen );
			BIN_CASE_ATTRIBUTE( Protocol::Window_HasPanel, m_hasWindowPanel );
			BIN_CASE_ATTRIBUTE( Protocol::Window_VSync, vsync );
			BIN_CASE_ATTRIBUTE( Protocol::Window_TextureFiltering, m_textureFiltering );
		}

		RenderEngine::hostage()
			->setVSync( vsync );
	}
	//////////////////////////////////////////////////////////////////////////
	void Game::parserResourcePack_( BinParser * _parser )
	{
		ResourcePak pak;
		pak.preload = true;

		BIN_FOR_EACH_ATTRIBUTES( _parser )
		{
			BIN_CASE_ATTRIBUTE( Protocol::ResourcePack_Name, pak.name );
			BIN_CASE_ATTRIBUTE( Protocol::ResourcePack_Path, pak.path );
			BIN_CASE_ATTRIBUTE( Protocol::ResourcePack_Description, pak.description );
			BIN_CASE_ATTRIBUTE( Protocol::ResourcePack_PreLoad, pak.preload );
		}

		m_paks.push_back( pak );
	}
	//////////////////////////////////////////////////////////////////////////
	void Game::parserLanguagePack_( BinParser * _parser )
	{
		ResourcePak pak;
		pak.preload = true;

		BIN_FOR_EACH_ATTRIBUTES( _parser )
		{
			BIN_CASE_ATTRIBUTE( Protocol::LanguagePack_Name, pak.name );
			BIN_CASE_ATTRIBUTE( Protocol::LanguagePack_Path, pak.path );
			BIN_CASE_ATTRIBUTE( Protocol::LanguagePack_Description, pak.description );
			BIN_CASE_ATTRIBUTE( Protocol::LanguagePack_PreLoad, pak.preload );
		}

		m_languagePaks.push_back( pak );
	}
	//////////////////////////////////////////////////////////////////////////
	void Game::readResourceFile( const String& _fileSystemName, const String& _path, const String& _descFile )
	{
		m_currentPakName = _fileSystemName;
		m_currentResourcePath = _path + "/";

		//Holder<FileEngine>::hostage()
		//	->loadPak( m_currentResourcePath.substr( 0, m_currentResourcePath.length() - 1 ) );

		if( XmlEngine::hostage()
			->parseXmlFileM( _fileSystemName, _descFile, this, &Game::loaderResourceFile ) == false )
		{
			MENGE_LOG_ERROR( "Invalid resource file '%s'"
				, _descFile.c_str() 
				);
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void Game::loaderResourceFile( XmlElement * _xml )
	{
		XML_SWITCH_NODE( _xml )
		{
			XML_CASE_NODE( "Resources" )
			{
				XML_PARSE_ELEMENT( this, &Game::loaderResourceFile_ );
			}
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void Game::loaderResourceFile_( XmlElement * _xml )
	{
		XML_SWITCH_NODE( _xml )
		{
			XML_CASE_NODE( "Scenes" )
			{
				String path;
				XML_FOR_EACH_ATTRIBUTES()
				{
					XML_CASE_ATTRIBUTE( "Path", path );
				}
				
				m_pathScenes.push_back( path );

				XML_PARSE_ELEMENT( this, &Game::loaderScenes_ );
			}

			XML_CASE_NODE( "Arrows" )
			{
				String path;
				XML_FOR_EACH_ATTRIBUTES()
				{
					XML_CASE_ATTRIBUTE( "Path", path );
				}
				m_pathArrows.push_back( path );

				XML_PARSE_ELEMENT( this, &Game::loaderArrows_ );
			}
			
			XML_CASE_NODE( "Entities" )
			{
				String path;
				XML_FOR_EACH_ATTRIBUTES()
				{
					XML_CASE_ATTRIBUTE( "Path", path );
				}
				m_pathEntities.push_back( path );

				XML_PARSE_ELEMENT( this, &Game::loaderEntities_ );
			}

			XML_CASE_NODE( "Resource" )
			{
				String path;
				XML_FOR_EACH_ATTRIBUTES()
				{
					XML_CASE_ATTRIBUTE( "Path", path );
				}
				m_pathResource.push_back( path );

				XML_PARSE_ELEMENT( this, &Game::loaderResources_ );
			}

			XML_CASE_NODE( "Scripts" )
			{
				String path;
				XML_FOR_EACH_ATTRIBUTES()
				{
					XML_CASE_ATTRIBUTE( "Path", path );
				}
				m_pathScripts.push_back( path );
			}

			XML_CASE_NODE( "Text" )
			{
				String path;
				XML_FOR_EACH_ATTRIBUTES()
				{
					XML_CASE_ATTRIBUTE( "Path", path );
				}
				m_pathText.push_back( path );
			}
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void Game::loaderScenes_( XmlElement * _xml )
	{
		XML_SWITCH_NODE( _xml )
		{
			XML_CASE_NODE( "Scene" )
			{
				String sceneName;
				XML_FOR_EACH_ATTRIBUTES()
				{
					XML_CASE_ATTRIBUTE( "Name", sceneName );
				}
				String & sceneFolder = m_pathScenes.back();
				m_mapScenesDeclaration[ sceneName ] = std::make_pair( m_currentPakName, sceneFolder );

			}

		}
	}
	//////////////////////////////////////////////////////////////////////////
	void Game::loaderArrows_( XmlElement * _xml )
	{
		XML_SWITCH_NODE( _xml )
		{
			XML_CASE_NODE( "Arrow" )
			{
				String arrowName;
				XML_FOR_EACH_ATTRIBUTES()
				{
					XML_CASE_ATTRIBUTE( "Name", arrowName );
				}
				String & arrowFolder = m_pathArrows.back();
				m_mapArrowsDeclaration[ arrowName ] = std::make_pair( m_currentPakName, arrowFolder );
			}
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void Game::loaderEntities_( XmlElement * _xml )
	{
		XML_SWITCH_NODE( _xml )
		{
			XML_CASE_NODE( "Entity" )
			{
				String entityName;
				XML_FOR_EACH_ATTRIBUTES()
				{
					XML_CASE_ATTRIBUTE( "Name", entityName );
				}
				String & entityFolder = m_pathEntities.back();
				m_mapEntitiesDeclaration[ entityName ] = std::make_pair( m_currentPakName, entityFolder );
			}
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void Game::loaderResources_( XmlElement * _xml )
	{
		XML_SWITCH_NODE( _xml )
		{
			XML_CASE_NODE( "Resource" )
			{
				String resourceName;
				XML_FOR_EACH_ATTRIBUTES()
				{
					XML_CASE_ATTRIBUTE( "Name", resourceName );
				}
				String & resourceFolder = m_pathResource.back();
				m_mapResourceDeclaration[ resourceName ] = std::make_pair( m_currentPakName, resourceFolder );
			}

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
	void Game::handleMouseLeave()
	{
		if( m_pyPersonality && ScriptEngine::hostage()
								->hasModuleFunction( m_pyPersonality, "onMouseLeave" ) )
		{
			ScriptEngine::hostage()
				->callModuleFunction( m_pyPersonality, "onMouseLeave", "()" );
		}

		m_player->onMouseLeave();
	}
	//////////////////////////////////////////////////////////////////////////
	void Game::handleMouseEnter()
	{
		if( m_pyPersonality && ScriptEngine::hostage()
			->hasModuleFunction( m_pyPersonality,  "onMouseEnter" ) )
		{
			ScriptEngine::hostage()
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
		ParticleEngine::hostage()
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
		m_pyPersonality = ScriptEngine::hostage()
							->importModule( m_personality );

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

		loadAccounts();

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
		for( TMapDeclaration::iterator it = m_mapScenesDeclaration.begin(), it_end = m_mapScenesDeclaration.end();
			it != it_end;
			++it )
		{
			String sceneModule = it->first;
			sceneModule += ".Scene";

			if( ScriptEngine::hostage()->importModule( sceneModule ) == NULL )
			{
				return false;
			}
		}

		m_defaultArrow = getArrow( m_defaultArrowName );

		if( m_player->init( m_contentResolution ) == false )
		{
			return false;
		}

		if( m_pyPersonality && 
			ScriptEngine::hostage()
			->hasModuleFunction( m_pyPersonality, "init" ) )
		{
			PyObject * pyResult = ScriptEngine::hostage()
				->askModuleFunction( m_pyPersonality, "init", "(s)", _scriptInitParams.c_str() );

			bool result = false;
			if( pyResult != NULL )
			{
				result = ScriptEngine::hostage()
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
			ScriptEngine::hostage()
				->callModuleFunction( m_pyPersonality, "fini", "()" );
		}

		removePredefinedResources_();

		m_amplifier->stop();

		for( TMapArrow::iterator
			it = m_mapArrow.begin(),
			it_end = m_mapArrow.end();
		it != it_end;
		++it)
		{
			it->second->release();
		}

		for( TMapScene::iterator
			it = m_mapScene.begin(),
			it_end = m_mapScene.end();
		it != it_end;
		++it)
		{
			it->second->release();
		}		
	}
	//////////////////////////////////////////////////////////////////////////
	void Game::removeArrow( const String& _name )
	{
		m_mapArrow.erase( _name );
	}
	//////////////////////////////////////////////////////////////////////////
	bool Game::loadArrow( const String& _pakName, const String& _name )
	{
		String arrowModule = _name;
		arrowModule += ".Arrow";

		Arrow * arrow = ScriptEngine::hostage()
							->createArrow( arrowModule );

		if( arrow == 0 )
		{
			MENGE_LOG_ERROR( "Can't create arrow '%s'"
				, _name.c_str() 
				); 

			return false;
		}

		TMapDeclaration::iterator it_find = m_mapArrowsDeclaration.find( _name );
		if( it_find == m_mapArrowsDeclaration.end() )
		{
			MENGE_LOG_ERROR( "Error: Arrow '%s' declaration not found",
				_name.c_str() 
				);

			arrow->destroy();

			return false;
		}

		String xml_path = it_find->second.second;
		xml_path += "/";
		xml_path += _name;
		xml_path += "/Arrow.xml";

		if( XmlEngine::hostage()
			->parseXmlFileM( _pakName, xml_path, arrow, &Arrow::loader ) == false )
		{
			MENGE_LOG_ERROR( "Warning: invalid loader xml '%s' for arrow '%s'"
				, xml_path.c_str()
				, _name.c_str() 
				);

			arrow->destroy();

			return false;
		}

		m_mapArrow.insert( std::make_pair( _name, arrow ) );

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	Arrow * Game::getDefaultArrow()
	{
		return m_defaultArrow;
	}
	//////////////////////////////////////////////////////////////////////////
	Arrow * Game::getArrow( const String& _name )
	{
		TMapArrow::iterator it_find = m_mapArrow.find( _name );

		if( it_find == m_mapArrow.end() )
		{
			MENGE_LOG_ERROR( "Error: arrow '%s' not found"
				, _name.c_str()
				);

			return 0;
		}

		return it_find->second;
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
	Scene * Game::getScene( const String& _name )
	{
		TMapScene::iterator it_find = m_mapScene.find( _name );

		if( it_find == m_mapScene.end() )
		{
			TMapDeclaration::iterator it_find = m_mapScenesDeclaration.find( _name );

			if( it_find == m_mapScenesDeclaration.end() )
			{
				MENGE_LOG_ERROR( "Error: Scene '%s' declaration not found"
					, _name.c_str() 
					);

				return 0;
			}

			String sceneModule = _name;
			sceneModule += ".Scene";

			Scene * scene = ScriptEngine::hostage()
								->createScene( sceneModule );

			if( scene == 0 )
			{
				MENGE_LOG_ERROR( "Can't create scene '%s'"
					, _name.c_str() 
					); 

				return 0;
			}

			scene->setName( _name );

			String xml_path = it_find->second.second;
			xml_path += "/";
			xml_path += _name;
			xml_path += "/Scene.xml";

			if( XmlEngine::hostage()
				->parseXmlFileM( it_find->second.first, xml_path, scene, &Scene::loader ) == false )
			{
				MENGE_LOG_ERROR( "Warning: invalid loader xml '%s' for scene '%s'"
					, xml_path.c_str()
					, _name.c_str() 
					);
			}

			m_mapScene.insert( std::make_pair( _name, scene ) );
			scene->incrementReference();

			return scene;
		}

		it_find->second->incrementReference();

		return it_find->second;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Game::destroyScene( Scene * _scene )
	{
		const String & sceneName = _scene->getName();

		return this->destroySceneByName( sceneName );
	}
	//////////////////////////////////////////////////////////////////////////
	bool Game::destroySceneByName( const String & _sceneName )
	{
		TMapScene::iterator it_find = m_mapScene.find( _sceneName );

		if( it_find == m_mapScene.end() )
		{
			return false;
		}
		
		Scene * scene = it_find->second;

		if( scene->decrementReference() != 0 )
		{
			return false;
		}		

		scene->destroy();

		m_mapScene.erase( it_find );

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	const Resolution & Game::getContentResolution() const
	{
		return m_contentResolution;
	}
	//////////////////////////////////////////////////////////////////////////
	String Game::getTitle() const
	{
		TextManager * textMgr = TextManager::hostage();

		if( textMgr == 0 )
		{
			return m_title;
		}

		String title = m_title;

		if( m_localizedTitle == true )
		{
			title = textMgr->getTextEntry( m_title ).text;
		}

		return title;
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
	const String& Game::getPhysicSystemName() const
	{
		return m_physicSystemName;
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
	String Game::createNewAccount()
	{
		String accountID = "Player_";
		accountID += Utils::toString( m_playerNumberCounter );
		createAccount_( accountID );
		++m_playerNumberCounter;
		return accountID;
	}
	//////////////////////////////////////////////////////////////////////////
	void Game::createAccount_( const String& _accountID )
	{
		TAccountMap::iterator it_find = m_accounts.find( _accountID );
		if( it_find != m_accounts.end() )
		{
			MENGE_LOG_ERROR( "Warning: Account with ID '%s' already exist. Account not created"
				, _accountID.c_str() 
				);

			return;
		}

		Account* newAccount = new Account( _accountID );
		m_accounts.insert( std::make_pair( _accountID, newAccount ) );

		m_currentAccount = newAccount;

		if( ScriptEngine::hostage()
			->hasModuleFunction( m_pyPersonality, ("onCreateAccount") ) )
		{
			//PyObject* uName = PyUnicode_DecodeUTF8( _accountName.c_str(), _accountName.length(), NULL );
			ScriptEngine::hostage()
				->callModuleFunction( m_pyPersonality, ("onCreateAccount"), "(s)", _accountID.c_str() );

			//String accountNameAnsi = Holder<Application>::hostage()->utf8ToAnsi( _accountName );
			//Holder<ScriptEngine>::hostage()
			//	->callModuleFunction( m_pyPersonality, ("onCreateAccount"), "(s)", accountNameAnsi.c_str() );
		}
		else
		{
			MENGE_LOG_ERROR( "Warning: Personality module has no method 'onCreateAccount'. Ambigous using accounts" );
		}

		FileEngine::hostage()
			->createDirectory( "user", newAccount->getFolder() );

		newAccount->save();
		saveAccountsInfo();
	}
	//////////////////////////////////////////////////////////////////////////
	void Game::deleteAccount( const String& _accountID )
	{
		TAccountMap::iterator it_find = m_accounts.find( _accountID );

		if( it_find != m_accounts.end() )
		{
			if( m_currentAccount && ( m_currentAccount->getFolder() == _accountID ) )
			{
				m_currentAccount = 0;
			}

			FileEngine::hostage()->
				removeDirectory( "user", it_find->second->getFolder() );

			delete it_find->second;

			m_accounts.erase( it_find );
		}
		else
		{
			MENGE_LOG_ERROR( "Can't delete account '%s'. There is no account with such ID"
				, _accountID.c_str() 
				);
		}

		saveAccountsInfo();
	}
	//////////////////////////////////////////////////////////////////////////
	void Game::selectAccount( const String& _accountID )
	{
		TAccountMap::iterator it_find = m_accounts.find( _accountID );

		if( it_find != m_accounts.end() )
		{
			m_currentAccount = it_find->second;
			m_currentAccount->load();
			m_currentAccount->apply();
			saveAccountsInfo();
		}
		else
		{
			MENGE_LOG_ERROR( "Can't select account '%s'. There is no account with such ID"
				, _accountID.c_str() 
				);
		}
	}
	//////////////////////////////////////////////////////////////////////////
	Account* Game::getCurrentAccount()
	{
		return m_currentAccount;
	}
	//////////////////////////////////////////////////////////////////////////
	Account* Game::getAccount( const String& _accountID )
	{
		TAccountMap::iterator it_find = m_accounts.find( _accountID );
		if( it_find != m_accounts.end() )
		{
			return it_find->second;
		}
		else
		{
			MENGE_LOG_ERROR( "Account with ID '%s' not found", _accountID.c_str() );
		}
		return NULL;
	}
	//////////////////////////////////////////////////////////////////////////
	void Game::loaderAccounts_( XmlElement* _xml )
	{
		XML_SWITCH_NODE( _xml )
		{
			XML_CASE_NODE( "AccountID" )
			{
				String accID;
				XML_FOR_EACH_ATTRIBUTES()
				{
					XML_CASE_ATTRIBUTE( "Value", accID );
				}
				//m_accountIDs.push_back( accID );
				m_accounts.insert( std::make_pair<String, Account*>( accID, NULL ) );
			}
			XML_CASE_ATTRIBUTE_NODE( "DefaultAccountID", "Value", m_defaultAccountID );
			XML_CASE_ATTRIBUTE_NODE( "PlayerCounter", "Value", m_playerNumberCounter );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	Account* Game::loadAccount_( const String& _accountID )
	{
		Account* newAccount = new Account( _accountID );
		m_currentAccount = newAccount;
		if( ScriptEngine::hostage()
			->hasModuleFunction( m_pyPersonality, ("onCreateAccount") ) )
		{
			ScriptEngine::hostage()
				->callModuleFunction( m_pyPersonality, ("onCreateAccount"), "(s)", _accountID.c_str() );

		}
		newAccount->load();
		return newAccount;
	}
	//////////////////////////////////////////////////////////////////////////
	void Game::loadAccounts()
	{
		String accFilename = "Accounts.xml";
		bool accountsExist = FileEngine::hostage()
								->existFile( "user", accFilename );
		if( accountsExist == true )
		{
			//if( loaderAccounts_( accFilename ) == false )
			if( XmlEngine::hostage()->parseXmlFileM( "user", accFilename, this, &Game::loaderAccounts_ ) == false )
			{
				MENGE_LOG_ERROR( "Parsing Accounts ini failed '%s'"
					, accFilename.c_str()
					);

				return;
			}

			for( TAccountMap::iterator it = m_accounts.begin(), it_end = m_accounts.end();
				it != it_end;
				it++ )
			{
				//createAccount( it->name, it->folder );
				it->second = loadAccount_( it->first );
			}

			if( m_defaultAccountID != "" )
			{
				selectAccount( m_defaultAccountID );
			}
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void Game::saveAccountsInfo()
	{
		FileOutputInterface* outFile = FileEngine::hostage()
									->openFileOutput( "user", "Accounts.xml" );

		if( outFile == NULL )
		{
			MENGE_LOG_ERROR( "Accounts info wouldn't be saved. Can't open file for writing" );
			return;
		}

		Utils::fileWrite( outFile, "<Accounts>\n" );
		for( TAccountMap::iterator it = m_accounts.begin(), it_end = m_accounts.end();
			it != it_end;
			++it )
		{
			Utils::fileWrite( outFile, "\t<AccountID Value = \"" + it->first + "\"/>\n" );
		}
		if( m_currentAccount != 0 )
		{
			Utils::fileWrite( outFile, "\t<DefaultAccountID Value = \"" + m_currentAccount->getFolder() + "\"/>\n" );
		}

		Utils::fileWrite( outFile, "\t<PlayerCounter Value = \"" + Utils::toString( m_playerNumberCounter ) + "\"/>\n" );
		Utils::fileWrite( outFile, "</Accounts>" );

		FileEngine::hostage()
			->closeFileOutput( outFile );
	}
	//////////////////////////////////////////////////////////////////////////
	void Game::saveAccount( const String& _accountID )
	{
		TAccountMap::iterator it_find = m_accounts.find( _accountID );

		if( it_find != m_accounts.end() )
		{
			it_find->second->save();
		}
		else
		{
			MENGE_LOG_ERROR( "Warning: Account with ID '%s' does not exist. Can't save"
				, _accountID.c_str() 
				);
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void Game::saveAccounts()
	{
		for( TAccountMap::iterator it = m_accounts.begin(), it_end = m_accounts.end();
		it != it_end;
		it++ )
		{
			it->second->save();
		}
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
		ResourceFactoryParam param = { "WhitePixel" };

		ResourceImageDefault * image = ResourceManager::hostage()
			->createResourceWithParamT<ResourceImageDefault>( "ResourceImageDefault", param );

		image->addImagePath( "CreateImage" );
		image->incrementReference();
		//ResourceImageDynamic * image = new ResourceImageDynamic( param );
		//image->setSize( mt::vec2f( 1.0f, 1.0f ) );
		//image->incrementReference();

		ResourceManager::hostage()
			->registerResource( image );
	}
	//////////////////////////////////////////////////////////////////////////
	void Game::removePredefinedResources_()
	{
		ResourceManager::hostage()
			->directResourceRelease("WhitePixel");
	}
	//////////////////////////////////////////////////////////////////////////
	const TVectorString& Game::getScriptsPaths() const
	{
		return m_pathScripts;
	}
	//////////////////////////////////////////////////////////////////////////
	const TVectorString& Game::getArrowPaths() const
	{
		return m_pathArrows;
	}
	//////////////////////////////////////////////////////////////////////////
	const TVectorString& Game::getEntitiesPaths() const
	{
		return m_pathEntities;
	}
	//////////////////////////////////////////////////////////////////////////
	const TVectorString& Game::getScenesPaths() const
	{
		return m_pathScenes;
	}
	//////////////////////////////////////////////////////////////////////////
	const TVectorString& Game::getResourcesPaths() const
	{
		return m_pathResource;
	}
	//////////////////////////////////////////////////////////////////////////
	const TVectorString& Game::getTextsPaths() const
	{
		return m_pathText;
	}
	/////////////////////////////////////////////////////////////////////////
	const TVectorString& Game::getResourceFilePaths() const
	{
		return m_pathResourceFiles;
	}
	//////////////////////////////////////////////////////////////////////////
	void Game::onFullscreen( bool _fullscreen )
	{
		static String one = "1";
		static String zero = "0";
		if( m_currentAccount != NULL )
		{
			m_currentAccount->changeSetting( "FullScreen", _fullscreen ? one : zero );
		}
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
	void Game::loadPak( const String& _pakName, const String& _pakPath, const String& _descFilename )
	{
		readResourceFile( _pakName, _pakPath, _descFilename );

		ScriptEngine::TListModulePath m_listModulePath;

		for( TVectorString::iterator it = m_pathScripts.begin(),
			it_end = m_pathScripts.end(); it != it_end; it++ )
		{
			m_listModulePath.push_back( m_baseDir + "/" + _pakPath + "/" + *it );
		}

		for( TVectorString::iterator it = m_pathEntities.begin(),
			it_end = m_pathEntities.end(); it != it_end; it++ )
		{
			m_listModulePath.push_back( m_baseDir + "/" + _pakPath + "/" + *it );
		}

		for( TVectorString::iterator it = m_pathScenes.begin(),
			it_end = m_pathScenes.end(); it != it_end; it++ )
		{
			m_listModulePath.push_back( m_baseDir + "/" + _pakPath + "/" + *it );
		}

		for( TVectorString::iterator it = m_pathArrows.begin(),
			it_end = m_pathArrows.end(); it != it_end; it++ )
		{
			m_listModulePath.push_back( m_baseDir + "/" + _pakPath + "/" + *it );
		}

		ScriptEngine::hostage()
			->addModulePath( m_listModulePath );

		for( TMapDeclaration::iterator
			it = m_mapEntitiesDeclaration.begin(),
			it_end = m_mapEntitiesDeclaration.end();
		it != it_end;
		it++ )
		{
			ScriptEngine::hostage()
				->registerEntityType( it->second.first, it->second.second, it->first );
		}


		for( TVectorString::iterator it = m_pathText.begin(),
			it_end = m_pathText.end(); it != it_end; it++ )
		{
			TextManager::hostage()
				->loadResourceFile( _pakName, *it );
		}

		for( TMapDeclaration::iterator
			it = m_mapResourceDeclaration.begin(),
			it_end = m_mapResourceDeclaration.end();
		it != it_end;
		it++ )
		{
			//String path = getPathResource( it->first );

			//m_pathResourceFiles.push_back( path );

			//String category = getCategoryResource( path );
			String path = it->second.second;
			path += "/";
			path += it->first;
			path += ".resource";

			ResourceManager::hostage()
				->loadResource( it->second.first, it->first, path );
		}

		for( TMapDeclaration::iterator
			it = m_mapArrowsDeclaration.begin(),
			it_end = m_mapArrowsDeclaration.end();
		it != it_end;
		it++ )
		{
			loadArrow( _pakName, it->first );
		}

		m_pathScripts.clear();
		m_pathEntities.clear();
		m_pathScenes.clear();
		m_pathArrows.clear();
		m_mapEntitiesDeclaration.clear();
		m_pathText.clear();
		m_mapResourceDeclaration.clear();
		m_mapArrowsDeclaration.clear();
	}
	//////////////////////////////////////////////////////////////////////////
	void Game::loadConfigPaks()
	{
		ResourcePak* languagePak = NULL;
		if( m_languagePack.empty() == false )
		{
			TResourcePakVector::iterator it_find = 
				std::find_if( m_languagePaks.begin(), m_languagePaks.end(), PakFinder( m_languagePack ) );
			if( it_find != m_languagePaks.end() )
			{
				languagePak = &(*it_find);
			}
		}
		if( languagePak == NULL && m_languagePaks.empty() == false )
		{
			languagePak = &(m_languagePaks[0]);
		}

		if( languagePak != NULL )
		{
			m_paks.push_back( *languagePak );
		}

		FileEngine* fileEngine = FileEngine::hostage();
		for( TResourcePakVector::iterator it = m_paks.begin(), it_end = m_paks.end();
			it != it_end;
			++it )
		{
			ResourcePak& pak = (*it);
			if( pak.preload == false )
			{
				continue;
			}
			if( fileEngine->mountFileSystem( pak.name, pak.path, false ) == false )
			{
				MENGE_LOG_ERROR( "Error: failed to mount pak '%s': '%s'"
					, pak.name.c_str()
					, pak.path.c_str() 
					);

				continue;
			}
			loadPak( pak.name, pak.path, pak.description );
		}

	}
	//////////////////////////////////////////////////////////////////////////
	void Game::setLanguagePack( const String& _packName )
	{
		m_languagePack = _packName;
	}
	//////////////////////////////////////////////////////////////////////////
	void Game::loadPakFromName( const String& _pakName )
	{
		TResourcePakVector::iterator it_find = 
			std::find_if( m_paks.begin(), m_paks.end(), PakFinder( _pakName )  );
		if( it_find != m_paks.end() )
		{
			ResourcePak& pak = (*it_find);
			loadPak( pak.name, pak.path, pak.description );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	String Game::getPakPath( const String& _packName )
	{
		TResourcePakVector::iterator it_find 
			= std::find_if( m_paks.begin(), m_paks.end(), PakFinder( _packName ) );
		if( it_find != m_paks.end() )
		{
			return it_find->path;
		}

		return "";
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
