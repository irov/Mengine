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

#	include "XmlParser/XmlParser.h"

//////////////////////////////////////////////////////////////////////////
namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	Game::Game()
		: m_defaultArrow(0)
		, m_pyPersonality(0)
	{
		Holder<Player>::keep( new Player );
		Holder<Amplifier>::keep( new Amplifier );
	}
	//////////////////////////////////////////////////////////////////////////
	Game::~Game()
	{
		if( m_pyPersonality && Holder<ScriptEngine>::hostage()
			->hasModuleFunction( m_pyPersonality, "fini" ) )
		{
			Holder<ScriptEngine>::hostage()
				->callModuleFunction( m_pyPersonality, "fini" );
		}

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
	void Game::loader(TiXmlElement *_xml)
	{	
		XML_FOR_EACH_TREE( _xml )
		{
			//<Scenes Path = "Game/Scenes">
			//	<Default/>
			//</Scenes>
			XML_CHECK_NODE("Scenes")
			{
				XML_DEF_ATTRIBUTES_NODE( Path );

				XML_VALUE_ATTRIBUTE( "Path", m_pathScenes );

				XML_FOR_EACH()
				{
					m_listScenesDeclaration.push_back( XML_TITLE_NODE );
				}
			}
			//<Arrows Path = "Game/Arrows">
			//	<Default/>
			//</Arrows>
			XML_CHECK_NODE("Arrows")
			{
				XML_DEF_ATTRIBUTES_NODE( Path );

				XML_VALUE_ATTRIBUTE( "Path", m_pathArrows );

				XML_FOR_EACH()
				{
					m_listArrowsDeclaration.push_back( XML_TITLE_NODE );
				}
			}

			//<Default>
			//	<Arrow Type = "Default"/>
			//</Default>
			XML_CHECK_NODE_FOR_EACH("Default")
			{
				XML_CHECK_VALUE_NODE("Arrow", "Name", m_defaultArrowName)
			}

			XML_CHECK_NODE("Personality")
			{
				XML_VALUE_ATTRIBUTE("Module", m_personality );

				XML_FOR_EACH()
				{				
					XML_CHECK_VALUE_NODE("Init","Function", m_eventInit);
					XML_CHECK_VALUE_NODE("Update","Function", m_eventUpdate);
					XML_CHECK_VALUE_NODE("Fini","Function", m_eventFini);
				}
			}

			XML_CHECK_NODE("Entities")
			{
				XML_DEF_ATTRIBUTES_NODE( Path );

				XML_VALUE_ATTRIBUTE( "Path", m_pathEntities );

				XML_FOR_EACH()
				{
					m_listEntitiesDeclaration.push_back( XML_TITLE_NODE );
				}
			}

			XML_CHECK_NODE("Resource")
			{
				XML_DEF_ATTRIBUTES_NODE( Path );

				XML_VALUE_ATTRIBUTE( "Path", m_pathResource );

				XML_FOR_EACH()
				{
					m_listResourceDeclaration.push_back( XML_TITLE_NODE );
				}
			}
			
			XML_CHECK_VALUE_NODE( "Scripts", "Path", m_pathScripts );

			XML_CHECK_VALUE_NODE( "ResourceResolution", "Value", m_resourceResolution );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	bool Game::handleKeyEvent( size_t _key, bool _isDown )
	{

		bool handle = false;

		if( !handle )
		{
			askEvent( handle, "KEY", "(Ib)", _key, _isDown );
		}

		if( !handle )
		{
			handle = Holder<Player>::hostage()
				->handleKeyEvent( _key, _isDown );
		}	

		//RenderImageInterface* image = Holder<RenderEngine>::hostage()->createImage( "shot", 200, 200 );
		//int rect[4] = { 10, 10, 210, 210 };
		//Holder<RenderEngine>::hostage()->render(image, rect);
		//image->writeToFile( "Shot.bmp" );

		return handle;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Game::handleMouseButtonEvent( size_t _button, bool _isDown )
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
	bool Game::handleMouseMove( int _x, int _y, int _whell )
	{
		bool handle = false;

		if( !handle )
		{
			askEvent( handle, "MOUSE_MOVE", "(iii)", _x, _y, _whell );
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
	const std::string & Game::getPathEntities() const
	{
		return m_pathEntities;
	}
	//////////////////////////////////////////////////////////////////////////
	const std::string & Game::getPathArrows() const
	{
		return m_pathArrows;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Game::init()
	{
		Holder<RenderEngine>::hostage()->setContentResolution( m_resourceResolution );

		ScriptEngine::TListModulePath m_listModulePath;

		m_listModulePath.push_back( m_pathScripts );
		m_listModulePath.push_back( m_pathEntities );
		m_listModulePath.push_back( m_pathScenes );
		m_listModulePath.push_back( m_pathArrows );	

		Holder<ScriptEngine>::hostage()
			->setModulePath( m_listModulePath );

		Holder<ScheduleManager>::keep( new ScheduleManager );
		Holder<MousePickerSystem>::keep( new MousePickerSystem );

		for( TListDeclaration::iterator
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
			std::string path = m_pathResource;
			path += '/';
			path += *it;
			path += ".resource";

			Holder<ResourceManager>::hostage()
				->loadResource( path );
		}

		for( TListDeclaration::iterator
			it = m_listArrowsDeclaration.begin(),
			it_end = m_listArrowsDeclaration.end();
		it != it_end;
		++it)	
		{
			getArrow( *it );
		}

		for( TListDeclaration::iterator
			it = m_listScenesDeclaration.begin(),
			it_end = m_listScenesDeclaration.end();
		it != it_end;
		++it)	
		{
			getScene( *it );
		}

		m_defaultArrow = getArrow(m_defaultArrowName);

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

			std::string xml_path = m_pathArrows;
			xml_path += "/";
			xml_path += _name;
			xml_path += "/Arrow.xml";

			TiXmlDocument * document = Holder<FileEngine>::hostage()
				->loadXml( xml_path );

			XML_FOR_EACH_DOCUMENT( document )
			{
				XML_CHECK_NODE("Arrow")
				{
					arrow->loader(XML_CURRENT_NODE);
				}
			}
			XML_INVALID_PARSE()
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

			std::string xml_path = m_pathScenes;
			xml_path += "/";
			xml_path += _name;
			xml_path += "/Scene.xml";

			TiXmlDocument * document = Holder<FileEngine>::hostage()
				->loadXml( xml_path );

			XML_FOR_EACH_DOCUMENT( document )
			{
				XML_CHECK_NODE("Scene")
				{
					scene->loader(XML_CURRENT_NODE);
				}
			}
			XML_INVALID_PARSE()
			{
				MENGE_LOG("Warrning: invalid loader xml '%s' for scene '%s'/n"
					, xml_path.c_str()
					, _name.c_str()
					);
			}

			m_mapScene.insert( std::make_pair( _name, scene ) );

			return scene;
		}

		return it_find->second;
	}
	//////////////////////////////////////////////////////////////////////////
	const mt::vec2f & Game::getResourceResolution() const
	{
		return m_resourceResolution;
	}
	//////////////////////////////////////////////////////////////////////////
}