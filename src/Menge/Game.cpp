#	include "Game.h"

#	include "SceneManager.h"

#	include "Chapter.h"

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

		for (TMapChapter::iterator 
			it = m_mapChapter.begin(),
			it_end = m_mapChapter.end();
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
			//<Chapter File = "Game/Chapters/Buba/Chapter.xml" />
			XML_CHECK_NODE("Chapter")
			{
				XML_DEF_ATTRIBUTES_NODE(File);

				SceneManager *sceneMgr = Holder<SceneManager>::hostage();

				Chapter *chapter = SceneManager::createNodeFromXmlT<Chapter>(File);
				sceneMgr->loadNode(chapter, File);

				addChapter(chapter);
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
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void Game::update( float _timing )
	{
		m_player->update( _timing );

		m_amplifier->update(_timing);

		Holder<ScriptEngine>::hostage()
			->callFunctionSafe( m_eventUpdate ) % _timing % lua_boost::ret_safe();
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

		if( Holder<ScriptEngine>::hostage()
			->doFile( m_personality ) == false )
		{
			return false;
		}

		lua_boost::safe_result<bool> result = 
			Holder<ScriptEngine>::hostage()
			->callFunctionSafe( m_eventInit ) % lua_boost::ret_safe<bool>();

		if( result.valid == false || result.result == false )
		{
			return false;
		}

		m_player->init();

		return result.result;
	}
	//////////////////////////////////////////////////////////////////////////
	void Game::release()
	{
		Holder<ScriptEngine>::hostage()
			->callFunctionSafe( m_eventFini ) % lua_boost::ret_safe();

		for (TMapArrow::iterator 
			it = m_mapArrow.begin(),
			it_end = m_mapArrow.end();
		it != it_end;
		++it)
		{
			it->second->deactivate();
		}

		for (TMapChapter::iterator 
			it = m_mapChapter.begin(),
			it_end = m_mapChapter.end();
		it != it_end;
		++it)
		{
			it->second->deactivate();
		}		
	}
	//////////////////////////////////////////////////////////////////////////
	void Game::addArrow(Arrow *_arrow)
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
	void Game::removeArrow(const std::string &_name)
	{
		m_mapArrow.erase(_name);
	}
	//////////////////////////////////////////////////////////////////////////
	Arrow * Game::getDefaultArrow()
	{
		return m_defaultArrow;
	}
	//////////////////////////////////////////////////////////////////////////
	Arrow * Game::getArrow(const std::string &_name)
	{
		TMapArrow::iterator it_find = m_mapArrow.find( _name );

		if( it_find == m_mapArrow.end() )
		{
			return 0;
		}

		return it_find->second;
	}
	//////////////////////////////////////////////////////////////////////////
	void Game::addChapter(Chapter * _chapter)
	{
		if( _chapter == 0 )
		{
			return;
		}

		const std::string &name = _chapter->getName();

		if( name.empty() )
		{
			return;
		}

		TMapChapter::iterator it_find = m_mapChapter.find( name );

		if( it_find != m_mapChapter.end() )
		{
			return;
		}

		m_mapChapter.insert(std::make_pair(name, _chapter));
	}
	//////////////////////////////////////////////////////////////////////////
	Chapter * Game::getChapter(const std::string & _name )
	{
		TMapChapter::iterator it_find = m_mapChapter.find( _name );

		if( it_find == m_mapChapter.end() )
		{
			return 0;
		}

		return it_find->second;
	}
}
