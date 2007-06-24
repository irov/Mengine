#	include "NodeImpl.h"
#	include "ObjectImplement.h"

#	include "SceneManager.h"

#	include "Entity.h"

#	include "FileEngine.h"
#	include "ScriptEngine.h"

#	include "XmlParser.h"
#	include "ErrorMessage.h"

#	include <algorithm>

OBJECT_IMPLEMENT( NodeImpl )
//////////////////////////////////////////////////////////////////////////
NodeImpl::NodeImpl()
: m_parent(0)
, m_active(false)
, m_scriptable(0)
, m_iteratorChildren(m_listChildren.begin())
{
}
//////////////////////////////////////////////////////////////////////////
NodeImpl::~NodeImpl()
{
	delete m_scriptable;
}
//////////////////////////////////////////////////////////////////////////
void NodeImpl::destroy()
{
	for each( Node * children in m_listChildren )
	{
		if( children->isScriptable() )
		{
			Holder<ScriptEngine>::hostage()
				->decref( children );
		}

		children->destroy();
	}

	if( isScriptable() )
	{
		Holder<ScriptEngine>::hostage()
			->decref( this );
	}
	else
	{
		delete this;
	}
}
//////////////////////////////////////////////////////////////////////////
bool NodeImpl::activate()
{
	for each( Node * children in m_listChildren )
	{
		children->activate();
	}

	if( m_active )
	{
		return true;
	}

	m_active = _activate();

	return m_active;
}
//////////////////////////////////////////////////////////////////////////
void NodeImpl::deactivate()
{
	for each( Node * children in m_listChildren )
	{
		children->deactivate();
	}

	if( m_active )
	{
		_deactivate();
	}

	m_active = false;
}
//////////////////////////////////////////////////////////////////////////
bool NodeImpl::isActive()
{
	return m_active;
}
//////////////////////////////////////////////////////////////////////////
void NodeImpl::setName(const std::string & _name)
{
	m_name = _name;
}
//////////////////////////////////////////////////////////////////////////
const std::string & NodeImpl::getName()const
{	
	return m_name;
}
//////////////////////////////////////////////////////////////////////////
void NodeImpl::setType(const std::string & _type)
{
	m_type = _type;
}
//////////////////////////////////////////////////////////////////////////
const std::string & NodeImpl::getType()const
{
	return m_type;
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
void NodeImpl::setParent(Node *_node)
{
	if( m_parent )
	{
		m_parent->_lostChildren(this,true);
	}

	m_parent = _node;
	_updateParent();
}
//////////////////////////////////////////////////////////////////////////
void NodeImpl::update(float _timing)
{
	if( m_active == true )
	{
		_update(_timing);

		for each( Node * children in m_listChildren )
		{
			children->update( _timing );
		}
	}
}
//////////////////////////////////////////////////////////////////////////
void NodeImpl::loader(TiXmlElement * _xml)
{
	XML_FOR_EACH_TREE(_xml)
	{
		Eventable::loader( XML_CURRENT_NODE );

		XML_CHECK_NODE("Node")
		{
			XML_DEF_ATTRIBUTES_NODE(Name);
			XML_DEF_ATTRIBUTES_NODE(Type);

			Node *node = createChildren( Type );
	
			if(node == 0)
			{
				continue;
			}

			node->setName( Name );
			node->loader(XML_CURRENT_NODE);
		}

		XML_CHECK_NODE("External")
		{
			XML_DEF_ATTRIBUTES_NODE(File);
			
			//bool loading = false; 
			//XML_VALUE_ATTRIBUTE("Loading",loading);
			
			XML_PARSE_FILE_EX(File)
			{
				//if ( loading == true )
				//{
				//	loader(XML_CURRENT_NODE);
				//}
				//else
				{
					XML_CHECK_NODE("Node")
					{
						XML_DEF_ATTRIBUTES_NODE(Name);
						XML_DEF_ATTRIBUTES_NODE(Type);

						Node *node = createChildren( Type );
						
						if(node == 0)
						{
							continue;
						}

						node->setName( Name );
					
						node->loader( XML_CURRENT_NODE );
						//node->setResource(File);
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
Node * NodeImpl::createChildren( const std::string &_type )
{
	Node *node = SceneManager::createNode( _type );

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

	Holder<ScriptEngine>::hostage()
		->incref( _node );

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
	if( _name.empty() )
	{
		return false;
	}

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
	for each( Node * children in m_listChildren )
	{
		_visitor->apply( children );
	}
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

		Holder<ScriptEngine>::hostage()
			->decref( *it_find );

		m_listChildren.erase(it_find);
	}
}
//////////////////////////////////////////////////////////////////////////
void NodeImpl::debugRender()
{
	for each( Node * children in m_listChildren )
	{
		children->debugRender();
	}

	_debugRender();
}
//////////////////////////////////////////////////////////////////////////
void NodeImpl::registerEvent( const std::string &_name, ScriptObject * _func  )
{
	Eventable::registerEvent( _name, _func );
}
//////////////////////////////////////////////////////////////////////////
ScriptObject * NodeImpl::event( const std::string &_name )
{
	return Eventable::event( _name );
}
//////////////////////////////////////////////////////////////////////////
void NodeImpl::setScriptable( ScriptObject * _scriptable )
{
	m_scriptable = _scriptable;
}
//////////////////////////////////////////////////////////////////////////
ScriptObject * NodeImpl::getScriptable()
{
	return m_scriptable;
}
//////////////////////////////////////////////////////////////////////////
bool NodeImpl::isScriptable() const
{
	return m_scriptable != 0;
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
void NodeImpl::_update(float _timing)
{
	//Empty
}
//////////////////////////////////////////////////////////////////////////
void NodeImpl::_visit( Visitor * _visitor )
{
	//Empty
}