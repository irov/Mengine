#	include "Game.h"
#	include "ObjectImplement.h"

#	include "Chapter.h"
#	include "Scene.h"
#	include "Player.h"

#	include "FileEngine.h"

#	include "Manager/XmlManager.h"

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

	return Node::addChildren(_node);
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
	}

	Node::loader(_xml);
}
//////////////////////////////////////////////////////////////////////////
bool Game::_compile()
{
	m_player = new Player;

	Chapter *logoChapter = getChildrenT<Chapter>(m_logoChapterName);
	Scene *logoScene = logoChapter->getChildrenT<Scene>(m_logoSceneName);
	m_player->setChapter(logoChapter);
	m_player->setScene(logoScene);

	return true;
}