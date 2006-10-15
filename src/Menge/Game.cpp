#	include "Game.h"

#	include "Chapter.h"

#	include "FileEngine.h"

#	include "Manager/XmlManager.h"

namespace Menge
{
	Game::Game()
	: m_fnInit(0)
	, m_fnUpdate(0)
	, m_fnRender(0)
	, m_currentChapter()
	{
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
	void Game::setChapter(const std::string &_chapter)
	{
		m_currentChapter = getChildrenT<Chapter>(_chapter);

		if( m_currentChapter == 0 )
		{
			//TODO: ERROR
		}

		m_currentChapter->activate();
	}
	//////////////////////////////////////////////////////////////////////////
	bool Game::_activate()
	{
		if( m_startChapter.empty() == false )
		{
			return false;
		}

		setChapter(m_startChapter);

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void Game::loader(TiXmlElement *_xml)
	{
		//<Game>
		//	<Chapters>
		//	<Chapter Value = "Buba"/>
		//	<Chapter Value = "Poba"/>
		//	<Chapters>
		//	</Game>
		XML_FOR_EACH_TREE( _xml )
		{
			XML_CHECK_VALUE_NODE("StartChapter","Name",m_startChapter);
		}

		Node::loader(_xml);
	}
}