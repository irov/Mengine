#	include "Node.h"
#	include "ObjectImplement.h"

#	include "SceneManager.h"

#	include "FileEngine.h"
#	include "Manager/XmlManager.h"
#	include "Manager/ErrorMessage.h"

#	include "Utility/algorithm.h"

OBJECT_IMPLEMENT(Node);
//////////////////////////////////////////////////////////////////////////
Node::Node()
: m_parent(0)
, m_active(true)
, m_compile(false)
, m_scriptObject(0)
, m_external(false)
, m_childrenForeach(true)
{
}
//////////////////////////////////////////////////////////////////////////
Node::~Node()
{

}
//////////////////////////////////////////////////////////////////////////
bool Node::activate()
{
	compile();

	if( m_compile == false )
	{
		return false;
	}

	if( m_childrenForeach )
	{
		Utility::for_each(m_listChildren,&Node::activate);
	}

	if( m_active )
	{
		return true;
	}

	return _activate();
}
//////////////////////////////////////////////////////////////////////////
void Node::deactivate()
{
	if( m_childrenForeach )
	{
		Utility::for_each(m_listChildren,&Node::deactivate);
	}

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
	if( m_external && m_compile == false )
	{
		Keeper<SceneManager>::hostage()
			->loadNode(this, m_resource);
	}

	if( m_childrenForeach )
	{
		Utility::for_each(m_listChildren,&Node::compile);
	}

	if( m_compile == true )
	{
		return true;
	}

	m_compile = _compile();

	return m_compile;	
}
//////////////////////////////////////////////////////////////////////////
void Node::release()
{
	if( m_childrenForeach )
	{
		Utility::for_each(m_listChildren,&Node::release);
	}

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
bool Node::isExternal()const
{
	return m_external;
}
//////////////////////////////////////////////////////////////////////////
void Node::setResource(const std::string &_file)
{
	m_external = true;
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
	if( m_parent )
	{
		m_parent->_lostChildren(this,true);
	}

	m_parent = node;
	_updateParent();
}
//////////////////////////////////////////////////////////////////////////
void Node::update(float _timing)
{
	if( m_active == true )
	{
		compile();

		_update(_timing);

		if( m_childrenForeach )
		{
			Utility::for_each(m_listChildren,&Node::update,_timing);
		}
	}
}
//////////////////////////////////////////////////////////////////////////
void Node::loader(TiXmlElement *xml)
{
	XML_FOR_EACH_TREE(xml)
	{
		XML_CHECK_NODE("Node")
		{
			XML_DEF_ATTRIBUTES_NODE(Name);
			XML_DEF_ATTRIBUTES_NODE(Type);

			Node *node = createChildren(Name,Type);

			if(node == 0)
			{
				continue;
			}

			node->loader(XML_CURRENT_NODE);

			addChildren(node);
		}

		XML_CHECK_NODE("External")
		{
			XML_DEF_ATTRIBUTES_NODE(File);
			
			bool loading = false; 
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

						Node *node = createChildren(Name,Type);

						node->setResource(File);

						addChildren(node);
					}				
				}
			}
			XML_INVALID_PARSE()
			{
				ErrorMessage("Invalid parse external node %s for %s", File.c_str(), m_name.c_str());
			}
		}
	}
}
//////////////////////////////////////////////////////////////////////////
luabind::adl::object * Node::getScriptable()
{
	return m_scriptObject;
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
Node * Node::createChildren(const std::string &_name, const std::string &_type)
{
	Node *node = Keeper<SceneManager>::hostage()
		->createNode(_name,_type);

	return node;
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
void Node::foreachFunc(TForEachFunc _func)
{
	std::for_each( m_listChildren.begin(), m_listChildren.end(), std::ptr_fun(_func) );
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
		_lostChildren(*it_find,false);
		m_listChildren.erase(it_find);
	}
}
//////////////////////////////////////////////////////////////////////////
void Node::debugRender()
{
	if( m_childrenForeach )
	{
		Utility::for_each(m_listChildren,&Node::debugRender);
	}

	_debugRender();
}
//////////////////////////////////////////////////////////////////////////
void Node::_debugRender()
{
	//Empty
}
//////////////////////////////////////////////////////////////////////////
void Node::_lostChildren(Node *_node, bool _valid)
{
	//Empty
}
//////////////////////////////////////////////////////////////////////////
void Node::_updateParent()
{
	//Empty
}
//////////////////////////////////////////////////////////////////////////
bool Node::_activate()
{
	//Empty
	return true;
}
//////////////////////////////////////////////////////////////////////////
void Node::_deactivate()
{
	//Empty
}
//////////////////////////////////////////////////////////////////////////
bool Node::_compile()
{
	//Empty
	return true;
}
//////////////////////////////////////////////////////////////////////////
void Node::_release()
{
	//Empty
}
//////////////////////////////////////////////////////////////////////////
void Node::_update(float _timing)
{
	//Empty
}
//////////////////////////////////////////////////////////////////////////
void Node::_visit( Visitor * _visitor )
{
	//Empty
}