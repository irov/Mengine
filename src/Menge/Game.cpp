#	include "Game.h"

#	include "SceneManager.h"

#	include "Scene.h"

#	include "Player.h"

#	include "Arrow.h"

#	include "Amplifier.h"

#	include "ScriptEngine.h"

#	include "XmlParser/XmlParser.h"

#	include "ErrorMessage.h"

#	include "DialogManager.h"

//////////////////////////////////////////////////////////////////////////
namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	Game::Game()
		: m_amplifier(0)
		, m_dialogManager(0)
		, m_defaultArrow(0)
		, m_pyPersonality(0)
	{
		m_player = new Player();

		m_amplifier = new Amplifier();

		m_dialogManager = new DialogManager();

		Holder<Game>::keep(this);
	}
	//////////////////////////////////////////////////////////////////////////
	Game::~Game()
	{
		delete m_dialogManager;

		delete m_amplifier;

		for each( const TMapArrow::value_type & it in m_mapArrow )
		{
			it.second->destroy();
		}

		for each( const TMapScene::value_type & it in m_mapScene )
		{
			it.second->destroy();
		}

		delete m_player;

		Holder<ScriptEngine>::hostage()
			->callFunction( m_eventFini );
	}
	//////////////////////////////////////////////////////////////////////////
	void Game::loader(TiXmlElement *_xml)
	{	
		XML_FOR_EACH_TREE( _xml )
		{
			XML_CHECK_NODE_FOR_EACH("Scenes")
			{
				XML_CHECK_NODE("Scene")
				{
					XML_DEF_ATTRIBUTES_NODE( File );
					Scene * scene = SceneManager::createNodeFromXmlT<Scene>( File );
					addScene( scene );
				}
			}

			XML_CHECK_NODE("BacksoundManager")
			{
				std::string	playlistFilename;
				XML_VALUE_ATTRIBUTE("File", playlistFilename);
				m_amplifier->loadPlayList(playlistFilename);
			}

			XML_CHECK_NODE("DialogManager")
			{
				std::string	messagesFilename;
				XML_VALUE_ATTRIBUTE("File", messagesFilename);
				m_dialogManager->loadMessagesList(messagesFilename);
			}

			//<Arrows>
			//	<Arrow File = "Game/Arrows/Default.xml" />
			//</Arrows>
			XML_CHECK_NODE_FOR_EACH("Arrows")
			{
				XML_CHECK_NODE("Arrow")
				{
					XML_DEF_ATTRIBUTES_NODE(File);

					Node * _node = SceneManager::createNodeFromXml(File);

					if( _node == 0 )
					{
						ErrorMessage("Invalid file `%s`", File.c_str() );
					}

					Arrow * arrow = dynamic_cast<Arrow*>(_node);

					if( arrow == 0 )
					{
						const std::string &name = _node->getName();
						ErrorMessage("Invalid arrow type - `%s` from file `%s`"
							, name.c_str()
							, File.c_str() );
					}

					addArrow(arrow);
				}
			}

			//<Default>
			//	<Arrow Type = "Default"/>
			//</Default>
			XML_CHECK_NODE_FOR_EACH("Default")
			{
				XML_CHECK_VALUE_NODE("Arrow", "Type", m_defaultArrowName)
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
		}
	}
	//////////////////////////////////////////////////////////////////////////
	bool Game::handleKeyEvent( size_t _key, bool _isDown )
	{
		bool handle = false;

		if( !handle && Holder<ScriptEngine>::hostage()
			->hasModuleFunction( m_pyPersonality, "onHandleKeyEvent" ) )
		{
			handle = Holder<ScriptEngine>::hostage()
				->callModuleFunction( m_pyPersonality, "onHandleKeyEvent", "(Ib)", _key, _isDown );
		}

		if( !handle && m_player )
		{
			return m_player->handleKeyEvent( _key, _isDown );
		}	

		return false;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Game::handleMouseButtonEvent( size_t _button, bool _isDown )
	{
		bool handle = false;

		if( !handle && Holder<ScriptEngine>::hostage()
			->hasModuleFunction( m_pyPersonality, "onHandleMouseButtonEvent" ) )
		{
			handle = Holder<ScriptEngine>::hostage()
				->callModuleFunction( m_pyPersonality, "onHandleMouseButtonEvent", "(Ib)", _button, _isDown );
		}

		if( !handle && m_player )
		{
			return m_player->handleMouseButtonEvent( _button, _isDown );
		}	

		return false;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Game::handleMouseMove( float _x, float _y, float _whell )
	{
		bool handle = false;

		if( !handle && Holder<ScriptEngine>::hostage()
			->hasModuleFunction( m_pyPersonality, "onHandleMouseMove" ) )
		{
			handle = Holder<ScriptEngine>::hostage()
				->callModuleFunction( m_pyPersonality, "onHandleMouseMove", "(fff)", _x, _y, _whell );
		}
		
		if( !handle && m_player )
		{
			return m_player->handleMouseMove( _x, _y, _whell );
		}		

		return false;
	}
	//////////////////////////////////////////////////////////////////////////
	void Game::update( float _timing )
	{
		m_player->update( _timing );

		m_amplifier->update(_timing);

		if( Holder<ScriptEngine>::hostage()
			->hasModuleFunction( m_pyPersonality, m_eventUpdate ) )
		{
			Holder<ScriptEngine>::hostage()
				->callModuleFunction( m_pyPersonality, m_eventUpdate, "(f)", _timing );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void Game::render()
	{
		m_player->render();
	}
	//////////////////////////////////////////////////////////////////////////
	void Game::debugRender()
	{
		m_player->debugRender();
	}
	//////////////////////////////////////////////////////////////////////////
	const std::string & Game::getPathEntities() const
	{
		return m_pathEntities;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Game::init()
	{
		m_defaultArrow = getArrow(m_defaultArrowName);

		ScriptEngine * scriptEngine = 
			Holder<ScriptEngine>::hostage();

		for each( const std::string & enType in m_listEntitiesDeclaration )			
		{
			Holder<ScriptEngine>::hostage()
				->registerEntityType( enType );
		}

		m_pyPersonality = Holder<ScriptEngine>::hostage()
			->importModule( m_personality );

		if( m_pyPersonality == 0 )
		{
			return false;
		}

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

		m_player->init();

		return result;
	}
	//////////////////////////////////////////////////////////////////////////
	void Game::release()
	{
		Holder<ScriptEngine>::hostage()
			->callModuleFunction( m_pyPersonality, m_eventFini );

		for each( const TMapArrow::value_type & it in m_mapArrow )
		{
			it.second->deactivate();
		}

		for each( const TMapScene::value_type & it in m_mapScene )
		{
			it.second->deactivate();
		}		
	}
	//////////////////////////////////////////////////////////////////////////
	void Game::addArrow( Arrow * _arrow )
	{
		const std::string &name = _arrow->getName();

		if( name.empty() == true )
		{
			ErrorMessage("add invalid arrow");
		}

		TMapArrow::iterator it_find = m_mapArrow.find(name);

		if( it_find != m_mapArrow.end() )
		{
			return;
		}

		m_mapArrow.insert(std::make_pair(name,_arrow));
	}
	//////////////////////////////////////////////////////////////////////////
	void Game::removeArrow( const std::string & _name )
	{
		m_mapArrow.erase(_name);
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
	void Game::addScene( Scene * _scene )
	{
		if( _scene == 0 )
		{
			return;
		}

		const std::string &name = _scene->getName();

		if( name.empty() )
		{
			return;
		}

		TMapScene::iterator it_find = m_mapScene.find( name );

		if( it_find != m_mapScene.end() )
		{
			return;
		}

		Holder<ScriptEngine>::hostage()
			->incref( _scene );

		m_mapScene.insert( std::make_pair( name, _scene ) );
	}
	//////////////////////////////////////////////////////////////////////////
	Scene * Game::getScene(const std::string & _name )
	{
		TMapScene::iterator it_find = m_mapScene.find( _name );

		if( it_find == m_mapScene.end() )
		{
			return 0;
		}

		return it_find->second;
	}
}
