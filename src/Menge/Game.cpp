#	include "Game.h"

#	include "SceneManager.h"

#	include "Chapter.h"
#	include "Scene.h"
#	include "Player.h"
#	include "Arrow.h"

#	include "BackgroundSound.h"

#	include "FileEngine.h"

#	include "XmlParser.h"
#	include "ErrorMessage.h"

//////////////////////////////////////////////////////////////////////////
namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	Game::Game()
		: m_fnInit(0)
		, m_fnUpdate(0)
		, m_fnRender(0)
		, m_backsoundManager(0)
	{
		m_player = new Player;

		m_backsoundManager = new BackgroundSound();

		Keeper<Game>::keep(this);
	}
	//////////////////////////////////////////////////////////////////////////
	Game::~Game()
	{
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
	void Game::playList(const std::string& _name)
	{
		assert(!"ahtung");
		std::string	fileNameTrack;
		//грузим все плейлисты из какого-то файлика
		XML_PARSE_FILE_EX("tracks.xml")
		{
			XML_CHECK_NODE_FOR_EACH("Tracks")
			{
				XML_CHECK_VALUE_NODE("Track","File",fileNameTrack)
				{
					if(fileNameTrack == _name)
					{
						///как получить ноду?
						////Node* p = getChildren(fileNameTrack);
						XML_PARSE_FILE_EX(fileNameTrack)
						{
							XML_CHECK_NODE_FOR_EACH("Tracks")
							{
								XML_CHECK_VALUE_NODE("Track","File",fileNameTrack)
								{
								}
							}
						}
						//получаем имя плейлиста (PlatList*)nodepl
						//
						//nodepl->push_back()
	//					createNodeFromXml(fileNameTrack);
					}
				}
			}
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void Game::update( float _timing )
	{
		m_player->update( _timing );


		static bool fuck = true;

		if (fuck)
		{
			playList("logoSceneMusic.xml");
		//m_backSound->play(pl);

		fuck = false;
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
	void Game::loader(TiXmlElement *_xml)
	{	
		XML_FOR_EACH_TREE( _xml )
		{
			//<Chapter File = "Game/Chapters/Buba/Chapter.xml" />
			XML_CHECK_NODE("Chapter")
			{
				XML_DEF_ATTRIBUTES_NODE(File);

				SceneManager *sceneMgr = Keeper<SceneManager>::hostage();

				Chapter *chapter = sceneMgr->createNodeFromXmlT<Chapter>(File);
				sceneMgr->loadNode(chapter, File);

				addChapter(chapter);
			}


			//<Logo Chapter = "Buba" Scene = "Buka" />
			XML_CHECK_NODE("Logo")
			{
				XML_VALUE_ATTRIBUTE("Chapter", m_logoChapterName);
				XML_VALUE_ATTRIBUTE("Scene", m_logoSceneName);
			}


			//<Arrows>
			//	<Arrow File = "Game/Arrows/Default.xml" />
			//	</Arrows>
			XML_CHECK_NODE_FOR_EACH("Arrows")
			{
				XML_CHECK_NODE("Arrow")
				{
					XML_DEF_ATTRIBUTES_NODE(File);

					Node *_node = Keeper<SceneManager>::hostage()
						->createNodeFromXml(File);

					if( _node == 0 )
					{
						ErrorMessage("Invalide file `%s`", File.c_str() );
					}

					Arrow *arrow = dynamic_cast<Arrow*>(_node);

					if( arrow == 0 )
					{
						const std::string &name = _node->getName();
						ErrorMessage("Invalide arrow type - `%s` from file `%s`"
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
	bool Game::compile()
	{
		Chapter *logoChapter = getChapter(m_logoChapterName);
		Scene *logoScene = logoChapter->getChildrenT<Scene>(m_logoSceneName);

		Arrow *defaultArrow = getArrow(m_defaultArrowName);

		m_player->setChapter(logoChapter);
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
