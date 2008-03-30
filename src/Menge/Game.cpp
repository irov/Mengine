#	include "Game.h"

#	include "SceneManager.h"

#	include "Scene.h"
#	include "Player.h"
#	include "Arrow.h"
#	include "Amplifier.h"

#	include "MousePickerSystem.h"

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
#ifndef _DEBUG
		, m_renderDriver("RenderSystem_Direct3D9")
#else
		, m_renderDriver("RenderSystem_Direct3D9_d")
#endif
	{
		Holder<Player>::keep( new Player );
		Holder<Amplifier>::keep( new Amplifier );
	}
	//////////////////////////////////////////////////////////////////////////
	Game::~Game()
	{
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
		Holder<ScheduleManager>::destroy();
		Holder<MousePickerSystem>::destroy();
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
			//<Scenes Path = "Game/Scenes">
			//	<Default/>
			//</Scenes>
			XML_CASE_ATTRIBUTE_NODE( "ResourceResolution", "Value", m_resourceResolution );
			XML_CASE_ATTRIBUTE_NODE( "Title", "Name", m_title );
			XML_CASE_ATTRIBUTE_NODE( "FixedContentResolution", "Value", m_fixedContentResolution );
			XML_CASE_ATTRIBUTE_NODE( "RenderDriver", "Name", m_renderDriver );
			XML_CASE_ATTRIBUTE_NODE( "PhysicSystem", "Name", m_physicSystemName );
			XML_CASE_ATTRIBUTE_NODE( "Width", "Value", m_width );					
			XML_CASE_ATTRIBUTE_NODE( "Height", "Value", m_height );
			XML_CASE_ATTRIBUTE_NODE( "Bits", "Value", m_bits );
			XML_CASE_ATTRIBUTE_NODE( "Fullscreen", "Value", m_fullScreen );
			XML_CASE_ATTRIBUTE_NODE( "VSync", "Value", m_vsync );
			/*XML_CASE_NODE("Scenes")
			{
				XML_FOR_EACH_ATTRIBUTES()
				{
					XML_CASE_ATTRIBUTE( "Path", m_pathScenes );
				}

				XML_PARSE_ELEMENT( this, &Game::loaderScenes_ );
			}*/
			//<Arrows Path = "Game/Arrows">
			//	<Default/>
			//</Arrows>
			/*XML_CASE_NODE("Arrows")
			{
				XML_FOR_EACH_ATTRIBUTES()
				{
					XML_CASE_ATTRIBUTE( "Path", m_pathArrows );
				}

				XML_PARSE_ELEMENT( this, &Game::loaderArrows_ );

			}*/

			//<Default>
			//	<Arrow Type = "Default"/>
			//</Default>
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

			/*XML_CASE_NODE("Entities")
			{
				XML_FOR_EACH_ATTRIBUTES()
				{
					XML_CASE_ATTRIBUTE( "Path", m_pathEntities );
				}

				XML_PARSE_ELEMENT( this, &Game::loaderEntities_ );
			}

			XML_CASE_NODE("Resource")
			{
				XML_FOR_EACH_ATTRIBUTES()
				{
					XML_CASE_ATTRIBUTE( "Path", m_pathResource );
				}

				XML_PARSE_ELEMENT( this, &Game::loaderResources_ );
			}

			XML_CASE_NODE("ResourceLocation")
			{
				std::string path("");
				XML_FOR_EACH_ATTRIBUTES()
				{
					XML_CASE_ATTRIBUTE( "Path", path );
					m_listResourceLocation.push_back( path );
				}
			}

			XML_CASE_ATTRIBUTE_NODE( "Scripts", "Path", m_pathScripts );*/
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
				m_pathScenes.push_back( m_currentResourcePath + path );

				XML_PARSE_ELEMENT( this, &Game::loaderScenes_ );
			}

			XML_CASE_NODE("Arrows")
			{
				std::string path;
				XML_FOR_EACH_ATTRIBUTES()
				{
					XML_CASE_ATTRIBUTE( "Path", path );
				}
				m_pathArrows.push_back( m_currentResourcePath + path );

				XML_PARSE_ELEMENT( this, &Game::loaderArrows_ );

			}
			
			XML_CASE_NODE("Entities")
			{
				std::string path;
				XML_FOR_EACH_ATTRIBUTES()
				{
					XML_CASE_ATTRIBUTE( "Path", path );
				}
				m_pathEntities.push_back( m_currentResourcePath + path );

				XML_PARSE_ELEMENT( this, &Game::loaderEntities_ );
			}

			XML_CASE_NODE("Resource")
			{
				std::string path;
				XML_FOR_EACH_ATTRIBUTES()
				{
					XML_CASE_ATTRIBUTE( "Path", path );
				}
				m_pathResource.push_back( m_currentResourcePath + path );

				XML_PARSE_ELEMENT( this, &Game::loaderResources_ );
			}

			XML_CASE_NODE("ResourceLocation")
			{
				std::string path;
				XML_FOR_EACH_ATTRIBUTES()
				{
					XML_CASE_ATTRIBUTE( "Path", path );
					m_listResourceLocation.push_back( m_currentResourcePath + path );
				}
			}

			XML_CASE_NODE("Scripts")
			{
				std::string path;
				XML_FOR_EACH_ATTRIBUTES()
				{
					XML_CASE_ATTRIBUTE( "Path", path );
				}
				m_pathScripts.push_back( m_currentResourcePath + path );
			}
		}

	}
	//////////////////////////////////////////////////////////////////////////
	void Game::loaderScenes_( XmlElement * _xml )
	{
		XML_SWITCH_NODE( _xml )
		{
			//m_listScenesDeclaration.push_back( XML_TITLE_NODE );
			m_mapScenesDeclaration[ m_pathScenes.back() ].push_back( XML_TITLE_NODE );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void Game::loaderArrows_( XmlElement * _xml )
	{
		XML_SWITCH_NODE( _xml )
		{
			//m_listArrowsDeclaration.push_back( XML_TITLE_NODE );
			m_mapArrowsDeclaration[ m_pathArrows.back() ].push_back( XML_TITLE_NODE );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void Game::loaderEntities_( XmlElement * _xml )
	{
		XML_SWITCH_NODE( _xml )
		{
			//m_listEntitiesDeclaration.push_back( XML_TITLE_NODE );
			m_mapEntitiesDeclaration[ m_pathEntities.back() ].push_back( XML_TITLE_NODE );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void Game::loaderResources_( XmlElement * _xml )
	{
		XML_SWITCH_NODE( _xml )
		{
			//m_listResourceDeclaration.push_back( XML_TITLE_NODE );
			m_mapResourceDeclaration[ m_pathResource.back() ].push_back( XML_TITLE_NODE );
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
			handle = Holder<Player>::hostage()
				->handleKeyEvent( _key, _char, _isDown );
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
			handle = Holder<Player>::hostage()->handleMouseButtonEvent( _button, _isDown );
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
			handle = Holder<Player>::hostage()
				->handleMouseMove( _x, _y, _whell );
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
	}
	//////////////////////////////////////////////////////////////////////////
	void Game::update( float _timing )
	{
		Holder<Player>::hostage()
			->update( _timing );

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
	void Game::render()
	{
		Holder<Player>::hostage()
			->render();
	}
	//////////////////////////////////////////////////////////////////////////
	void Game::debugRender()
	{
		Holder<Player>::hostage()
			->debugRender();
	}
	//////////////////////////////////////////////////////////////////////////
	const std::string & Game::getPathEntities( const std::string& _entity ) const
	{
		for( TMapDeclaration::const_iterator it = m_mapEntitiesDeclaration.begin(),
				it_end = m_mapEntitiesDeclaration.end();
				it != it_end;
				it++ )
		{
			if( std::find( it->second.begin(), it->second.end(), _entity ) != it->second.end() )
			{
				return it->first;
			}
		}
		return m_pathEntities.front();
	}
	//////////////////////////////////////////////////////////////////////////
	const std::string & Game::getPathArrows() const
	{
		return m_pathArrows.front();
	}
	//////////////////////////////////////////////////////////////////////////
	void Game::setParamString( const std::string& _params )
	{
		if( _params.empty() == false )
		{
			Holder<Player>::hostage()
					->setCurrentScene( _params );
		}
		else
		{
		}
	}
	//////////////////////////////////////////////////////////////////////////
	bool Game::init()
	{
		Holder<RenderEngine>::hostage()->setContentResolution( m_resourceResolution );

		ScriptEngine::TListModulePath m_listModulePath;

		//m_listModulePath.push_back( m_pathScripts );
		for( TListDeclaration::iterator it = m_pathScripts.begin(),
				it_end = m_pathScripts.end(); it != it_end; it++ )
		{
			m_listModulePath.push_back( *it );
		}
		//m_listModulePath.push_back( m_pathEntities );
		for( TListDeclaration::iterator it = m_pathEntities.begin(),
			it_end = m_pathEntities.end(); it != it_end; it++ )
		{
			m_listModulePath.push_back( *it );
		}

		//m_listModulePath.push_back( m_pathScenes );
		for( TListDeclaration::iterator it = m_pathScenes.begin(),
			it_end = m_pathScenes.end(); it != it_end; it++ )
		{
			m_listModulePath.push_back( *it );
		}

		//m_listModulePath.push_back( m_pathArrows );	
		for( TListDeclaration::iterator it = m_pathArrows.begin(),
			it_end = m_pathArrows.end(); it != it_end; it++ )
		{
			m_listModulePath.push_back( *it );
		}


		Holder<ScriptEngine>::hostage()
			->setModulePath( m_listModulePath );

		Holder<ScheduleManager>::keep( new ScheduleManager );
		Holder<MousePickerSystem>::keep( new MousePickerSystem );

		for( TMapDeclaration::iterator
			it = m_mapEntitiesDeclaration.begin(),
			it_end = m_mapEntitiesDeclaration.end();
		it != it_end;
		it++ )
		{
			for( TListDeclaration::iterator
				itl = it->second.begin(),
				itl_end = it->second.end();
			itl != itl_end;
			itl++)
			{
				Holder<ScriptEngine>::hostage()
					->registerEntityType( *itl );
			}
		}
		/*for( TListDeclaration::iterator
			it = m_listEntitiesDeclaration.begin(),
			it_end = m_listEntitiesDeclaration.end();
		it != it_end;
		++it)			
		{
			Holder<ScriptEngine>::hostage()
				->registerEntityType( *it );
		}

		for( TListDeclaration::iterator
			it = m_listResourceDeclaration.begin(),
			it_end = m_listResourceDeclaration.end();
		it != it_end;
		++it)	
		{
			std::string path = m_pathResource.front();
			path += '/';
			path += *it;
			path += ".resource";

			Holder<ResourceManager>::hostage()
				->loadResource( path );
		}*/
		for( TMapDeclaration::iterator
				it = m_mapResourceDeclaration.begin(),
				it_end = m_mapResourceDeclaration.end();
				it != it_end;
				it++ )
		{
			for( TListDeclaration::iterator
					itl = it->second.begin(),
					itl_end = it->second.end();
					itl != itl_end;
					itl++)
			{
				std::string path = it->first;
				path += '/';
				path += *itl;
				path += ".resource";

				Holder<ResourceManager>::hostage()
					->loadResource( path );
			}
		}

		/*for( TListDeclaration::iterator
			it = m_listArrowsDeclaration.begin(),
			it_end = m_listArrowsDeclaration.end();
		it != it_end;
		++it)	
		{
			getArrow( *it );
		}*/
		for( TMapDeclaration::iterator
			it = m_mapArrowsDeclaration.begin(),
			it_end = m_mapArrowsDeclaration.end();
		it != it_end;
		it++ )
		{
			for( TListDeclaration::iterator
				itl = it->second.begin(),
				itl_end = it->second.end();
			itl != itl_end;
			itl++)
			{
				getArrow( *itl );
			}

		}

		/*for( TMapDeclaration::iterator
			it = m_mapScenesDeclaration.begin(),
			it_end = m_mapScenesDeclaration.end();
		it != it_end;
		it++ )
		{
			for( TListDeclaration::iterator
				itl = it->second.begin(),
				itl_end = it->second.end();
			itl != itl_end;
			itl++)
			{
				getScene( *itl );
			}
		}
		/*for( TListDeclaration::iterator
			it = m_listScenesDeclaration.begin(),
			it_end = m_listScenesDeclaration.end();
		it != it_end;
		++it)	
		{
			getScene( *it );
		}*/

		FileEngine* fileEngine = Holder<FileEngine>::hostage();
		for( TListDeclaration::iterator
			it = m_listResourceLocation.begin(),
			it_end = m_listResourceLocation.end();
		it != it_end;
		++it)	
		{
			fileEngine->addResourceLocation( "/" + (*it) );
		}

		if( !m_listResourceLocation.empty() )
		{
			fileEngine->initResources();
		}

		m_defaultArrow = getArrow( m_defaultArrowName );

		m_pyPersonality = Holder<ScriptEngine>::hostage()
			->importModule( m_personality );

		if( m_pyPersonality == 0 )
		{
			return false;
		}

		registerEventListener( "KEY", "onHandleKeyEvent", m_pyPersonality );
		registerEventListener( "MOUSE_BUTTON", "onHandleMouseButtonEvent", m_pyPersonality );
		registerEventListener( "MOUSE_MOVE", "onHandleMouseMove", m_pyPersonality );

		Holder<Player>::hostage()
			->init();

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

		Holder<ResourceManager>::hostage()->addListener( this );

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
			std::string arrowModule = _name;
			arrowModule += ".Arrow";

			Arrow * arrow = Holder<ScriptEngine>::hostage()
				->createArrow( arrowModule );

			if( arrow == 0 )
			{
				MENGE_LOG("Can't create arrow [%s]\n"
					, _name.c_str() 
					);

				return 0;
			}

			std::string xml_path = m_pathArrows.front();
			xml_path += "/";
			xml_path += _name;
			xml_path += "/Arrow.xml";

			if( Holder<XmlEngine>::hostage()
				->parseXmlFileM( xml_path, arrow, &Arrow::loader ) == false )
			{

			}
	
			m_mapArrow.insert( std::make_pair( _name, arrow ) );

			return arrow;
		}

		return it_find->second;
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

			for( TMapDeclaration::iterator it = m_mapScenesDeclaration.begin(),
					it_end = m_mapScenesDeclaration.end();
					it != it_end;
					it++ )
			{
				if( std::find( it->second.begin(), it->second.end(), _name ) != it->second.end() )
				{
					xml_path = it->first;
					xml_path += "/";
					xml_path += _name;
					xml_path += "/Scene.xml";
				}
			}
			/*std::string xml_path = m_pathScenes.front();
			xml_path += "/";
			xml_path += _name;
			xml_path += "/Scene.xml";*/

			if( Holder<XmlEngine>::hostage()
				->parseXmlFileM( xml_path, scene, &Scene::loader ) == false )
			{
				MENGE_LOG("Warrning: invalid loader xml '%s' for scene '%s'/n"
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
	void Game::onResourceLoaded()
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
	const std::string& Game::getRenderDriverName() const
	{
		return m_renderDriver;
	}
	//////////////////////////////////////////////////////////////////////////
	const std::string& Game::getPhysicSystemName() const
	{
		return m_physicSystemName;
	}
	//////////////////////////////////////////////////////////////////////////
}
