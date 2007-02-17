#	include "NodeImpl.h"
#	include "ObjectImplement.h"

#	include "SceneManager.h"

#	include "FileEngine.h"
#	include "ScriptEngine.h"

#	include "lua_boost/lua_functor.h"

#	include "XmlParser.h"
#	include "ErrorMessage.h"

#	include "Utility/algorithm.h"

OBJECT_IMPLEMENT( NodeImpl )
//////////////////////////////////////////////////////////////////////////
NodeImpl::NodeImpl()
: m_parent(0)
, m_active(true)
, m_compile(false)
, m_external(false)
, m_childrenForeach(true)
, m_iteratorChildren(m_listChildren.begin())
{
}
//////////////////////////////////////////////////////////////////////////
NodeImpl::~NodeImpl()
{
	for( TListChildren::iterator 
		it = m_listChildren.begin(),
		it_end = m_listChildren.end();
	it != it_end;
	++it)
	{
		delete *it;
	}
}
//////////////////////////////////////////////////////////////////////////
bool NodeImpl::activate()
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
void NodeImpl::deactivate()
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
bool NodeImpl::isActive()
{
	return m_active;
}
//////////////////////////////////////////////////////////////////////////
bool NodeImpl::compile()
{
	if( m_external && m_compile == false )
	{
		if( m_resource.empty() == false )
		{
			SceneManager::loadNode(this, m_resource);
		}
	}

	if( m_compile == false )
	{
		m_compile = _compile();
	}

	if( m_childrenForeach )
	{
		Utility::for_each(m_listChildren,&Node::compile);
	}

	return m_compile;	
}
//////////////////////////////////////////////////////////////////////////
void NodeImpl::release()
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
bool NodeImpl::isCompile()
{
	return m_compile;
}
//////////////////////////////////////////////////////////////////////////
void NodeImpl::setName(const std::string &name)
{
	m_name = name;
}
//////////////////////////////////////////////////////////////////////////
const std::string & NodeImpl::getName()const
{	
	return m_name;
}
//////////////////////////////////////////////////////////////////////////
void NodeImpl::setType(const std::string &type)
{
	m_type = type;
}
//////////////////////////////////////////////////////////////////////////
const std::string & NodeImpl::getType()const
{
	return m_type;
}
//////////////////////////////////////////////////////////////////////////
size_t NodeImpl::getTypeId()const
{
	return NodeFactory::getId(m_type);
}
//////////////////////////////////////////////////////////////////////////
bool NodeImpl::isExternal()const
{
	return m_external;
}
//////////////////////////////////////////////////////////////////////////
void NodeImpl::setResource(const std::string &_file)
{
	m_external = true;
	m_resource = _file;
}
//////////////////////////////////////////////////////////////////////////
const std::string & NodeImpl::getResource()const
{
	return m_resource;
}
//////////////////////////////////////////////////////////////////////////
Node * NodeImpl::getParent()
{
	return m_parent;
}
//////////////////////////////////////////////////////////////////////////
bool NodeImpl::isRoot()
{
	return m_parent == 0;
}
//////////////////////////////////////////////////////////////////////////
void NodeImpl::setParent(Node *node)
{
	if( m_parent )
	{
		m_parent->_lostChildren(this,true);
	}

	m_parent = node;
	_updateParent();
}
//////////////////////////////////////////////////////////////////////////
void NodeImpl::update(float _timing)
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
void NodeImpl::loader(TiXmlElement *xml)
{
	XML_FOR_EACH_TREE(xml)
	{
		Eventable::loader( XML_CURRENT_NODE );

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
					loader(XML_CURRENT_NODE);
				}
				else
				{
					XML_CHECK_NODE("Node")
					{
						XML_DEF_ATTRIBUTES_NODE(Name);
						XML_DEF_ATTRIBUTES_NODE(Type);

						Node *node = createChildren(Name,Type);

						node->setResource(File);
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
/////////////////////////////////////////////////////////////////////////
NodeImpl::TListChildren::iterator NodeImpl::_findChildren(const std::string &_name)
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
Node * NodeImpl::createChildren(const std::string &_name, const std::string &_type)
{
	Node *node = SceneManager::createNode(_name,_type);

	if( node )
	{
		addChildren(node);
	}

	return node;
}

/////////////////////////////////////////////////////////////////////////
bool NodeImpl::addChildren(Node *_node)
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
bool NodeImpl::isChildren(Node *_node)
{
	const std::string & name = _node->getName();

	return isChildren(name);
}
//////////////////////////////////////////////////////////////////////////
bool NodeImpl::isChildren(const std::string &_name)
{
	TListChildren::iterator it_find = _findChildren(_name);

	if( it_find == m_listChildren.end() )
	{
		return false;
	}

	return true;
}
//////////////////////////////////////////////////////////////////////////
Node * NodeImpl::getChildren(const std::string &_name)
{
	TListChildren::iterator it_find = _findChildren(_name);

	if( it_find == m_listChildren.end() )
	{
		return 0;
	}

	return *it_find;
}
//////////////////////////////////////////////////////////////////////////
Node * NodeImpl::nextChildren()
{
	++m_iteratorChildren;

	if( m_iteratorChildren == m_listChildren.end() )
	{
		return 0;
	}

	return *m_iteratorChildren;
}
//////////////////////////////////////////////////////////////////////////
Node * NodeImpl::beginChildren()
{
	m_iteratorChildren = m_listChildren.begin();
	
	if( m_listChildren.empty() )
	{
		return 0;
	}

	return *m_iteratorChildren;
}
//////////////////////////////////////////////////////////////////////////
void NodeImpl::visitChildren( Visitor *_visitor )
{
	std::for_each( 
		m_listChildren.begin(), 
		m_listChildren.end(), 
		std::bind1st( std::mem_fun( &Visitor::apply ) , _visitor ) );
}
//////////////////////////////////////////////////////////////////////////
void NodeImpl::removeChildren(Node *_node)
{
	if( _node == 0 )
	{
		return;
	}

	const std::string &name = _node->getName();

	removeChildren(name);	
}
//////////////////////////////////////////////////////////////////////////
void NodeImpl::removeChildren(const std::string &_name)
{
	TListChildren::iterator it_find = _findChildren(_name);

	if( it_find != m_listChildren.end() )
	{
		_lostChildren(*it_find,false);
		m_listChildren.erase(it_find);
	}
}
//////////////////////////////////////////////////////////////////////////
void NodeImpl::debugRender()
{
	if( m_childrenForeach )
	{
		Utility::for_each(m_listChildren,&Node::debugRender);
	}

	_debugRender();
}
//////////////////////////////////////////////////////////////////////////
void NodeImpl::registerEvent( const std::string &_name, const lua_boost::lua_functor * _func  )
{
	Eventable::registerEvent( _name, _func );
}
//////////////////////////////////////////////////////////////////////////
const lua_boost::lua_functor * NodeImpl::event( const std::string &_name )
{
	return Eventable::event( _name );
}
//////////////////////////////////////////////////////////////////////////
void NodeImpl::_debugRender()
{
	//Empty
}
//////////////////////////////////////////////////////////////////////////
void NodeImpl::_lostChildren(Node *_node, bool _valid)
{
	//Empty
}
//////////////////////////////////////////////////////////////////////////
void NodeImpl::_updateParent()
{
	//Empty
}
//////////////////////////////////////////////////////////////////////////
bool NodeImpl::_activate()
{
	//Empty
	return true;
}
//////////////////////////////////////////////////////////////////////////
void NodeImpl::_deactivate()
{
	//Empty
}
//////////////////////////////////////////////////////////////////////////
bool NodeImpl::_compile()
{
	//Empty
	return true;
}
//////////////////////////////////////////////////////////////////////////
void NodeImpl::_release()
{
	//Empty
}
//////////////////////////////////////////////////////////////////////////
void NodeImpl::_update(float _timing)
{
	//Empty
}
//////////////////////////////////////////////////////////////////////////
void NodeImpl::_visit( Visitor * _visitor )
{
	//Empty
}