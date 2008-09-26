#	include "Game.h"

#	include "SceneManager.h"

#	include "Scene.h"
#	include "Player.h"
#	include "Arrow.h"
#	include "Amplifier.h"

//#	include "MousePickerSystem.h"
#	include "LightSystem.h"

#	include "ScriptEngine.h"
#	include "FileEngine.h"
#	include "RenderEngine.h"
#	include "ResourceManager.h"
#	include "ScheduleManager.h"
#	include "LogEngine.h"
#	include "RenderEngine.h"
#	include "ProfilerEngine.h"

#	include "XmlEngine.h"
#	include "Utils.h"
#	include "ConfigFile.h"
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
		//Holder<ScheduleManager>::destroy();
		//Holder<MousePickerSystem>::destroy();
		Holder<LightSystem>::destroy();
	}
	//////////////////////////////////////////////////////////////////////////
	/*void Game::loader( XmlElement * _xml )
	{
		XML_SWITCH_NODE( _xml )
		{
			XML_CASE_NODE( "Game" )
			{
				XML_PARSE_ELEMENT( this, &Game::loaderGame_ );
			}
		}
	}*/
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
		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	/*void Game::loaderGame_(XmlElement *_xml)
	{	
		XML_SWITCH_NODE( _xml )
		{
			XML_CASE_ATTRIBUTE_NODE( "ResourceResolution", "Value", m_resourceResolution );
			XML_CASE_ATTRIBUTE_NODE( "Title", "Value", m_title );
			XML_CASE_ATTRIBUTE_NODE( "FixedContentResolution", "Value", m_fixedContentResolution );
			XML_CASE_ATTRIBUTE_NODE( "PhysicSystem", "Value", m_physicSystemName );
			XML_CASE_ATTRIBUTE_NODE( "Width", "Value", m_resolution[0] );					
			XML_CASE_ATTRIBUTE_NODE( "Height", "Value", m_resolution[1] );
			XML_CASE_ATTRIBUTE_NODE( "Bits", "Value", m_bits );
			XML_CASE_ATTRIBUTE_NODE( "Fullscreen", "Value", m_fullScreen );
			XML_CASE_ATTRIBUTE_NODE( "VSync", "Value", m_vsync );
			XML_CASE_ATTRIBUTE_NODE( "TextureFiltering", "Value", m_textureFiltering );
			XML_CASE_ATTRIBUTE_NODE( "FSAAType", "Value", m_FSAAType );
			XML_CASE_ATTRIBUTE_NODE( "FSAAQuality", "Value", m_FSAAQuality );
			XML_CASE_ATTRIBUTE_NODE( "DefaultArrow", "Value", m_defaultArrowName );
			XML_CASE_ATTRIBUTE_NODE( "PersonalityModule", "Value", m_personality );
			XML_CASE_ATTRIBUTE_NODE( "InitFunction", "Value", m_eventInit );
			XML_CASE_ATTRIBUTE_NODE( "UpdateFunction", "Value", m_eventUpdate );
			XML_CASE_ATTRIBUTE_NODE( "FinilizeFunction", "Value", m_eventFini );
		}
	}*/
	//////////////////////////////////////////////////////////////////////////
	void Game::readResourceFile( const String& _file )
	{
		m_currentResourcePath = _file.substr( 0, _file.find_last_of( '/' ) + 1 );

		if( Holder<XmlEngine>::hostage()
			->parseXmlFileM( _file, this, &Game::loaderResourceFile ) == false )
		{
			MENGE_LOG_ERROR << "Invalid resource file " << _file;
			//return false;
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
				
				m_pathScenes.push_back( std::make_pair( m_currentResourcePath, path ) );

				XML_PARSE_ELEMENT( this, &Game::loaderScenes_ );
			}

			XML_CASE_NODE( "Arrows" )
			{
				String path;
				XML_FOR_EACH_ATTRIBUTES()
				{
					XML_CASE_ATTRIBUTE( "Path", path );
				}
				m_pathArrows.push_back( std::make_pair( m_currentResourcePath, path ) );

				XML_PARSE_ELEMENT( this, &Game::loaderArrows_ );
			}
			
			XML_CASE_NODE( "Entities" )
			{
				String path;
				XML_FOR_EACH_ATTRIBUTES()
				{
					XML_CASE_ATTRIBUTE( "Path", path );
				}
				m_pathEntities.push_back( std::make_pair( m_currentResourcePath, path ) );

				XML_PARSE_ELEMENT( this, &Game::loaderEntities_ );
			}

			XML_CASE_NODE( "Resource" )
			{
				String path;
				XML_FOR_EACH_ATTRIBUTES()
				{
					XML_CASE_ATTRIBUTE( "Path", path );
				}
				m_pathResource.push_back( std::make_pair( m_currentResourcePath, path ) );

				XML_PARSE_ELEMENT( this, &Game::loaderResources_ );
			}

			XML_CASE_NODE( "ResourceLocation" )
			{
				String path;
				XML_FOR_EACH_ATTRIBUTES()
				{
					XML_CASE_ATTRIBUTE( "Path", path );
					m_listResourceLocation.push_back( std::make_pair( m_currentResourcePath, path ) );
				}
			}

			XML_CASE_NODE( "Scripts" )
			{
				String path;
				XML_FOR_EACH_ATTRIBUTES()
				{
					XML_CASE_ATTRIBUTE( "Path", path );
				}
				m_pathScripts.push_back( std::make_pair( m_currentResourcePath, path ) );
			}
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void Game::loaderScenes_( XmlElement * _xml )
	{
		XML_SWITCH_NODE( _xml )
		{
			const TPairDeclaration & pair = m_pathScenes.back();
			m_mapScenesDeclaration[ XML_TITLE_NODE ] = pair;
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void Game::loaderArrows_( XmlElement * _xml )
	{
		XML_SWITCH_NODE( _xml )
		{
			TPairDeclaration & pair = m_pathArrows.back();
			m_mapArrowsDeclaration[ XML_TITLE_NODE ] = pair;
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void Game::loaderEntities_( XmlElement * _xml )
	{
		XML_SWITCH_NODE( _xml )
		{
			TPairDeclaration & pair = m_pathEntities.back();
			m_mapEntitiesDeclaration[ XML_TITLE_NODE ] = pair;
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void Game::loaderResources_( XmlElement * _xml )
	{
		XML_SWITCH_NODE( _xml )
		{
			TPairDeclaration & pair = m_pathResource.back();
			m_mapResourceDeclaration[ XML_TITLE_NODE ] = pair;
		}
	}
	//////////////////////////////////////////////////////////////////////////
	bool Game::handleKeyEvent( unsigned int _key, unsigned int _char, bool _isDown )
	{
		bool handle = false;

		if( !handle )
		{
			askEvent( handle, EVENT_KEY, "(IIb)", _key, _char, _isDown );
		}

		if( !handle )
		{
			handle = m_player->handleKeyEvent( _key, _char, _isDown );
		}	

		//RenderImageInterface* image = Holder<RenderEngine>::hostage()->createImage( "shot", 200, 200 );
		//int rect[4] = { 10, 10, 210, 210 };
		//Holder<RenderEngine>::hostage()->render(image, rect);
		//image->writeToFile( "Shot.bmp" );

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

		Holder<ScheduleManager>::hostage()
			->update( _timing );

		StringA eventUpdate;
#	ifdef MENGE_UNICODE
		eventUpdate = Utils::WToA( m_eventUpdate );
#	else if
		eventUpdate = m_eventUpdate;
#	endif


		if( m_pyPersonality && Holder<ScriptEngine>::hostage()
			->hasModuleFunction( m_pyPersonality, eventUpdate ) )
		{
			Holder<ScriptEngine>::hostage()
				->callModuleFunction( m_pyPersonality, eventUpdate, "(f)", _timing );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void Game::render( unsigned int _debugMask )
	{
		Holder<ProfilerEngine>::hostage()->beginProfile( "Render" );
		m_player->render( _debugMask );

		Holder<ProfilerEngine>::hostage()->displayStats();

		if(Holder<ProfilerEngine>::hostage()->isEnabled())
		{
			static TChar m_debugText[128];
			std::sprintf( m_debugText, "FPS=%f", m_FPS );
			Holder<RenderEngine>::hostage()->renderText( m_debugText,mt::vec2f::zero_v2,0xFFFFFFFF);
		}

		Holder<ProfilerEngine>::hostage()->endProfile( "Render" );
	}
	//////////////////////////////////////////////////////////////////////////
	String Game::getPathEntities( const String& _entity ) const
	{
		TMapDeclaration::const_iterator it_find = m_mapEntitiesDeclaration.find( _entity );

		if( it_find == m_mapEntitiesDeclaration.end() )
		{
			return Utils::emptyString();
		}

		return it_find->second.first + it_find->second.second;
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

		registerEvent( EVENT_KEY, "onHandleKeyEvent", m_pyPersonality );
		registerEvent( EVENT_MOUSE_BUTTON, "onHandleMouseButtonEvent", m_pyPersonality );
		registerEvent( EVENT_MOUSE_MOVE, "onHandleMouseMove", m_pyPersonality );

		loadAccounts();

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Game::init()
	{
		//Holder<MousePickerSystem>::keep( new MousePickerSystem );
		Holder<LightSystem>::keep( new LightSystem );
		for( TMapDeclaration::iterator
				it = m_mapResourceDeclaration.begin(),
				it_end = m_mapResourceDeclaration.end();
				it != it_end;
				it++ )
		{
			String path = it->second.first;
			path += it->second.second;
			path += '/';
			path += it->first;
			path += ".resource";

			Holder<ResourceManager>::hostage()
				->loadResource( it->second.first, path, it->first );
		}

		for( TMapDeclaration::iterator
			it = m_mapArrowsDeclaration.begin(),
			it_end = m_mapArrowsDeclaration.end();
		it != it_end;
		it++ )
		{
			String path = it->second.first;
			path += it->second.second;

			loadArrow( it->first, path );
		}

		m_defaultArrow = getArrow( m_defaultArrowName );

		if( m_player->init( m_resourceResolution ) == false )
		{
			return false;
		}

		StringA eventInit;
#	ifdef MENGE_UNICODE
		eventInit = Utils::WToA( m_eventInit );
#	else if
		eventInit = m_eventInit;
#	endif

		if( m_pyPersonality&& 
			Holder<ScriptEngine>::hostage()
			->hasModuleFunction( m_pyPersonality, eventInit ) )
		{
			PyObject * pyResult = Holder<ScriptEngine>::hostage()
				->askModuleFunction( m_pyPersonality, eventInit, "()" );

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
		if( m_pyPersonality )
		{
			StringA eventFini;
#	ifdef MENGE_UNICODE
			eventFini = Utils::WToA( m_eventFini );
#	else if
			eventFini = m_eventFini;
#	endif

			Holder<ScriptEngine>::hostage()
				->callModuleFunction( m_pyPersonality, eventFini, "()" );
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
	bool Game::loadArrow( const String& _name, const String& _path )
	{
		String arrowModule = _name;
		arrowModule += ".Arrow";

		Arrow * arrow = Holder<ScriptEngine>::hostage()
			->createArrow( arrowModule );

		if( arrow == 0 )
		{
			MENGE_LOG_ERROR << "Can't create arrow " << _name; 
			return false;
		}

		String xml_path = _path;
		xml_path += "/";
		xml_path += _name;
		xml_path += "/Arrow.xml";

		if( Holder<XmlEngine>::hostage()
			->parseXmlFileM( xml_path, arrow, &Arrow::loader ) == false )
		{

		}

		//arrow->activate();

		m_mapArrow.insert( std::make_pair( _name, arrow ) );

		return true;
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
				MENGE_LOG_ERROR << "Can't create scene " << _name; 
				return 0;
			}

			scene->setName( _name );
			
			String xml_path;

			const TPairDeclaration & pair = m_mapScenesDeclaration[ _name ];

			xml_path = pair.first;
			xml_path += pair.second;
			xml_path += "/";
			xml_path += _name;
			xml_path += "/Scene.xml";

			if( Holder<XmlEngine>::hostage()
				->parseXmlFileM( xml_path, scene, &Scene::loader ) == false )
			{
				MENGE_LOG_ERROR << "Warning: invalid loader xml " << xml_path << " for scene " << _name;
			}

			m_mapScene.insert( std::make_pair( _name, scene ) );

			return scene;
		}

		return it_find->second;
	}
	//////////////////////////////////////////////////////////////////////////
	void Game::destroyScene( const String& _name  )
	{
		TMapScene::iterator it_find = m_mapScene.find( _name );

		if( it_find != m_mapScene.end() )
		{
			it_find->second->destroy();
			m_mapScene.erase( it_find );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	const Resolution & Game::getResourceResolution() const
	{
		return m_resourceResolution;
	}
	//////////////////////////////////////////////////////////////////////////
	const String& Game::getTitle() const
	{
		return m_title;
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
			MENGE_LOG_ERROR << "Warning: Account with name " << _accountName << " already exist. Account not created";
			return;
		}

		Holder<FileEngine>::hostage()->
			createFolder( Holder<FileEngine>::hostage()->getAppDataPath() + "\\" + _accountName );

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
			Holder<ScriptEngine>::hostage()
				->callModuleFunction( m_pyPersonality, ("onCreateAccount"), "(s)", _accountName.c_str() );
		}
		else
		{
			MENGE_LOG_ERROR << "Warning: Personality module has no method 'onCreateAccount'. Ambigous using accounts";
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
				deleteFolder( Holder<FileEngine>::hostage()->getAppDataPath() + "\\" + _accountName );

			delete it_find->second;

			m_accounts.erase( it_find );
			m_accountNames.erase( std::remove( m_accountNames.begin(), m_accountNames.end(), _accountName ) );
		}
		else
		{
			MENGE_LOG_ERROR << "Can't delete account " << _accountName << ". There is no account with such name";
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
		}
		else
		{
			MENGE_LOG_ERROR << "Can't select account \"" << _accountName << "\". There is no account with such name";
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

		String file = Holder<FileEngine>::hostage()->getAppDataPath() + "\\Accounts.ini";
		
		if( Holder<FileEngine>::hostage()->existFile( file ) )
		{
			if( loaderAccounts_( file ) == false )
			{
				MENGE_LOG_ERROR << "Parsing Accounts ini failed " << file;
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
			MENGE_LOG_ERROR << "Warning: Account " << _accountName << " does not exist. Can't save";
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
	void Game::registerResources()
	{
		ScriptEngine::TListModulePath m_listModulePath;

		for( TListDeclaration::iterator it = m_pathScripts.begin(),
			it_end = m_pathScripts.end(); it != it_end; it++ )
		{
			m_listModulePath.push_back( it->first + it->second );
		}

		for( TListDeclaration::iterator it = m_pathEntities.begin(),
			it_end = m_pathEntities.end(); it != it_end; it++ )
		{
			m_listModulePath.push_back( it->first + it->second );
		}

		for( TListDeclaration::iterator it = m_pathScenes.begin(),
			it_end = m_pathScenes.end(); it != it_end; it++ )
		{
			m_listModulePath.push_back( it->first + it->second );
		}

		for( TListDeclaration::iterator it = m_pathArrows.begin(),
			it_end = m_pathArrows.end(); it != it_end; it++ )
		{
			m_listModulePath.push_back( it->first + it->second );
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

	}
	//////////////////////////////////////////////////////////////////////////
}
