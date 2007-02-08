#	include "Game.h"

#	include "SceneManager.h"

#	include "Chapter.h"

#	include "Scene.h"

#	include "Player.h"

#	include "Arrow.h"

#	include "BacksoundManager.h"

#	include "XmlParser.h"

#	include "ErrorMessage.h"

#	include "DialogManager.h"

//////////////////////////////////////////////////////////////////////////
namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	Game::Game()
		: m_fnInit(0)
		, m_fnUpdate(0)
		, m_fnRender(0)
		, m_backsoundManager(0)
		, m_dialogManager(0)
	{
		m_player = new Player;

		m_backsoundManager = new BacksoundManager();

		m_dialogManager = new DialogManager();

		Keeper<Game>::keep(this);
	}
	//////////////////////////////////////////////////////////////////////////
	Game::~Game()
	{
		delete m_dialogManager;

		delete m_backsoundManager;



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

				SceneManager *sceneMgr = Keeper<SceneManager>::hostage();

				Chapter *chapter = SceneManager::createNodeFromXmlT<Chapter>(File);
				sceneMgr->loadNode(chapter, File);

				addChapter(chapter);
			}


			//<Logo Chapter = "Buba" Scene = "Buka" />
			XML_CHECK_NODE("Logo")
			{
				XML_VALUE_ATTRIBUTE("Chapter", m_logoChapterName);
				XML_VALUE_ATTRIBUTE("Scene", m_logoSceneName);
			}

			XML_CHECK_NODE("BacksoundManager")
			{
				std::string	playlistFilename;
				XML_VALUE_ATTRIBUTE("File", playlistFilename);
				m_backsoundManager->loadPlayList(playlistFilename);
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
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void Game::update( float _timing )
	{
		static bool test = true;

		if (test)
		{
			m_backsoundManager->setFadeTime(6);
			m_backsoundManager->playList("logoSceneMusic.xml");
			test = false;
		}

		m_player->update( _timing );

		m_backsoundManager->update(_timing);
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
	bool Game::compile()
	{
		Chapter *logoChapter = getChapter(m_logoChapterName);
		Scene *logoScene = logoChapter->getChildrenT<Scene>(m_logoSceneName);

		Arrow *defaultArrow = getArrow(m_defaultArrowName);

		m_player->setScene(logoScene);
		m_player->setArrow(defaultArrow);

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void Game::release()
	{
		for (TMapArrow::iterator 
			it = m_mapArrow.begin(),
			it_end = m_mapArrow.end();
		it != it_end;
		++it)
		{
			it->second->release();
		}

		for (TMapChapter::iterator 
			it = m_mapChapter.begin(),
			it_end = m_mapChapter.end();
		it != it_end;
		++it)
		{
			it->second->release();
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
