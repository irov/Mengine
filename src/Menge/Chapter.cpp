#	include "Chapter.h"

#	include "SceneManager.h"

#	include "Scene.h"

#	include "FileEngine.h"
#	include "Manager/XmlManager.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////()
	Chapter::Chapter(const std::string &_name)
	: m_name(_name)
	, m_currentScene(0)
	{
	}
	//////////////////////////////////////////////////////////////////////////
	bool Chapter::addChildren(Node *_node)
	{
		if( dynamic_cast<Scene*>(_node) == 0 )
		{
			return false;
		}

		return Node::addChildren(_node);
	};
	//////////////////////////////////////////////////////////////////////////
	void Chapter::setScene(const std::string &_name)
	{
		m_currentScene = getChildrenT<Scene>(_name);

		if( m_currentScene == 0 )
		{
			return;
		}

		m_currentScene->start();
	}
	//////////////////////////////////////////////////////////////////////////
	bool Chapter::_activate()
	{
		if( m_startScene.empty() == false )
		{
			return false;
		}

		m_currentScene = 0;

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void Chapter::loader(TiXmlElement *_xml)
	{
		//<Chapter>
		//	<Scenes>
		//	<Scene Value = "Buka"/>
		//	<Scenes>
		//	<Chapter>
		XML_FOR_EACH_TREE( _xml )
		{
			XML_CHECK_VALUE_NODE("StartScene","Name",m_startScene);
		}

		Node::loader(_xml);
	};
}