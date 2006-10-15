#	include "Node.h"
#	include "ObjectImplement.h"

#	include "SceneManager.h"

#	include "FileEngine.h"
#	include "Manager/XmlManager.h"

#	include "Utility/algorithm.h"

OBJECT_IMPLEMENT(Node);
//////////////////////////////////////////////////////////////////////////
Node::Node()
: m_parent(0)
, m_active(false)
, m_compile(false)
, m_scriptObject(0)
, m_external(false)
{
}
//////////////////////////////////////////////////////////////////////////
Node::~Node()
{

}
//////////////////////////////////////////////////////////////////////////
bool Node::activate()
{
	Utility::for_each(m_listChildren,&Node::activate);

	if( m_active )
	{
		return true;
	}

	if( m_compile )
	{
		return _activate();
	}

	return false;
}
//////////////////////////////////////////////////////////////////////////
void Node::deactivate()
{
	Utility::for_each(m_listChildren,&Node::deactivate);

	if( m_compile )
	{
		_deactivate();

		m_active = false;
	}
}
//////////////////////////////////////////////////////////////////////////
bool Node::isActive()
{
	return m_active;
}
//////////////////////////////////////////////////////////////////////////
bool Node::compile()
{
	Utility::for_each(m_listChildren,&Node::compile);

	if( m_compile == false )
	{
		m_compile = _compile();
	}

	return m_compile;	
}
//////////////////////////////////////////////////////////////////////////
void Node::release()
{
	Utility::for_each(m_listChildren,&Node::release);

	if( m_compile == true )
	{
		deactivate();

		_release();

		m_compile = false;
	}
}
//////////////////////////////////////////////////////////////////////////
bool Node::isCompile()
{
	return m_compile;
}
//////////////////////////////////////////////////////////////////////////
void Node::setName(const std::string &name)
{
	m_name = name;
}
//////////////////////////////////////////////////////////////////////////
const std::string & Node::getName()const
{	
	return m_name;
}
//////////////////////////////////////////////////////////////////////////
void Node::setType(const std::string &type)
{
	m_type = type;
}
//////////////////////////////////////////////////////////////////////////
const std::string & Node::getType()const
{
	return m_type;
}
//////////////////////////////////////////////////////////////////////////
size_t Node::getTypeId()const
{
	return NodeFactory::getId(m_type);
}
//////////////////////////////////////////////////////////////////////////
void Node::setExternal(bool _value)
{
	m_external = _value;
}
//////////////////////////////////////////////////////////////////////////
bool Node::isExternal()const
{
	return m_external;
}

