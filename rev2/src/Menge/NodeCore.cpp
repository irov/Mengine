#	include "NodeCore.h"

#	include "ObjectImplement.h"

#	include "NodeForeach.h"

#	include "SceneManager.h"

#	include "FileEngine.h"

#	include "ScriptEngine.h"

#	include "LogEngine.h"

#	include "XmlEngine.h"

#	include <algorithm>

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	NodeCore::NodeCore()
		: m_active(false)
		, m_enable(true)
		, m_updatable(true)
		, m_parent(0)
		, m_hide( false )
	{
		this->setWrapp( this );
	}
	//////////////////////////////////////////////////////////////////////////
	NodeCore::~NodeCore()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	bool NodeCore::registerEventMethod( const std::string & _name, const std::string & _method  )
	{
		Scriptable * scriptable = getScriptable();
		PyObject * module = scriptable->getScript();

		return Eventable::registerEventListener( _name, _method, module );
	}
	//////////////////////////////////////////////////////////////////////////
	void NodeCore::hide( bool _value )
	{
		m_hide = _value;
	}
	//////////////////////////////////////////////////////////////////////////
	bool NodeCore::isHide() const
	{
		return m_hide;
	}
	//////////////////////////////////////////////////////////////////////////
	bool NodeCore::isRenderable()
	{
		if( isEnable() == false )
		{
			return false;
		}

		if( isActivate() == false )
		{
			return false;
		}

		if( m_hide == true )
		{
			return false;
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void NodeCore::render()
	{
		if( isRenderable() == false )
		{
			return;
		}

		if( _renderBegin() )
		{
			_render();

			struct ForeachRender
				: public NodeForeach
			{
				void apply( Node * children ) override
				{
					children->render();
				}
			};

			foreachChildren( ForeachRender() );

			_renderEnd();
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void NodeCore::_render()
	{
		//Empty
	}
	//////////////////////////////////////////////////////////////////////////
	bool NodeCore::_renderBegin()
	{
		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void NodeCore::_renderEnd()
	{
		//Empty
	}
	//////////////////////////////////////////////////////////////////////////
	void NodeCore::_addChildren( Node * _node )
	{
		//	Empty
	}
	//////////////////////////////////////////////////////////////////////////
	void NodeCore::setParent( Node * _parent )
	{
		m_parent = dynamic_cast<Node *>(_parent);
	}
	//////////////////////////////////////////////////////////////////////////
	Node * NodeCore::getParent()
	{
		return m_parent;
	}
	//////////////////////////////////////////////////////////////////////////
	bool NodeCore::addChildren( Node * _node )
	{
		if( isChildren( _node ) )
		{
			return false;
		}

		Node * t_node = dynamic_cast<Node *>( _node );

		t_node->setParent( this );

		m_listChildren.push_back( t_node );

		_addChildren( t_node );

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void NodeCore::removeChildren( Node *_node)
	{
		TListChildren::iterator it_find = 
			std::find( m_listChildren.begin(), m_listChildren.end(), _node );

		if( it_find != m_listChildren.end() )
		{
			m_listChildren.erase( it_find );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	bool NodeCore::isChildren( Node *_node)
	{
		TListChildren::iterator it_find = 
			std::find( m_listChildren.begin(), m_listChildren.end(), _node );

		return it_find != m_listChildren.end();
	}
	//////////////////////////////////////////////////////////////////////////
	void NodeCore::foreachChildren( NodeForeach & _foreach )
	{
		for( TListChildren::iterator
			it = m_listChildren.begin(),
			it_end = m_listChildren.end();
		it != it_end;
		++it)
		{
			_foreach.apply( *it );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	Node * NodeCore::getChildren( const std::string & _name, bool _recursion )
	{
		for( TListChildren::iterator
			it = m_listChildren.begin(),
			it_end = m_listChildren.end();
		it != it_end;
		++it)
		{
			Node * children = *it;
			if( children->getName() == _name )
			{
				return children;
			}

			if( _recursion )
			{
				if( Node * result = children->getChildren( _name, _recursion ) )
				{
					return result;
				}
			}
		}

		return 0;
	}
	//////////////////////////////////////////////////////////////////////////
	void NodeCore::destroy()
	{
		struct ForeachDestroy
			: public NodeForeach
		{
			void apply( Node * children ) override
			{
				children->destroy();
			}
		};

		foreachChildren( ForeachDestroy() );

		_destroy();

		delete this;
	}
	//////////////////////////////////////////////////////////////////////////
	bool NodeCore::activate()
	{
		struct ForeachActivate
			: public NodeForeach
		{
			void apply( Node * children ) override
			{
				children->activate();
			}
		};

		foreachChildren( ForeachActivate() );

		if( m_active )
		{
			return true;
		}

		if( isCompile() == false )
		{
			if( compile() == false )
			{
				MENGE_LOG("Error: activate Node '%s' is failed, becouse compiled is failed\n"
					, m_name.c_str() 
					);

				return false;
			}
		}

		m_active = _activate();

		return m_active;
	}
	//////////////////////////////////////////////////////////////////////////
	void NodeCore::deactivate()
	{
		struct ForeachDeactivate
			: public NodeForeach
		{
			void apply( Node * children ) override
			{
				children->deactivate();
			}
		};

		foreachChildren( ForeachDeactivate() );

		if( m_active )
		{
			m_active = false;

			_deactivate();
		}		
	}
	//////////////////////////////////////////////////////////////////////////
	bool NodeCore::isActivate()
	{
		return m_active;
	}
	//////////////////////////////////////////////////////////////////////////
	void NodeCore::enable()
	{
		m_enable = true;
	}
	//////////////////////////////////////////////////////////////////////////
	void NodeCore::disable()
	{
		m_enable = false;
	}
	//////////////////////////////////////////////////////////////////////////
	bool NodeCore::isEnable()
	{
		return m_enable;
	}
	//////////////////////////////////////////////////////////////////////////
	void NodeCore::setName(const std::string & _name)
	{
		m_name = _name;
	}
	//////////////////////////////////////////////////////////////////////////
	const std::string & NodeCore::getName()const
	{	
		return m_name;
	}
	//////////////////////////////////////////////////////////////////////////
	void NodeCore::setType( const std::string & _type)
	{
		m_type = _type;
	}
	//////////////////////////////////////////////////////////////////////////
	const std::string & NodeCore::getType()const
	{
		return m_type;
	}
	//////////////////////////////////////////////////////////////////////////
	void NodeCore::setUpdatable( bool _updatable )
	{
		m_updatable = _updatable;

		//// !!!! Temporary hack
		struct ForeachSetUpdatable
			: public NodeForeach
		{
			ForeachSetUpdatable( bool _updatable )
				: m_updatable(_updatable)
			{

			}

			void apply( Node * children ) override
			{
				children->setUpdatable( m_updatable );
			}

			bool m_updatable;
		};

		foreachChildren( ForeachSetUpdatable( _updatable ) );
		//// !!!!
	}
	//////////////////////////////////////////////////////////////////////////
	bool NodeCore::isUpdatable()
	{
		/*if( m_updatable == false )
		{
			return false;
		}*/

		if( m_enable == false )
		{
			return false;
		}

		if( m_active == false )
		{
			return false;
		}
		
		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void NodeCore::update( float _timing )
	{
		if( isUpdatable() == false )
		{
			return;
		}
		
		if( m_updatable )	// !!!!
		{
			_update( _timing );
		}

		struct ForeachUpdate
			: public NodeForeach
		{
			ForeachUpdate( float _timing )
				: m_timing(_timing)
			{

			}

			void apply( Node * children ) override
			{
				children->update( m_timing );
			}

			float m_timing;
		};

		foreachChildren( ForeachUpdate( _timing ) );
	}
	//////////////////////////////////////////////////////////////////////////
	void NodeCore::loader( XmlElement * _xml )
	{
		//NodeEventable::loader( _xml );

		XML_SWITCH_NODE(_xml)
		{
			XML_CASE_ATTRIBUTE_NODE("Enable", "Value", m_enable );

			XML_CASE_NODE("Node")
			{
				std::string name;
				std::string type;

				XML_FOR_EACH_ATTRIBUTES()
				{
					XML_CASE_ATTRIBUTE( "Name", name );
					XML_CASE_ATTRIBUTE( "Type", type );
				}

				Node *node = SceneManager::createNode( type );

				if(node == 0)
				{
					continue;
				}

				node->setName( name );
				addChildren( node );

				XML_PARSE_ELEMENT( node, &Node::loader );
			}
			//
			XML_CASE_NODE("Hide")
			{
				XML_FOR_EACH_ATTRIBUTES()
				{
					XML_CASE_ATTRIBUTE("Value", m_hide);
				}
			}
		}
	}
	//////////////////////////////////////////////////////////////////////////
	Scriptable * NodeCore::getScriptable()
	{
		return this;
	}
	//////////////////////////////////////////////////////////////////////////
	void NodeCore::debugRender()
	{
		struct ForeachDebugRender
			: public NodeForeach
		{
			void apply( Node * children ) override
			{
				children->debugRender();
			}
		};

		foreachChildren( ForeachDebugRender() );

		_debugRender();
	}
	//////////////////////////////////////////////////////////////////////////
	void NodeCore::_debugRender()
	{
		//Empty
	}
	//////////////////////////////////////////////////////////////////////////
	bool NodeCore::_activate()
	{
		//Empty
		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void NodeCore::_deactivate()
	{
		//Empty
	}
	//////////////////////////////////////////////////////////////////////////
	void NodeCore::_update( float _timing )
	{
		//Empty
	}
	//////////////////////////////////////////////////////////////////////////
	void NodeCore::_destroy()
	{
		//Empty
	}
	//////////////////////////////////////////////////////////////////////////
}