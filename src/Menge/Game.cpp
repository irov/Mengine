#	include "Game.h"

#	include "SceneManager.h"

#	include "Scene.h"

#	include "Player.h"

#	include "Arrow.h"

#	include "Amplifier.h"

#	include "ScriptEngine.h"
#	include "FileEngine.h"

#	include "XmlParser.h"

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
	{
		m_player = new Player;

		m_amplifier = new Amplifier();

		m_dialogManager = new DialogManager();

		Holder<Game>::keep(this);
	}
	//////////////////////////////////////////////////////////////////////////
	Game::~Game()
	{
		delete m_dialogManager;

		delete m_amplifier;

		for (TMapArrow::iterator 
			it = m_mapArrow.begin(),
			it_end = m_mapArrow.end();
		it != it_end;
		++it)
		{
			delete it->second;
		}

		for (TMapScene::iterator 
			it = m_mapScene.begin(),
			it_end = m_mapScene.end();
		it != it_end;
		++it)
		{
			delete it->second;
		}

		delete m_player;
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
			//	</Arrows>
			XML_CHECK_NODE_FOR_EACH("Arrows")
			{
				XML_CHECK_NODE("Arrow")
				{
					XML_DEF_ATTRIBUTES_NODE(File);

					Node *_node = SceneManager::createNodeFromXml(File);

					if( _node == 0 )
					{
						ErrorMessage("Invalid file `%s`", File.c_str() );
					}

					Arrow *arrow = dynamic_cast<Arrow*>(_node);

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
			//	<Arrow Type = "Default" />
			//	</Default>
			XML_CHECK_NODE_FOR_EACH("Default")
			{
				XML_CHECK_VALUE_NODE("Arrow", "Type", m_defaultArrowName)
			}

			XML_CHECK_NODE("Personality")
			{
				XML_VALUE_ATTRIBUTE("File", m_personality );

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
	void Game::update( float _timing )
	{
		m_player->update( _timing );

		m_amplifier->update(_timing);

		Holder<ScriptEngine>::hostage()
			->callFunction( m_eventUpdate , "(f)", _timing );
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
	void Game::test( const char * _text )
	{
		printf("%s\n", _text );
	}
	//////////////////////////////////////////////////////////////////////////
	bool Game::init()
	{
		m_defaultArrow = getArrow(m_defaultArrowName);

		ScriptEngine * scriptEngine = 
			Holder<ScriptEngine>::hostage();

		Holder<ScriptEngine>::hostage()
			->setEntitiesPath( m_pathEntities );

		for( TListEntitysDeclaration::iterator 
			it = m_listEntitiesDeclaration.begin(),
			it_end = m_listEntitiesDeclaration.end();
		it != it_end;
		++it)
		{
			Holder<ScriptEngine>::hostage()
				->registerEntityType( *it );			
		}

		if( Holder<ScriptEngine>::hostage()
			->doFile( m_personality ) == false )
		{
			return false;
		}

		bool result = 
			Holder<ScriptEngine>::hostage()
			->callFunctionBool( m_eventInit );

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
			->callFunction( m_eventFini );

		for (TMapArrow::iterator 
			it = m_mapArrow.begin(),
			it_end = m_mapArrow.end();
		it != it_end;
		++it)
		{
			it->second->deactivate();
		}

		for (TMapScene::iterator 
			it = m_mapScene.begin(),
			it_end = m_mapScene.end();
		it != it_end;
		++it)
		{
			it->second->deactivate();
		}		
	}
	//////////////////////////////////////////////////////////////////////////
	void Game::addArrow( Arrow *_arrow )
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
	void Game::removeArrow( const std::string &_name )
	{
		m_mapArrow.erase(_name);
	}
	//////////////////////////////////////////////////////////////////////////
	Arrow * Game::getDefaultArrow()
	{
		return m_defaultArrow;
	}
	//////////////////////////////////////////////////////////////////////////
	Arrow * Game::getArrow( const std::string &_name )
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
	//////////////////////////////////////////////////////////////////////////
	void Game::setCurrentScene( const std::string & _name )
	{
		Scene * scene = getScene( _name );

		if( scene )
		{

		}
	}
}
