#	include "Game.h"

#	include "SceneManager.h"

#	include "Scene.h"
#	include "Player.h"
#	include "Arrow.h"
#	include "Amplifier.h"

#	include "MousePickerSystem.h"
#	include "LightSystem.h"

#	include "ScriptEngine.h"
#	include "FileEngine.h"
#	include "RenderEngine.h"
#	include "ResourceManager.h"
#	include "ScheduleManager.h"
#	include "LogEngine.h"
#	include "RenderEngine.h"

#	include "XmlEngine.h"

//////////////////////////////////////////////////////////////////////////
namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	Game::Game()
		: m_resourceResolution(0.f, 0.f)
		, m_defaultArrow(0)
		, m_pyPersonality(0)
		, m_title("Game")
		, m_fixedContentResolution( false )
		, m_physicSystemName("None")
		, m_width( 1024 )
		, m_height( 768 )
		, m_fullScreen( true )
		, m_vsync( false )
		, m_textureFiltering( true )
		, m_FSAAType( 0 )
		, m_FSAAQuality( 0 )
		, m_currentAccount( 0 )
	{
		m_player = new Player();
		Holder<Player>::keep( m_player );
		Holder<Amplifier>::keep( new Amplifier );
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
		Holder<Player>::empty();
		delete m_player; m_player = NULL;
		Holder<ScheduleManager>::destroy();
		Holder<MousePickerSystem>::destroy();
		Holder<LightSystem>::destroy();
	}
	//////////////////////////////////////////////////////////////////////////
	void Game::loader( XmlElement * _xml )
	{
		XML_SWITCH_NODE( _xml )
		{
			XML_CASE_NODE( "Game" )
			{
				XML_PARSE_ELEMENT( this, &Game::loaderGame_ );
			}
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void Game::loaderGame_(XmlElement *_xml)
	{	
		XML_SWITCH_NODE( _xml )
		{

			XML_CASE_ATTRIBUTE_NODE( "ResourceResolution", "Value", m_resourceResolution );
			XML_CASE_ATTRIBUTE_NODE( "Title", "Name", m_title );
			XML_CASE_ATTRIBUTE_NODE( "FixedContentResolution", "Value", m_fixedContentResolution );
			XML_CASE_ATTRIBUTE_NODE( "PhysicSystem", "Name", m_physicSystemName );
			XML_CASE_ATTRIBUTE_NODE( "Width", "Value", m_width );					
			XML_CASE_ATTRIBUTE_NODE( "Height", "Value", m_height );
			XML_CASE_ATTRIBUTE_NODE( "Bits", "Value", m_bits );
			XML_CASE_ATTRIBUTE_NODE( "Fullscreen", "Value", m_fullScreen );
			XML_CASE_ATTRIBUTE_NODE( "VSync", "Value", m_vsync );
			XML_CASE_ATTRIBUTE_NODE( "TextureFiltering", "Value", m_textureFiltering );
			XML_CASE_ATTRIBUTE_NODE( "FSAAType", "Value", m_FSAAType );
			XML_CASE_ATTRIBUTE_NODE( "FSAAQuality", "Value", m_FSAAQuality );

			XML_CASE_NODE("Default")
			{
				XML_PARSE_ELEMENT( this, &Game::loaderDefault_ );
			}

			XML_CASE_NODE("Personality")
			{
				XML_FOR_EACH_ATTRIBUTES()
				{
					XML_CASE_ATTRIBUTE( "Module", m_personality );
				}

				XML_PARSE_ELEMENT( this, &Game::loaderPersonality_ );
			}

		}
	}
	//////////////////////////////////////////////////////////////////////////
	void Game::readResourceFile( const std::string& _file )
	{
		m_currentResourcePath = _file.substr( 0, _file.find_last_of('/') + 1 );

		if( Holder<XmlEngine>::hostage()
			->parseXmlFileM( _file, this, &Game::loaderResourceFile ) == false )
		{
			MENGE_LOG("Invalid resource file [%s] ...\n", _file.c_str() );
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

			XML_CASE_NODE("Scenes")
			{
				std::string path;
				XML_FOR_EACH_ATTRIBUTES()
				{
					XML_CASE_ATTRIBUTE( "Path", path );
				}
				
				m_pathScenes.push_back( std::make_pair( m_currentResourcePath, path ) );

				XML_PARSE_ELEMENT( this, &Game::loaderScenes_ );
			}

			XML_CASE_NODE("Arrows")
			{
				std::string path;
				XML_FOR_EACH_ATTRIBUTES()
				{
					XML_CASE_ATTRIBUTE( "Path", path );
				}
				m_pathArrows.push_back( std::make_pair( m_currentResourcePath, path ) );

				XML_PARSE_ELEMENT( this, &Game::loaderArrows_ );

			}
			
			XML_CASE_NODE("Entities")
			{
				std::string path;
				XML_FOR_EACH_ATTRIBUTES()
				{
					XML_CASE_ATTRIBUTE( "Path", path );
				}
				m_pathEntities.push_back( std::make_pair( m_currentResourcePath, path ) );

				XML_PARSE_ELEMENT( this, &Game::loaderEntities_ );
			}

			XML_CASE_NODE("Resource")
			{
				std::string path;
				XML_FOR_EACH_ATTRIBUTES()
				{
					XML_CASE_ATTRIBUTE( "Path", path );
				}
				m_pathResource.push_back( std::make_pair( m_currentResourcePath, path ) );

				XML_PARSE_ELEMENT( this, &Game::loaderResources_ );
			}

			XML_CASE_NODE("ResourceLocation")
			{
				std::string path;
				XML_FOR_EACH_ATTRIBUTES()
				{
					XML_CASE_ATTRIBUTE( "Path", path );
					m_listResourceLocation.push_back( std::make_pair( m_currentResourcePath, path ) );
				}
			}

			XML_CASE_NODE("Scripts")
			{
				std::string path;
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
			//m_listScenesDeclaration.push_back( XML_TITLE_NODE );
			const TPairDeclaration & pair = m_pathScenes.back();
			m_mapScenesDeclaration[ XML_TITLE_NODE ] = pair;
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void Game::loaderArrows_( XmlElement * _xml )
	{
		XML_SWITCH_NODE( _xml )
		{
			//m_listArrowsDeclaration.push_back( XML_TITLE_NODE );
			TPairDeclaration & pair = m_pathArrows.back();
			m_mapArrowsDeclaration[ XML_TITLE_NODE ] = pair;
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void Game::loaderEntities_( XmlElement * _xml )
	{
		XML_SWITCH_NODE( _xml )
		{
			//m_listEntitiesDeclaration.push_back( XML_TITLE_NODE );
			TPairDeclaration & pair = m_pathEntities.back();
			m_mapEntitiesDeclaration[ XML_TITLE_NODE ] = pair;
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void Game::loaderResources_( XmlElement * _xml )
	{
		XML_SWITCH_NODE( _xml )
		{
			//m_listResourceDeclaration.push_back( XML_TITLE_NODE );
			TPairDeclaration & pair = m_pathResource.back();
			m_mapResourceDeclaration[ XML_TITLE_NODE ] = pair;
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void Game::loaderDefault_( XmlElement * _xml )
	{
		XML_SWITCH_NODE( _xml )
		{
			XML_CASE_ATTRIBUTE_NODE( "Arrow", "Name", m_defaultArrowName );
		}		
	}
	//////////////////////////////////////////////////////////////////////////
	void Game::loaderPersonality_( XmlElement * _xml )
	{
		XML_SWITCH_NODE( _xml )
		{
			XML_CASE_ATTRIBUTE_NODE( "Init", "Function", m_eventInit );
			XML_CASE_ATTRIBUTE_NODE( "Update", "Function", m_eventUpdate );
			XML_CASE_ATTRIBUTE_NODE( "Fini", "Function", m_eventFini );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	bool Game::handleKeyEvent( unsigned int _key, unsigned int _char, bool _isDown )
	{
		bool handle = false;

		if( !handle )
		{
			askEvent( handle, "KEY", "(IIb)", _key, _char, _isDown );
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
			askEvent( handle, "MOUSE_BUTTON", "(Ib)", _button, _isDown );
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
			askEvent( handle, "MOUSE_MOVE", "(ffi)", _x, _y, _whell );
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
				->callModuleFunction( m_pyPersonality, "onMouseLeave" );
		}
		m_player->onMouseLeave();
	}
	//////////////////////////////////////////////////////////////////////////
	void Game::handleMouseEnter()
	{
		if( m_pyPersonality && Holder<ScriptEngine>::hostage()
			->hasModuleFunction( m_pyPersonality, "onMouseEnter" ) )
		{
			Holder<ScriptEngine>::hostage()
				->callModuleFunction( m_pyPersonality, "onMouseEnter" );
		}
		m_player->onMouseEnter();
	}
	//////////////////////////////////////////////////////////////////////////
	void Game::update( float _timing )
	{
		m_player->update( _timing );

		Holder<ScheduleManager>::hostage()
			->update( _timing );

		if( m_pyPersonality && Holder<ScriptEngine>::hostage()
			->hasModuleFunction( m_pyPersonality, m_eventUpdate ) )
		{
			Holder<ScriptEngine>::hostage()
				->callModuleFunction( m_pyPersonality, m_eventUpdate, "(f)", _timing );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void Game::render( bool _enableDebug )
	{
		m_player->render( _enableDebug );
	}
	//////////////////////////////////////////////////////////////////////////
	std::string Game::getPathEntities( const std::string& _entity ) const
	{
		TMapDeclaration::const_iterator it_find = m_mapEntitiesDeclaration.find( _entity );

		if( it_find == m_mapEntitiesDeclaration.end() )
		{
			static std::string empty;
			return empty;
		}

		return it_find->second.first + it_find->second.second;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Game::init()
	{
		ScriptEngine::TListModulePath m_listModulePath;

		//m_listModulePath.push_back( m_pathScripts );
		for( TListDeclaration::iterator it = m_pathScripts.begin(),
				it_end = m_pathScripts.end(); it != it_end; it++ )
		{
			m_listModulePath.push_back( it->first + it->second );
		}
		//m_listModulePath.push_back( m_pathEntities );
		for( TListDeclaration::iterator it = m_pathEntities.begin(),
			it_end = m_pathEntities.end(); it != it_end; it++ )
		{
			m_listModulePath.push_back( it->first + it->second );
		}

		//m_listModulePath.push_back( m_pathScenes );
		for( TListDeclaration::iterator it = m_pathScenes.begin(),
			it_end = m_pathScenes.end(); it != it_end; it++ )
		{
			m_listModulePath.push_back( it->first + it->second );
		}

		//m_listModulePath.push_back( m_pathArrows );	
		for( TListDeclaration::iterator it = m_pathArrows.begin(),
			it_end = m_pathArrows.end(); it != it_end; it++ )
		{
			m_listModulePath.push_back( it->first + it->second );
		}


		Holder<ScriptEngine>::hostage()
			->setModulePath( m_listModulePath );

		Holder<ScheduleManager>::keep( new ScheduleManager );
		Holder<MousePickerSystem>::keep( new MousePickerSystem );
		Holder<LightSystem>::keep( new LightSystem );

		for( TMapDeclaration::iterator
			it = m_mapEntitiesDeclaration.begin(),
			it_end = m_mapEntitiesDeclaration.end();
		it != it_end;
		it++ )
		{
			Holder<ScriptEngine>::hostage()
				->registerEntityType( it->first );
		}

		for( TMapDeclaration::iterator
				it = m_mapResourceDeclaration.begin(),
				it_end = m_mapResourceDeclaration.end();
				it != it_end;
				it++ )
		{
			std::string path = it->second.first;
			path += it->second.second;
			path += '/';
			path += it->first;
			path += ".resource";

			Holder<ResourceManager>::hostage()
				->loadResource( it->second.first, path );
		}


		for( TMapDeclaration::iterator
			it = m_mapArrowsDeclaration.begin(),
			it_end = m_mapArrowsDeclaration.end();
		it != it_end;
		it++ )
		{
			std::string path = it->second.first;
			path += it->second.second;

			loadArrow( it->first, path );
		}

		m_defaultArrow = getArrow( m_defaultArrowName );

		m_pyPersonality = Holder<ScriptEngine>::hostage()
			->importModule( m_personality );

		if( m_pyPersonality == 0 )
		{
			return false;
		}


		loadAccounts();

		registerEvent( "KEY", "onHandleKeyEvent", m_pyPersonality );
		registerEvent( "MOUSE_BUTTON", "onHandleMouseButtonEvent", m_pyPersonality );
		registerEvent( "MOUSE_MOVE", "onHandleMouseMove", m_pyPersonality );

		m_player->init();

		bool result = false;

		if( Holder<ScriptEngine>::hostage()
			->hasModuleFunction( m_pyPersonality, m_eventInit ) )
		{
			PyObject * pyResult = Holder<ScriptEngine>::hostage()
				->callModuleFunction( m_pyPersonality, m_eventInit );

			result = Holder<ScriptEngine>::hostage()
				->parseBool( pyResult );
		}

		if( result == false )
		{
			return false;
		}

		//Holder<ResourceManager>::hostage()->addListener( this );

		return result;
	}
	//////////////////////////////////////////////////////////////////////////
	void Game::release()
	{
		if( m_pyPersonality )
		{
			Holder<ScriptEngine>::hostage()
				->callModuleFunction( m_pyPersonality, m_eventFini );
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

		//Holder<ResourceManager>::hostage()->removeListener( this );
	}
	//////////////////////////////////////////////////////////////////////////
	void Game::removeArrow( const std::string & _name )
	{
		m_mapArrow.erase( _name );
	}
	//////////////////////////////////////////////////////////////////////////
	Arrow * Game::getDefaultArrow()
	{
		return m_defaultArrow;
	}
	//////////////////////////////////////////////////////////////////////////
	Arrow * Game::getArrow( const std::string & _name )
	{
		TMapArrow::iterator it_find = m_mapArrow.find( _name );

		if( it_find == m_mapArrow.end() )
		{
			return 0;
		}

		return it_find->second;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Game::loadArrow( const std::string & _name, const std::string & _path )
	{
		std::string arrowModule = _name;
		arrowModule += ".Arrow";

		Arrow * arrow = Holder<ScriptEngine>::hostage()
			->createArrow( arrowModule );

		if( arrow == 0 )
		{
			MENGE_LOG("Can't create arrow [%s]\n"
				, _name.c_str() 
				);

			return false;
		}

		std::string xml_path = _path;
		xml_path += "/";
		xml_path += _name;
		xml_path += "/Arrow.xml";

		if( Holder<XmlEngine>::hostage()
			->parseXmlFileM( xml_path, arrow, &Arrow::loader ) == false )
		{

		}

		arrow->activate();

		m_mapArrow.insert( std::make_pair( _name, arrow ) );

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	Scene * Game::getScene(const std::string & _name )
	{
		TMapScene::iterator it_find = m_mapScene.find( _name );

		if( it_find == m_mapScene.end() )
		{
			std::string sceneModule = _name;
			sceneModule += ".Scene";

			Scene * scene = Holder<ScriptEngine>::hostage()
				->createScene( sceneModule );

			if( scene == 0 )
			{
				MENGE_LOG("Can't create scene [%s]\n"
					, _name.c_str() 
					);

				return 0;
			}

			scene->setName( _name );
			
			std::string xml_path;

			const TPairDeclaration & pair = m_mapScenesDeclaration[ _name ];

			xml_path = pair.first;
			xml_path += pair.second;
			xml_path += "/";
			xml_path += _name;
			xml_path += "/Scene.xml";

			//for( TMapDeclaration::iterator it = m_mapScenesDeclaration.begin(),
			//		it_end = m_mapScenesDeclaration.end();
			//		it != it_end;
			//		it++ )
			//{
			//	if( std::find( it->second.begin(), it->second.end(), _name ) != it->second.end() )
			//	{
			//		xml_path = it->first;
			//		xml_path += "/";
			//		xml_path += _name;
			//		xml_path += "/Scene.xml";
			//	}
			//}
			/*std::string xml_path = m_pathScenes.front();
			xml_path += "/";
			xml_path += _name;
			xml_path += "/Scene.xml";*/

			if( Holder<XmlEngine>::hostage()
				->parseXmlFileM( xml_path, scene, &Scene::loader ) == false )
			{
				MENGE_LOG("Warrning: invalid loader xml '%s' for scene '%s'"
					, xml_path.c_str()
					, _name.c_str()
					);
			}
			//TiXmlDocument * document = Holder<FileEngine>::hostage()
			//	->loadXml( xml_path );

			//XML_FOR_EACH_DOCUMENT( document )
			//{
			//	XML_CHECK_NODE("Scene")
			//	{
			//		scene->loader(XML_CURRENT_NODE);
			//	}
			//}
			//XML_INVALID_PARSE()
			//{
			//	MENGE_LOG("Warrning: invalid loader xml '%s' for scene '%s'/n"
			//		, xml_path.c_str()
			//		, _name.c_str()
			//		);
			//}

			m_mapScene.insert( std::make_pair( _name, scene ) );

			return scene;
		}

		return it_find->second;
	}
	//////////////////////////////////////////////////////////////////////////
	void Game::destroyScene( const std::string & _name  )
	{
		TMapScene::iterator it_find = m_mapScene.find( _name );

		if( it_find != m_mapScene.end() )
		{
			it_find->second->destroy();
			m_mapScene.erase( it_find );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	const mt::vec2f & Game::getResourceResolution() const
	{
		return m_resourceResolution;
	}
	//////////////////////////////////////////////////////////////////////////
	void Game::onResourceLoaded( const std::string& _name )
	{
		if( m_pyPersonality && Holder<ScriptEngine>::hostage()
			->hasModuleFunction( m_pyPersonality, "onHandleResourceLoaded" ) )
		{
			Holder<ScriptEngine>::hostage()
				->callModuleFunction( m_pyPersonality, "onHandleResourceLoaded" );
		}	
	}
	//////////////////////////////////////////////////////////////////////////
	void Game::onResourceUnLoaded()
	{
		if( m_pyPersonality && Holder<ScriptEngine>::hostage()
			->hasModuleFunction( m_pyPersonality, "onHandleResourceUnLoaded" ) )
		{
			Holder<ScriptEngine>::hostage()
				->callModuleFunction( m_pyPersonality, "onHandleResourceUnLoaded" );
		}	
	}
	//////////////////////////////////////////////////////////////////////////
	const std::string& Game::getTitle() const
	{
		return m_title;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Game::isContentResolutionFixed() const
	{
		return m_fixedContentResolution;
	}
	//////////////////////////////////////////////////////////////////////////
	float Game::getWidth() const
	{
		return m_width;
	}
	//////////////////////////////////////////////////////////////////////////
	float Game::getHeight() const
	{
		return m_height;
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
			MENGE_LOG("Warning: Account with name '%s' already exist. Account not created",
				_accountName.c_str() );
			return;
		}

		Holder<FileEngine>::hostage()->
			createFolder( Holder<FileEngine>::hostage()->getAppDataPath() + "\\" + _accountName );

		Account* newAccount = new Account( _accountName );
		m_accounts.insert( std::make_pair( _accountName, newAccount ) );

		m_currentAccount = newAccount;

		if( Holder<ScriptEngine>::hostage()
			->hasModuleFunction( m_pyPersonality, "onCreateAccount" ) )
		{
			Holder<ScriptEngine>::hostage()
				->callModuleFunction( m_pyPersonality, "onCreateAccount", "(s)", _accountName.c_str() );
		}
		else
		{
			MENGE_LOG("Warning: Personality module has no method 'onCreateAccount'. Ambigous using accounts" );
		}

		//m_currentAccount->apply();
	}
	//////////////////////////////////////////////////////////////////////////
	void Game::deleteAccount( const String& _accountName )
	{
		TAccountMap::iterator it = m_accounts.find( _accountName );
		if( it != m_accounts.end() )
		{
			if( m_currentAccount->getName() == _accountName )
			{
				m_currentAccount = 0;
			}

			Holder<FileEngine>::hostage()->
				deleteFolder( Holder<FileEngine>::hostage()->getAppDataPath() + "\\" + _accountName );
			delete it->second;
			m_accounts.erase( it );
		}
		else
		{
			MENGE_LOG("Error: Can't delete account '%s'. There is no account with such name",
				_accountName.c_str() );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void Game::selectAccount( const String& _accountName )
	{
		TAccountMap::iterator it = m_accounts.find( _accountName );
		if( it != m_accounts.end() )
		{
			m_currentAccount = it->second;
			it->second->load();
			it->second->apply();
		}
		else
		{
			MENGE_LOG("Error: Can't select account '%s'. There is no account with such name",
				_accountName.c_str() );
		}

	}
	//////////////////////////////////////////////////////////////////////////
	Account* Game::getCurrentAccount()
	{
		return m_currentAccount;
	}
	//////////////////////////////////////////////////////////////////////////
	void Game::loaderAccounts_( XmlElement* _xml )
	{
		XML_SWITCH_NODE( _xml )
		{
			String accountName;

			XML_CASE_NODE( "Account" )
			{
				XML_FOR_EACH_ATTRIBUTES()
				{
					XML_CASE_ATTRIBUTE( "Name", accountName );
				}
				m_accounts.insert( std::make_pair( accountName, static_cast<Account*>(0) ) );
			}
			XML_CASE_ATTRIBUTE_NODE( "DefaultAccount", "Name", m_defaultAccountName );
		}		
	}
	//////////////////////////////////////////////////////////////////////////
	void Game::loadAccounts()
	{
		String file = Holder<FileEngine>::hostage()->getAppDataPath() + "\\" + "Accounts.ini";
		
		if( Holder<FileEngine>::hostage()->existFile( file ) )
		{
			if( Holder<XmlEngine>::hostage()
				->parseXmlFileM( file, this, &Game::loaderAccounts_ ) == false )
			{
				MENGE_LOG("Parsing Accounts xml failed '%s'\n"
					, file.c_str()
					);
				return;
			}
			TStringVector accountNames;

			for( TAccountMap::iterator it = m_accounts.begin(), it_end = m_accounts.end();
				it != it_end;
				it++ )
			{
				accountNames.push_back( it->first );
			}
			m_accounts.clear();

			for( TStringVector::iterator it = accountNames.begin(), it_end = accountNames.end();
				it != it_end;
				it++ )
			{
				createAccount( (*it) );
			}

			selectAccount( m_defaultAccountName );
		}

	}
	//////////////////////////////////////////////////////////////////////////
	void Game::saveAccountsInfo()
	{
		OutStreamInterface* outStream = Holder<FileEngine>::hostage()->
			openOutStream( "Accounts.ini", false );

		outStream->write( "<Accounts>\n" );

		for( TAccountMap::iterator it = m_accounts.begin(), it_end = m_accounts.end();
			it != it_end;
			it++ )
		{
			outStream->write( "\t<Account Name = \"" + it->first + "\"/>\n" );
		}
		outStream->write( "\t<DefaultAccount Name = \"" + m_currentAccount->getName() + "\"/>\n" );

		outStream->write( "</Accounts>" );

		Holder<FileEngine>::hostage()->closeOutStream( outStream );

		for( TAccountMap::iterator it = m_accounts.begin(), it_end = m_accounts.end();
			it != it_end;
			it++ )
		{
			it->second->save();
		}
	}
	//////////////////////////////////////////////////////////////////////////
}
