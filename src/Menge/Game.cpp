#	include "Game.h"
#	include "ObjectImplement.h"

#	include "SceneManager.h"

#	include "Chapter.h"
#	include "Scene.h"
#	include "Player.h"
#	include "Arrow.h"

#	include "FileEngine.h"

#	include "XmlParser.h"
#	include "ErrorMessage.h"

//////////////////////////////////////////////////////////////////////////
OBJECT_IMPLEMENT(Game);
//////////////////////////////////////////////////////////////////////////
Game::Game()
: m_fnInit(0)
, m_fnUpdate(0)
, m_fnRender(0)
{
	Keeper<Game>::keep(this);

	//m_childrenForeach = false;
}
//////////////////////////////////////////////////////////////////////////
bool Game::addChildren(Node *_node)
{
	if( dynamic_cast<Chapter*>(_node) == 0 )
	{
		return false;
	}

	return NodeImpl::addChildren(_node);
}
//////////////////////////////////////////////////////////////////////////
void Game::update( float _timing )
{
	m_player->update( _timing );
}
//////////////////////////////////////////////////////////////////////////
void Game::render()
{
	m_player->render();
}
//////////////////////////////////////////////////////////////////////////
void Game::_debugRender()
{
	m_player->debugRender();
}
//////////////////////////////////////////////////////////////////////////
void Game::loader(TiXmlElement *_xml)
{	
	XML_FOR_EACH_TREE( _xml )
	{
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

	NodeImpl::loader(_xml);
}
//////////////////////////////////////////////////////////////////////////
bool Game::_compile()
{
	m_player = new Player;

	Chapter *logoChapter = getChildrenT<Chapter>(m_logoChapterName);
	Scene *logoScene = logoChapter->getChildrenT<Scene>(m_logoSceneName);

	Arrow *defaultArrow = getArrow(m_defaultArrowName);

	m_player->setChapter(logoChapter);
	m_player->setScene(logoScene);
	m_player->setArrow(defaultArrow);

	return true;
}
//////////////////////////////////////////////////////////////////////////
void Game::_release()
{
	for (TMapArrow::iterator 
		it = m_mapArrow.begin(),
		it_end = m_mapArrow.end();
	it != it_end;
	++it)
	{
		it->second->release();
	}

	delete m_player;
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
