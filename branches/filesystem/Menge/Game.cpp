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
#	include "LogEngine.h"
#	include "RenderEngine.h"

#	include "XmlEngine.h"
#	include "Utils.h"
#	include "ConfigFile.h"
#	include "TextManager.h"

#	include "Application.h"

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
		, m_vsync( false )
		, m_textureFiltering( true )
		, m_FSAAType( 0 )
		, m_FSAAQuality( 0 )
		, m_currentAccount( 0 )
		, m_loadingAccounts( false )
		, m_hasWindowPanel( true )
		, m_localizedTitle( false )
		, m_personalityHasOnClose( false )
	{
		m_languagePack.preload = false;
		m_player = new Player();
		Holder<Player>::keep( m_player );
		Holder<Amplifier>::keep( new Amplifier() );
		Holder<LightSystem>::keep( new LightSystem );//?
	}
	//////////////////////////////////////////////////////////////////////////
	Game::~Game()
	{
		release();

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
				scene->release();
				scene->destroy();
			}
		}

		for( TMapArrow::iterator
			it = m_mapArrow.begin(),
			it_end = m_mapArrow.end();
		it != it_end;
		++it)
		{
			Arrow * arrow = it->second;

			arrow->release();
			arrow->destroy();
		}

		Holder<Amplifier>::destroy();
		Holder<Player>::destroy();
		//Holder<MousePickerSystem>::destroy();
		Holder<LightSystem>::destroy();
	}
	//////////////////////////////////////////////////////////////////////////
	//bool Game::loader( const String& _iniFile )
	void Game::loader( XmlElement* _xml )
	{
		//ConfigFile config;
		//if( config.load( _iniFile ) == false )
		//{
		//	return false;
		//}
		//m_title = config.getSetting( "Title", "GAME" );
		//m_resourceResolution[0] = config.getSettingUInt( "ResourceResolutionWidth", "GAME" );
		//m_resourceResolution[1] = config.getSettingUInt( "ResourceResolutionHeight", "GAME" );
		//m_fixedContentResolution = config.getSettingBool( "FixedContentResolution", "GAME" );
		//m_personality = config.getSetting( "PersonalityModule", "GAME" );
		//m_eventInit = config.getSetting( "InitFunction", "GAME" );
		//m_eventUpdate = config.getSetting( "UpdateFunction", "GAME" );
		//m_eventFini = config.getSetting( "FinilizeFunction", "GAME" );
		//m_defaultArrowName = config.getSetting( "DefaultArrow", "GAME" );
		//m_resolution[0] = config.getSettingUInt( "Width", "GAME" );
		//m_resolution[1] = config.getSettingUInt( "Height", "GAME" );
		//m_bits = config.getSettingInt( "Bits", "GAME" );
		//m_fullScreen = config.getSettingBool( "Fullscreen", "GAME" );
		//m_resourcePaths = config.getMultiSetting( "ResourceFile", "GAME" );
		//if( config.getSetting( "WindowPanel", "GAME" ).empty() == false )
		//{
		//	m_hasWindowPanel = config.getSettingBool( "WindowPanel", "GAME" );
		//}
		//return true;
		XML_SWITCH_NODE( _xml )
		{
			XML_CASE_NODE( "Title" )
			{
				XML_FOR_EACH_ATTRIBUTES()
				{
					XML_CASE_ATTRIBUTE( "Value", m_title );
					XML_CASE_ATTRIBUTE( "Localized", m_localizedTitle );
				}
			}
			XML_CASE_ATTRIBUTE_NODE( "ResourceResolution", "Value", m_resourceResolution );
			XML_CASE_ATTRIBUTE_NODE( "FixedContentResolution", "Value", m_fixedContentResolution );
			XML_CASE_ATTRIBUTE_NODE( "PersonalityModule", "Value", m_personality );
			XML_CASE_ATTRIBUTE_NODE( "DefaultArrow", "Value", m_defaultArrowName );
			XML_CASE_NODE( "Window" )
			{
				XML_FOR_EACH_ATTRIBUTES()
				{
					XML_CASE_ATTRIBUTE( "Size", m_resolution );
					XML_CASE_ATTRIBUTE( "Bits", m_bits );
					XML_CASE_ATTRIBUTE( "Fullscreen", m_fullScreen );
					XML_CASE_ATTRIBUTE( "HasPanel", m_hasWindowPanel );
					XML_CASE_ATTRIBUTE( "VSync", m_vsync );
					XML_CASE_ATTRIBUTE( "TextureFiltering", m_textureFiltering );
				}
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
				if( pak.preload == true )
				{
					//m_resourcePaths.push_back( pak.name + "/" + pak.description );
					m_preloadPaks.push_back( pak );
				}
			}
			XML_CASE_NODE( "LanguagePack" )
			{
				m_languagePack.preload = true;
				XML_FOR_EACH_ATTRIBUTES()
				{
					XML_CASE_ATTRIBUTE( "Name", m_languagePack.name );
					XML_CASE_ATTRIBUTE( "Path", m_languagePack.path );
					XML_CASE_ATTRIBUTE( "Description", m_languagePack.description );
					XML_CASE_ATTRIBUTE( "PreLoad", m_languagePack.preload );
				}
				if( m_languagePack.preload == true )
				{
					m_preloadPaks.push_back( m_languagePack );
				}
				//if( pak.preload == true && m_languagePack.empty() == true )
				//{
				//	m_languagePack = pak.name + "/" + pak.description;
				//}

			}
		}
		XML_END_NODE()
		{
			const Resolution& dres = Holder<Application>::hostage()
										->getMaxClientAreaSize();
			size_t minHeight = std::min( dres[1], m_resolution[1] );
			float contentAspect = 
				static_cast<float>( m_resourceResolution[0] ) / static_cast<float>( m_resourceResolution[1] );
			size_t minWidth = static_cast<size_t>( minHeight * contentAspect );
			m_resolution[0] = minWidth;
			m_resolution[1] = minHeight;
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void Game::readResourceFile( const String& _fileSystemName, const String& _path, const String& _descFile )
	{
		m_currentPakName = _fileSystemName;
		m_currentResourcePath = _path + "/";

		//Holder<FileEngine>::hostage()
		//	->loadPak( m_currentResourcePath.substr( 0, m_currentResourcePath.length() - 1 ) );

		if( Holder<XmlEngine>::hostage()
			->parseXmlFileM( _fileSystemName, _descFile, this, &Game::loaderResourceFile ) == false )
		{
			MENGE_LOG_ERROR( "Invalid resource file \"%s\""
				, _descFile.c_str() );
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
			String & sceneFolder = m_pathScenes.back();
			m_mapScenesDeclaration[ XML_TITLE_NODE ] = std::make_pair( m_currentPakName, sceneFolder );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void Game::loaderArrows_( XmlElement * _xml )
	{
		XML_SWITCH_NODE( _xml )
		{
			String & arrowFolder = m_pathArrows.back();
			m_mapArrowsDeclaration[ XML_TITLE_NODE ] = std::make_pair( m_currentPakName, arrowFolder );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void Game::loaderEntities_( XmlElement * _xml )
	{
		XML_SWITCH_NODE( _xml )
		{
			String & entityFolder = m_pathEntities.back();
			m_mapEntitiesDeclaration[ XML_TITLE_NODE ] = std::make_pair( m_currentPakName, entityFolder );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void Game::loaderResources_( XmlElement * _xml )
	{
		XML_SWITCH_NODE( _xml )
		{
			String & resourceFolder = m_pathResource.back();
			m_mapResourceDeclaration[ XML_TITLE_NODE ] = std::make_pair( m_currentPakName, resourceFolder );
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
		if( m_pyPersonality && Holder<ScriptEngine>::hostage()
			->hasModuleFunction( m_pyPersonality, "onMouseLeave" ) )
		{
			Holder<ScriptEngine>::hostage()
				->callModuleFunction( m_pyPersonality, "onMouseLeave", "()" );
		}
		m_player->onMouseLeave();
	}
	//////////////////////////////////////////////////////////////////////////
	void Game::handleMouseEnter()
	{
		if( m_pyPersonality && Holder<ScriptEngine>::hostage()
			->hasModuleFunction( m_pyPersonality,  "onMouseEnter" ) )
		{
			Holder<ScriptEngine>::hostage()
				->callModuleFunction( m_pyPersonality,  "onMouseEnter", "()" );
		}
		m_player->onMouseEnter();
	}
	//////////////////////////////////////////////////////////////////////////
	void Game::update( float _timing )
	{
		Holder<Amplifier>::hostage()->update( _timing );

		m_player->update( _timing );

	}
	//////////////////////////////////////////////////////////////////////////
	void Game::render( unsigned int _debugMask )
	{
		m_player->render( _debugMask );
	}
	//////////////////////////////////////////////////////////////////////////
	bool Game::loadPersonality()
	{
		m_pyPersonality = Holder<ScriptEngine>::hostage()
			->importModule( m_personality );

		if( m_pyPersonality == 0 )
		{
			return false;
		}

		registerEvent( EVENT_KEY, "onHandleKeyEvent", this->getPersonality() );
		registerEvent( EVENT_MOUSE_BUTTON, "onHandleMouseButtonEvent", this->getPersonality() );
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
			if( Holder<ScriptEngine>::hostage()->importModule( sceneModule ) == NULL )
			{
				return false;
			}
		}

		m_defaultArrow = getArrow( m_defaultArrowName );

		if( m_player->init( m_resourceResolution ) == false )
		{
			return false;
		}

		if( m_pyPersonality && 
			Holder<ScriptEngine>::hostage()
			->hasModuleFunction( m_pyPersonality, "init" ) )
		{
			PyObject * pyResult = Holder<ScriptEngine>::hostage()
				->askModuleFunction( m_pyPersonality, "init", "(s)", _scriptInitParams.c_str() );

			bool result = Holder<ScriptEngine>::hostage()
				->parseBool( pyResult );

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
		removePredefinedResources_();

		if( m_pyPersonality )
		{
			Holder<ScriptEngine>::hostage()
				->callModuleFunction( m_pyPersonality, "fini", "()" );
		}

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

		Arrow * arrow = Holder<ScriptEngine>::hostage()
			->createArrow( arrowModule );

		if( arrow == 0 )
		{
			MENGE_LOG_ERROR( "Can't create arrow \"%s\""
				, _name.c_str() ); 

			return false;
		}

		TMapDeclaration::iterator it_find = m_mapArrowsDeclaration.find( _name );
		if( it_find == m_mapArrowsDeclaration.end() )
		{
			MENGE_LOG_ERROR( "Error: Arrow \"%s\" declaration not found",
				_name.c_str() );
			return false;
		}

		String xml_path = it_find->second.second;
		xml_path += "/";
		xml_path += _name;
		xml_path += "/Arrow.xml";

		if( Holder<XmlEngine>::hostage()
			->parseXmlFileM( _pakName, xml_path, arrow, &Arrow::loader ) == false )
		{
			MENGE_LOG_ERROR( "Warning: invalid loader xml \"%s\" for arrow \"%s\""
				, xml_path.c_str()
				, _name.c_str() );
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
			return 0;
		}

		return it_find->second;
	}
	//////////////////////////////////////////////////////////////////////////
	void Game::addHomeless( PyObject * _homeless )
	{
		Scene * scene = Holder<Player>::hostage()
			->getCurrentScene();

		if( scene == 0 )
		{
			pybind::incref( _homeless );
			m_homeless.push_back( _homeless );
		}
		else
		{
			Node * node = pybind::extract_nt<Node *>( _homeless );
			scene->addHomeless( node );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	Scene * Game::getScene( const String& _name )
	{
		TMapScene::iterator it_find = m_mapScene.find( _name );

		if( it_find == m_mapScene.end() )
		{
			String sceneModule = _name;
			sceneModule += ".Scene";

			Scene * scene = Holder<ScriptEngine>::hostage()
				->createScene( sceneModule );

			for( TContainerHomeless::iterator
				it = m_homeless.begin(),
				it_end = m_homeless.end();
			it != it_end;
			++it)
			{
				PyObject * homeless = *it;

				if( homeless->ob_refcnt > 1 )
				{
					Node * node = pybind::extract_nt<Node *>( homeless );

					if( node->getParent() == 0 )
					{
						scene->addHomeless( node );
					}
				}

				pybind::decref( homeless );


			}

			m_homeless.clear();

			if( scene == 0 )
			{
				MENGE_LOG_ERROR( "Can't create scene \"%s\""
					, _name.c_str() ); 

				return 0;
			}

			scene->setName( _name );
			
			TMapDeclaration::iterator it_find = m_mapScenesDeclaration.find( _name );
			if( it_find == m_mapScenesDeclaration.end() )
			{
				MENGE_LOG_ERROR( "Error: Scene \"%s\" declaration not found",
					_name.c_str() );
				return 0;
			}

			String xml_path = it_find->second.second;
			xml_path += "/";
			xml_path += _name;
			xml_path += "/Scene.xml";

			if( Holder<XmlEngine>::hostage()
				->parseXmlFileM( it_find->second.first, xml_path, scene, &Scene::loader ) == false )
			{
				MENGE_LOG_ERROR( "Warning: invalid loader xml \"%s\" for scene \"%s\""
				, xml_path.c_str()
				, _name.c_str() );
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

		TMapScene::iterator it_find = m_mapScene.find( sceneName );

		if( it_find == m_mapScene.end() )
		{
			return false;
		}
		
		if( it_find->second->decrementReference() != 0 )
		{
			return false;
		}

			
		Scene * scene = it_find->second;

		scene->destroy();

		m_mapScene.erase( it_find );

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	const Resolution & Game::getResourceResolution() const
	{
		return m_resourceResolution;
	}
	//////////////////////////////////////////////////////////////////////////
	String Game::getTitle() const
	{
		TextManager * textMgr = Holder<TextManager>::hostage();

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
	bool Game::getVSync() const
	{
		return m_vsync;
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
	void Game::createAccount( const String& _accountName )
	{
		TAccountMap::iterator it = m_accounts.find( _accountName );
		if( it != m_accounts.end() )
		{
			MENGE_LOG_ERROR( "Warning: Account with name \"%s\" already exist. Account not created"
				, _accountName.c_str() );
			return;
		}

		if( m_loadingAccounts == false )
		{
			m_accountNames.push_back( _accountName );
		}

		Account* newAccount = new Account( _accountName );
		m_accounts.insert( std::make_pair( _accountName, newAccount ) );

		m_currentAccount = newAccount;

		if( Holder<ScriptEngine>::hostage()
			->hasModuleFunction( m_pyPersonality, ("onCreateAccount") ) )
		{
			PyObject* uName = PyUnicode_DecodeUTF8( _accountName.c_str(), _accountName.length(), NULL );
			Holder<ScriptEngine>::hostage()
				->callModuleFunction( m_pyPersonality, ("onCreateAccount"), "(O)", uName );

			//String accountNameAnsi = Holder<Application>::hostage()->utf8ToAnsi( _accountName );
			//Holder<ScriptEngine>::hostage()
			//	->callModuleFunction( m_pyPersonality, ("onCreateAccount"), "(s)", accountNameAnsi.c_str() );
		}
		else
		{
			MENGE_LOG_ERROR( "Warning: Personality module has no method 'onCreateAccount'. Ambigous using accounts" );
		}

		Holder<FileEngine>::hostage()
			->createDirectory( "user", _accountName );

		if( m_loadingAccounts == false )
		{
			newAccount->save();
			saveAccountsInfo();
		}
		//m_currentAccount->apply();
	}
	//////////////////////////////////////////////////////////////////////////
	void Game::deleteAccount( const String& _accountName )
	{
		TAccountMap::iterator it_find = m_accounts.find( _accountName );

		if( it_find != m_accounts.end() )
		{
			if( m_currentAccount && ( m_currentAccount->getName() == _accountName ) )
			{
				m_currentAccount = 0;
			}

			Holder<FileEngine>::hostage()->
				removeDirectory( "user", _accountName );

			delete it_find->second;

			m_accounts.erase( it_find );
			m_accountNames.erase( std::remove( m_accountNames.begin(), m_accountNames.end(), _accountName ) );
		}
		else
		{
			MENGE_LOG_ERROR( "Can't delete account \"%s\". There is no account with such name"
				, _accountName.c_str() );
		}

		saveAccountsInfo();
	}
	//////////////////////////////////////////////////////////////////////////
	void Game::selectAccount( const String& _accountName )
	{
		TAccountMap::iterator it_find = m_accounts.find( _accountName );

		if( it_find != m_accounts.end() )
		{
			m_currentAccount = it_find->second;
			m_currentAccount->load();
			m_currentAccount->apply();
			saveAccountsInfo();
		}
		else
		{
			MENGE_LOG_ERROR( "Can't select account \"%s\". There is no account with such name"
				, _accountName.c_str() );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	Account* Game::getCurrentAccount()
	{
		return m_currentAccount;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Game::loaderAccounts_( const String& _iniFile )
	{
		ConfigFile accountsConfig;
		if( accountsConfig.load( "user", _iniFile ) == false )
		{
			return false;
		}
		ConfigFile::TSettings settings = accountsConfig.getSettings( "ACCOUNTS" );
		for( ConfigFile::TSettings::iterator it = settings.begin(), it_end = settings.end();
			it != it_end;
			it++ )
		{
			if( it->first == "AccountName" )
			{
				m_accountNames.push_back( it->second );
			}
			else if( it->first == "DefaultAccountName" )
			{
				m_defaultAccountName = it->second;
			}
		}
		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void Game::loadAccounts()
	{
		m_loadingAccounts = true;

		String accFilename = "Accounts.ini";
		bool accountsExist = Holder<FileEngine>::hostage()
								->existFile( "user", accFilename );
		if( accountsExist == true )
		{
			if( loaderAccounts_( accFilename ) == false )
			{
				MENGE_LOG_ERROR( "Parsing Accounts ini failed \"%s\""
					, accFilename.c_str() );
				return;
			}

			for( TStringVector::iterator it = m_accountNames.begin(), it_end = m_accountNames.end();
				it != it_end;
				it++ )
			{
				createAccount( (*it) );
			}

			if( m_defaultAccountName != "" )
			{
				selectAccount( m_defaultAccountName );
			}
		}

		m_loadingAccounts = false;
	}
	//////////////////////////////////////////////////////////////////////////
	void Game::saveAccountsInfo()
	{
		FileOutputInterface* outFile = Holder<FileEngine>::hostage()
										->openFileOutput( "user", "Accounts.ini" );

		if( outFile == NULL )
		{
			MENGE_LOG_ERROR( "Accounts info wouldn't be saved. Can't open file for writing" );
			return;
		}
		Utils::fileWrite( outFile, "[ACCOUNTS]\n" );
		for( TStringVector::iterator it = m_accountNames.begin(), it_end = m_accountNames.end();
			it != it_end;
			it++ )
		{
			Utils::fileWrite( outFile, "AccountName = " + (*it) + "\n" );
		}

		if( m_currentAccount != 0 )
		{
			Utils::fileWrite( outFile, "DefaultAccountName = " + m_currentAccount->getName() + "\n" );
		}

		Holder<FileEngine>::hostage()
			->closeFileOutput( outFile );
	}
	//////////////////////////////////////////////////////////////////////////
	void Game::saveAccount( const String& _accountName )
	{
		TAccountMap::iterator it_find = m_accounts.find( _accountName );

		if( it_find != m_accounts.end() )
		{
			it_find->second->save();
		}
		else
		{
			MENGE_LOG_ERROR( "Warning: Account \"%s\" does not exist. Can't save"
				, _accountName.c_str() );
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

		ResourceImageDefault * image = Holder<ResourceManager>::hostage()
			->createResourceWithParamT<ResourceImageDefault>( "ResourceImageDefault", param );

		image->addImagePath( "CreateImage" );
		image->incrementReference();
		//ResourceImageDynamic * image = new ResourceImageDynamic( param );
		//image->setSize( mt::vec2f( 1.0f, 1.0f ) );
		//image->incrementReference();

		Holder<ResourceManager>::hostage()
			->registerResource( image );
	}
	//////////////////////////////////////////////////////////////////////////
	void Game::removePredefinedResources_()
	{
		Holder<ResourceManager>::hostage()
			->directResourceRelease("WhitePixel");
	}
	//////////////////////////////////////////////////////////////////////////
	const TStringVector& Game::getScriptsPaths() const
	{
		return m_pathScripts;
	}
	//////////////////////////////////////////////////////////////////////////
	const TStringVector& Game::getArrowPaths() const
	{
		return m_pathArrows;
	}
	//////////////////////////////////////////////////////////////////////////
	const TStringVector& Game::getEntitiesPaths() const
	{
		return m_pathEntities;
	}
	//////////////////////////////////////////////////////////////////////////
	const TStringVector& Game::getScenesPaths() const
	{
		return m_pathScenes;
	}
	//////////////////////////////////////////////////////////////////////////
	const TStringVector& Game::getResourcesPaths() const
	{
		return m_pathResource;
	}
	//////////////////////////////////////////////////////////////////////////
	const TStringVector& Game::getTextsPaths() const
	{
		return m_pathText;
	}
	////////////////////////////////////////////////////////////////////////////
	//std::pair< String, String > Game::getPathEntity( const String& _name ) const
	//{
	//	TMapDeclaration::const_iterator it_find = m_mapEntitiesDeclaration.find( _name );

	//	if( it_find == m_mapEntitiesDeclaration.end() )
	//	{
	//		//return Utils::emptyString();
	//		std::make_pair( "", "" );
	//	}

	//	//String xml_path = it_find->second;

	//	//xml_path += '/';
	//	//xml_path += _name;

	//	//return xml_path;
	//	return it_find->second;
	//}
	////////////////////////////////////////////////////////////////////////////
	//String Game::getPathScene( const String& _name ) const
	//{
	//	TMapDeclaration::const_iterator it_find = m_mapScenesDeclaration.find( _name );

	//	if( it_find == m_mapScenesDeclaration.end() )
	//	{
	//		return Utils::emptyString();
	//	}

	//	String xml_path = it_find->second;

	//	xml_path += "/";
	//	xml_path += _name;

	//	return xml_path;
	//}
	////////////////////////////////////////////////////////////////////////////
	//String Game::getPathArrow( const String& _name ) const
	//{
	//	TMapDeclaration::const_iterator it_find = m_mapArrowsDeclaration.find( _name );

	//	if( it_find == m_mapArrowsDeclaration.end() )
	//	{
	//		return Utils::emptyString();
	//	}

	//	String xml_path = it_find->second;

	//	xml_path += "/";
	//	xml_path += _name;

	//	return xml_path;
	//}
	////////////////////////////////////////////////////////////////////////////
	//String Game::getPathResource( const String& _name ) const
	//{
	//	TMapDeclaration::const_iterator it_find = m_mapResourceDeclaration.find( _name );

	//	if( it_find == m_mapResourceDeclaration.end() )
	//	{
	//		return Utils::emptyString();
	//	}

	//	String path = it_find->second;

	//	path += '/';
	//	path += _name;
	//	path += ".resource"; //?

	//	return path;
	//}
	/////////////////////////////////////////////////////////////////////////
	const TStringVector& Game::getResourceFilePaths() const
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

		for( TStringVector::iterator it = m_pathScripts.begin(),
			it_end = m_pathScripts.end(); it != it_end; it++ )
		{
			m_listModulePath.push_back( m_baseDir + "/" + _pakPath + "/" + *it );
		}

		for( TStringVector::iterator it = m_pathEntities.begin(),
			it_end = m_pathEntities.end(); it != it_end; it++ )
		{
			m_listModulePath.push_back( m_baseDir + "/" + _pakPath + "/" + *it );
		}

		for( TStringVector::iterator it = m_pathScenes.begin(),
			it_end = m_pathScenes.end(); it != it_end; it++ )
		{
			m_listModulePath.push_back( m_baseDir + "/" + _pakPath + "/" + *it );
		}

		for( TStringVector::iterator it = m_pathArrows.begin(),
			it_end = m_pathArrows.end(); it != it_end; it++ )
		{
			m_listModulePath.push_back( m_baseDir + "/" + _pakPath + "/" + *it );
		}

		Holder<ScriptEngine>::hostage()
			->addModulePath( m_listModulePath );

		for( TMapDeclaration::iterator
			it = m_mapEntitiesDeclaration.begin(),
			it_end = m_mapEntitiesDeclaration.end();
		it != it_end;
		it++ )
		{
			Holder<ScriptEngine>::hostage()
				->registerEntityType( it->second.first, it->second.second, it->first );
		}


		for( TStringVector::iterator it = m_pathText.begin(),
			it_end = m_pathText.end(); it != it_end; it++ )
		{
			Holder<TextManager>::hostage()
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

			Holder<ResourceManager>::hostage()
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
		//for( TStringVector::iterator it = m_resourcePaths.begin(),
		//	it_end = m_resourcePaths.end();
		//	it != it_end;
		//it++ )
		//{
		//	loadPak( *it );
		//}
		FileEngine* fileEngine = Holder<FileEngine>::hostage();
		for( TResourcePakVector::iterator it = m_preloadPaks.begin(), it_end = m_preloadPaks.end();
			it != it_end;
			++it )
		{
			ResourcePak& pak = (*it);
			if( fileEngine->mountFileSystem( pak.name, pak.path, false ) == false )
			{
				MENGE_LOG_ERROR( "Error: failed to mount pak \"%s\": \"%s\"",
					pak.name.c_str(), pak.path.c_str() );
				continue;
			}
			loadPak( pak.name, pak.path, pak.description );
		}
		/*if( ( m_languagePack.preload == true ) && ( m_languagePack.path.empty() == false ) )
		{
			loadPak( m_languagePack.name, m_languagePack.path, m_languagePack.description );
		}*/
	}
	//////////////////////////////////////////////////////////////////////////
	void Game::setLanguagePack( const String& _packName )
	{
		//m_languagePack = _packName;
		m_languagePack.path = _packName;
	}
	//////////////////////////////////////////////////////////////////////////
}
