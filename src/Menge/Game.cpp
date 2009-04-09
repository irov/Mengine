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

#	include "pybind/pybind.hpp"

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
		, m_FPS( 0.0f )
		, m_hasWindowPanel( true )
	{
		m_player = new Player();
		Holder<Player>::keep( m_player );
		Holder<Amplifier>::keep( new Amplifier() );
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
	bool Game::loader( const String& _iniFile )
	{
		ConfigFile config;
		if( config.load( _iniFile ) == false )
		{
			return false;
		}
		m_title = config.getSetting( "Title", "GAME" );
		m_resourceResolution[0] = config.getSettingUInt( "ResourceResolutionWidth", "GAME" );
		m_resourceResolution[1] = config.getSettingUInt( "ResourceResolutionHeight", "GAME" );
		m_fixedContentResolution = config.getSettingBool( "FixedContentResolution", "GAME" );
		m_personality = config.getSetting( "PersonalityModule", "GAME" );
		m_eventInit = config.getSetting( "InitFunction", "GAME" );
		m_eventUpdate = config.getSetting( "UpdateFunction", "GAME" );
		m_eventFini = config.getSetting( "FinilizeFunction", "GAME" );
		m_defaultArrowName = config.getSetting( "DefaultArrow", "GAME" );
		m_resolution[0] = config.getSettingUInt( "Width", "GAME" );
		m_resolution[1] = config.getSettingUInt( "Height", "GAME" );
		m_bits = config.getSettingInt( "Bits", "GAME" );
		m_fullScreen = config.getSettingBool( "Fullscreen", "GAME" );
		m_resourcePaths = config.getMultiSetting( "ResourceFile", "GAME" );
		if( config.getSetting( "WindowPanel", "GAME" ).empty() == false )
		{
			m_hasWindowPanel = config.getSettingBool( "WindowPanel", "GAME" );
		}
		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void Game::readResourceFile( const String& _file )
	{
		m_currentResourcePath = _file.substr( 0, _file.find_last_of( '/' ) + 1 );

		Holder<FileEngine>::hostage()
			->loadPack( m_currentResourcePath.substr( 0, m_currentResourcePath.length() - 1 ) );

		if( Holder<XmlEngine>::hostage()
			->parseXmlFileM( _file, this, &Game::loaderResourceFile ) == false )
		{
			MENGE_LOG_ERROR( "Invalid resource file \"%s\""
				, _file.c_str() );
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
				
				m_pathScenes.push_back( m_currentResourcePath + path );

				XML_PARSE_ELEMENT( this, &Game::loaderScenes_ );
			}

			XML_CASE_NODE( "Arrows" )
			{
				String path;
				XML_FOR_EACH_ATTRIBUTES()
				{
					XML_CASE_ATTRIBUTE( "Path", path );
				}
				m_pathArrows.push_back( m_currentResourcePath + path );

				XML_PARSE_ELEMENT( this, &Game::loaderArrows_ );
			}
			
			XML_CASE_NODE( "Entities" )
			{
				String path;
				XML_FOR_EACH_ATTRIBUTES()
				{
					XML_CASE_ATTRIBUTE( "Path", path );
				}
				m_pathEntities.push_back( m_currentResourcePath + path );

				XML_PARSE_ELEMENT( this, &Game::loaderEntities_ );
			}

			XML_CASE_NODE( "Resource" )
			{
				String path;
				XML_FOR_EACH_ATTRIBUTES()
				{
					XML_CASE_ATTRIBUTE( "Path", path );
				}
				m_pathResource.push_back( m_currentResourcePath + path );

				XML_PARSE_ELEMENT( this, &Game::loaderResources_ );
			}

			XML_CASE_NODE( "Scripts" )
			{
				String path;
				XML_FOR_EACH_ATTRIBUTES()
				{
					XML_CASE_ATTRIBUTE( "Path", path );
				}
				m_pathScripts.push_back( m_currentResourcePath + path );
			}

			XML_CASE_NODE( "Text" )
			{
				String path;
				XML_FOR_EACH_ATTRIBUTES()
				{
					XML_CASE_ATTRIBUTE( "Path", path );
				}
				m_pathText.push_back( m_currentResourcePath + path );
			}
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void Game::loaderScenes_( XmlElement * _xml )
	{
		XML_SWITCH_NODE( _xml )
		{
			String & sceneFolder = m_pathScenes.back();
			m_mapScenesDeclaration[ XML_TITLE_NODE ] = sceneFolder;
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void Game::loaderArrows_( XmlElement * _xml )
	{
		XML_SWITCH_NODE( _xml )
		{
			String & arrowFolder = m_pathArrows.back();
			m_mapArrowsDeclaration[ XML_TITLE_NODE ] = arrowFolder;
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void Game::loaderEntities_( XmlElement * _xml )
	{
		XML_SWITCH_NODE( _xml )
		{
			String & entityFolder = m_pathEntities.back();
			m_mapEntitiesDeclaration[ XML_TITLE_NODE ] = entityFolder;
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void Game::loaderResources_( XmlElement * _xml )
	{
		XML_SWITCH_NODE( _xml )
		{
			String & resourceFolder = m_pathResource.back();
			m_mapResourceDeclaration[ XML_TITLE_NODE ] = resourceFolder;
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
		static int d = 0;
		d++;
		if( !(d % 10) )
		{
			m_FPS = 1000.0f / _timing;
		}

		Holder<Amplifier>::hostage()->update( _timing );

		m_player->update( _timing );

		if( m_pyPersonality && Holder<ScriptEngine>::hostage()
			->hasModuleFunction( m_pyPersonality, m_eventUpdate ) )
		{
			Holder<ScriptEngine>::hostage()
				->callModuleFunction( m_pyPersonality, m_eventUpdate, "(f)", _timing );
		}
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
	bool Game::init()
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
			->hasModuleFunction( m_pyPersonality, m_eventInit ) )
		{
			PyObject * pyResult = Holder<ScriptEngine>::hostage()
				->askModuleFunction( m_pyPersonality, m_eventInit, "()" );

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
				->callModuleFunction( m_pyPersonality, m_eventFini, "()" );
		}

		for( TMapArrow::iterator
			it = m_mapArrow.begin(),
			it_end = m_mapArrow.end();
		it != it_end;
		++it)
		{
			it->second->deactivate();
		}

		for( TMapScene::iterator
			it = m_mapScene.begin(),
			it_end = m_mapScene.end();
		it != it_end;
		++it)
		{
			it->second->deactivate();
		}		
	}
	//////////////////////////////////////////////////////////////////////////
	void Game::removeArrow( const String& _name )
	{
		m_mapArrow.erase( _name );
	}
	//////////////////////////////////////////////////////////////////////////
	bool Game::loadArrow( const String& _name )
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

		String xml_path = getPathArrow(_name);
		xml_path += "/Arrow.xml";

		if( Holder<XmlEngine>::hostage()
			->parseXmlFileM( xml_path, arrow, &Arrow::loader ) == false )
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
	Scene * Game::getScene( const String& _name )
	{
		TMapScene::iterator it_find = m_mapScene.find( _name );

		if( it_find == m_mapScene.end() )
		{
			String sceneModule = _name;
			sceneModule += ".Scene";

			Scene * scene = Holder<ScriptEngine>::hostage()
				->createScene( sceneModule );

			if( scene == 0 )
			{
				MENGE_LOG_ERROR( "Can't create scene \"%s\""
					, _name.c_str() ); 

				return 0;
			}

			scene->setName( _name );
			
			String xml_path = getPathScene( _name );
			xml_path += "/Scene.xml";

			if( Holder<XmlEngine>::hostage()
				->parseXmlFileM( xml_path, scene, &Scene::loader ) == false )
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
	void Game::destroyScene( const String& _name  )
	{
		TMapScene::iterator it_find = m_mapScene.find( _name );

		if( it_find != m_mapScene.end() )
		{
			if( it_find->second->decrementReference() == 0 )
			{
				it_find->second->destroy();
				m_mapScene.erase( it_find );
			}
		}
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

		String title = textMgr->getTextEntry( m_title ).text;

		if( title.empty() == true )
		{
			String locTitle = m_title + " [Localize me, please!!!!!!]";
			return locTitle;
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

		String appdatapath = Holder<FileEngine>::hostage()->getAppDataPath();

		Holder<FileEngine>::hostage()->
			createFolder( appdatapath + "/" + _accountName );

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
				deleteFolder( Holder<FileEngine>::hostage()->getAppDataPath() + "/" + _accountName );

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
		if( accountsConfig.load( _iniFile ) == false )
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

		String file = Holder<FileEngine>::hostage()->getAppDataPath() + "/Accounts.ini";
		
		if( Holder<FileEngine>::hostage()->existFile( file ) )
		{
			if( loaderAccounts_( file ) == false )
			{
				MENGE_LOG_ERROR( "Parsing Accounts ini failed \"%s\""
					, file.c_str() );
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
		OutStreamInterface* outStream = Holder<FileEngine>::hostage()
			->openOutStream( "Accounts.ini", false );

		if( outStream == NULL )
		{
			MENGE_LOG_ERROR( "Accounts info wouldn't be saved. Can't open file for writing" );
			return;
		}
		outStream->write( "[ACCOUNTS]\n" );
		for( TStringVector::iterator it = m_accountNames.begin(), it_end = m_accountNames.end();
			it != it_end;
			it++ )
		{
			outStream->write( "AccountName = " + (*it) + "\n" );
		}

		if( m_currentAccount != 0 )
		{
			outStream->write( "DefaultAccountName = " + m_currentAccount->getName() + "\n" );
		}

		Holder<FileEngine>::hostage()
			->closeOutStream( outStream );
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
	void Game::registerResources( const String & _baseDir )
	{
		for( TStringVector::iterator it = m_resourcePaths.begin(),
			it_end = m_resourcePaths.end();
			it != it_end;
			it++ )
		{
			readResourceFile( *it );
		}

		ScriptEngine::TListModulePath m_listModulePath;

		for( TStringVector::iterator it = m_pathScripts.begin(),
			it_end = m_pathScripts.end(); it != it_end; it++ )
		{
			m_listModulePath.push_back( _baseDir + *it );
		}

		for( TStringVector::iterator it = m_pathEntities.begin(),
			it_end = m_pathEntities.end(); it != it_end; it++ )
		{
			m_listModulePath.push_back( _baseDir + *it );
		}

		for( TStringVector::iterator it = m_pathScenes.begin(),
			it_end = m_pathScenes.end(); it != it_end; it++ )
		{
			m_listModulePath.push_back( _baseDir + *it );
		}

		for( TStringVector::iterator it = m_pathArrows.begin(),
			it_end = m_pathArrows.end(); it != it_end; it++ )
		{
			m_listModulePath.push_back( _baseDir + *it );
		}

		Holder<ScriptEngine>::hostage()
			->setModulePath( m_listModulePath );

		for( TMapDeclaration::iterator
			it = m_mapEntitiesDeclaration.begin(),
			it_end = m_mapEntitiesDeclaration.end();
		it != it_end;
		it++ )
		{
			Holder<ScriptEngine>::hostage()
				->registerEntityType( it->first );
		}


		for( TStringVector::iterator it = m_pathText.begin(),
			it_end = m_pathText.end(); it != it_end; it++ )
		{
			Holder<TextManager>::hostage()
				->loadResourceFile( *it );
		}

		for( TMapDeclaration::iterator
				it = m_mapResourceDeclaration.begin(),
				it_end = m_mapResourceDeclaration.end();
				it != it_end;
				it++ )
		{
			String path = getPathResource( it->first );

			m_pathResourceFiles.push_back( path );

			String category = getCategoryResource( path );

			Holder<ResourceManager>::hostage()
				->loadResource( category, it->first, path );
		}

		for( TMapDeclaration::iterator
			it = m_mapArrowsDeclaration.begin(),
			it_end = m_mapArrowsDeclaration.end();
		it != it_end;
		it++ )
		{
			loadArrow( it->first );
		}

		Holder<LightSystem>::keep( new LightSystem );//?
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

		ResourceImageDefault* image = new ResourceImageDefault( param );
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
	const TStringVector& Game::getResourcePaths() const
	{
		return m_resourcePaths;
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
	//////////////////////////////////////////////////////////////////////////
	String Game::getPathEntity( const String& _name ) const
	{
		TMapDeclaration::const_iterator it_find = m_mapEntitiesDeclaration.find( _name );

		if( it_find == m_mapEntitiesDeclaration.end() )
		{
			return Utils::emptyString();
		}

		String xml_path = it_find->second;

		xml_path += '/';
		xml_path += _name;

		return xml_path;
	}
	//////////////////////////////////////////////////////////////////////////
	String Game::getPathScene( const String& _name ) const
	{
		TMapDeclaration::const_iterator it_find = m_mapScenesDeclaration.find( _name );

		if( it_find == m_mapScenesDeclaration.end() )
		{
			return Utils::emptyString();
		}

		String xml_path = it_find->second;

		xml_path += "/";
		xml_path += _name;

		return xml_path;
	}
	//////////////////////////////////////////////////////////////////////////
	String Game::getPathArrow( const String& _name ) const
	{
		TMapDeclaration::const_iterator it_find = m_mapArrowsDeclaration.find( _name );

		if( it_find == m_mapArrowsDeclaration.end() )
		{
			return Utils::emptyString();
		}

		String xml_path = it_find->second;

		xml_path += "/";
		xml_path += _name;

		return xml_path;
	}
	//////////////////////////////////////////////////////////////////////////
	String Game::getPathResource( const String& _name ) const
	{
		TMapDeclaration::const_iterator it_find = m_mapResourceDeclaration.find( _name );

		if( it_find == m_mapResourceDeclaration.end() )
		{
			return Utils::emptyString();
		}

		String path = it_find->second;

		path += '/';
		path += _name;
		path += ".resource"; //?

		return path;
	}
	/////////////////////////////////////////////////////////////////////////
	const TStringVector& Game::getResourceFilePaths() const
	{
		return m_pathResourceFiles;
	}
	//////////////////////////////////////////////////////////////////////////
	String Game::getCategoryResource( const String& _path ) const
	{
		size_t index = _path.find_first_of( '/' );

		if( index == String::npos )
		{
			return Utils::emptyString();
		}

		String category = _path.substr( 0, index + 1 );
		
		return category;
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
		return true;
	}
	//////////////////////////////////////////////////////////////////////////
}