//////////////////////////////////////////////////////////////////////////
void Node::setResource(const std::string &_file)
{
	m_resource = _file;
}
//////////////////////////////////////////////////////////////////////////
const std::string & Node::getResource()const
{
	return m_resource;
}
//////////////////////////////////////////////////////////////////////////
Node * Node::getParent()
{
	return m_parent;
}
//////////////////////////////////////////////////////////////////////////
bool Node::isRoot()
{
	return m_parent == 0;
}
//////////////////////////////////////////////////////////////////////////
void Node::setParent(Node *node)
{
	m_parent = node;
	updateParent();
}
//////////////////////////////////////////////////////////////////////////
void Node::updateParent()
{
	Utility::for_each(m_listChildren,&Node::updateParent);
}
//////////////////////////////////////////////////////////////////////////
void Node::visitChildren(Visitor::Base *visitor)
{
	Utility::for_each(m_listChildren,&Node::visit,visitor);
}
//////////////////////////////////////////////////////////////////////////
void Node::update(float _timing)
{
	Utility::for_each(m_listChildren,&Node::update,_timing);
}
//////////////////////////////////////////////////////////////////////////
void Node::loader(TiXmlElement *xml)
{
	SceneManager *sceneMgr = Keeper<SceneManager>::hostage();

	XML_FOR_EACH_TREE(xml)
	{
		XML_CHECK_NODE("Node")
		{
			XML_DEF_ATTRIBUTES_NODE(Name);
			XML_DEF_ATTRIBUTES_NODE(Type);

			Node *node = sceneMgr->createNode(Name,Type);

			if(node == 0)
			{
				continue;
			}

			node->loader(XML_CURRENT_NODE);

			addChildren(node);
		}

		XML_CHECK_NODE("EXTERNAL")
		{
			XML_DEF_ATTRIBUTES_NODE(File);
			
			bool loading; 
			XML_VALUE_ATTRIBUTE("Loading",loading);
			
			XML_PARSE_FILE_EX(File)
			{
				if ( loading == true )
				{
					Node::loader(XML_CURRENT_NODE);
				}
				else
				{
					XML_CHECK_NODE("Node")
					{
						XML_DEF_ATTRIBUTES_NODE(Name);
						XML_DEF_ATTRIBUTES_NODE(Type);

						Node *node = sceneMgr->createNode(Name,Type);

						node->setExternal(true);
						node->setResource(File);
					}				
				}
			}
		}
	}
}
//////////////////////////////////////////////////////////////////////////
luabind::adl::object * Node::getScriptable()
{
	return m_scriptObject;
}
//////////////////////////////////////////////////////////////////////////
bool Node::_activate()
{
	return true;
}
//////////////////////////////////////////////////////////////////////////
void Node::_deactivate()
{

}
//////////////////////////////////////////////////////////////////////////
bool Node::_compile()
{
	return true;
}
//////////////////////////////////////////////////////////////////////////
void Node::_release()
{

}
/////////////////////////////////////////////////////////////////////////
Node::TListChildren::iterator Node::_findChildren(const std::string &_name)
{
	struct FNodeFind
	{
	public:
		FNodeFind(const std::string &_findName)
			: m_findName(_findName)
		{
		}

		bool operator()(Node *_node)const
		{
			const std::string & nodeName = 
				_node->getName();

			if( nodeName == m_findName )
			{
				return true;
			}

			return false;
		}

	private:
		const std::string & m_findName;
	};

	TListChildren::iterator it_find = 
		std::find_if(
		m_listChildren.begin(),
		m_listChildren.end(),
		FNodeFind(_name)
		);

	return it_find;
}
/////////////////////////////////////////////////////////////////////////
bool Node::addChildren(Node *_node)
{
	bool bInsert = isChildren(_node);

	if( bInsert == true )
	{
		return false;
	}

	_node->setParent(this);

	m_listChildren.push_back(_node);

	return true;
}
//////////////////////////////////////////////////////////////////////////
bool Node::isChildren(Node *_node)
{
	const std::string & name = _node->getName();

	return isChildren(name);
}
//////////////////////////////////////////////////////////////////////////
bool Node::isChildren(const std::string &_name)
{
	TListChildren::iterator it_find = _findChildren(_name);

	if( it_find == m_listChildren.end() )
	{
		return false;
	}

	return true;
}
//////////////////////////////////////////////////////////////////////////
Node * Node::getChildren(const std::string &_name)
{
	TListChildren::iterator it_find = _findChildren(_name);

	if( it_find == m_listChildren.end() )
	{
		return 0;
	}

	return *it_find;
}
//////////////////////////////////////////////////////////////////////////
void Node::removeChildren(Node *_node)
{
	if( _node == 0 )
	{
		return;
	}

	const std::string &name = _node->getName();

	removeChildren(name);	
}
//////////////////////////////////////////////////////////////////////////
void Node::removeChildren(const std::string &_name)
{
	TListChildren::iterator it_find = _findChildren(_name);

	if( it_find != m_listChildren.end() )
	{
		m_listChildren.erase(it_find);
	}
}
//////////////////////////////////////////////////////////////////////////
void Node::debugRender()
{
	Utility::for_each(m_listChildren,&Node::debugRender);

	_debugRender();
}
//////////////////////////////////////////////////////////////////////////
void Node::_debugRender()
{

}