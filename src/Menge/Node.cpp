#	include "Node.h"
#	include "ScriptEngine.h"
#	include "NodeForeach.h"
#	include "XmlEngine.h"
#	include "SceneManager.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	Node::Node()
		: m_listener(0)
		, m_hide(false)
		, m_active(false)
		, m_enable(true)
		, m_updatable(true)
	{
	}
	//////////////////////////////////////////////////////////////////////////
	PyObject * Node::_embedded()
	{ 
		PyObject * embedding = Holder<ScriptEngine>::hostage()
			->wrap( this );

		return embedding;
	}
	//////////////////////////////////////////////////////////////////////////
	void Node::setListener( PyObject * _listener )
	{
		m_listener = _listener;
		this->_setListener();
	}
	//////////////////////////////////////////////////////////////////////////
	void Node::_setListener()
	{
		//Empty
	}
	//////////////////////////////////////////////////////////////////////////
	bool Node::compile()
	{
		struct ForeachCompile
			: public NodeForeach
		{
			ForeachCompile()
				: result(true)
			{
			}

			void apply( Node * children ) override
			{
				result = children->compile();
			}

			bool isCompile() const
			{
				return result;
			}

			bool result;
		};

		ForeachCompile functor;
		foreachChildren( functor );
		
		if( functor.isCompile() == false )
		{
			return false;
		}

		//MENGE_LOG( "Compiling Resource %s", this->getName().c_str() );
		bool result = Resource::compile();

		if( result == false )
		{
			//MENGE_LOG("Error: compiled Node '%s' is failed"
			//	, getName().c_str()
			//	);
		}

		return result;
	}
	//////////////////////////////////////////////////////////////////////////
	void Node::release()
	{
		struct ForeachRelease
			: public NodeForeach
		{
			void apply( Node * children ) override
			{
				children->release();
			}
		};

		foreachChildren( ForeachRelease() );

		deactivate();

		Resource::release();
	}
	//////////////////////////////////////////////////////////////////////////
	bool Node::recompile()
	{
		bool status = isActivate();

		release();
		compile();

		if( isCompile() == false )
		{
			return false;
		}

		if( status )
		{
			activate();
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void Node::hide( bool _value )
	{
		m_hide = _value;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Node::isHide() const
	{
		return m_hide;
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
	bool Node::activate()
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
				//MENGE_LOG("Error: activate Node '%s' is failed, becouse compiled is failed", m_name.c_str()	);
				return false;
			}
		}

		m_active = _activate();

		return m_active;
	}
	//////////////////////////////////////////////////////////////////////////
	void Node::deactivate()
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
	bool Node::isRenderable()
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
	bool Node::isActivate() const
	{
		return m_active;
	}
	//////////////////////////////////////////////////////////////////////////
	void Node::enable()
	{
		m_enable = true;
	}
	//////////////////////////////////////////////////////////////////////////
	void Node::disable()
	{
		m_enable = false;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Node::isEnable()
	{
		return m_enable;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Node::isUpdatable() const
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
	void Node::destroy()
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
	void Node::setUpdatable( bool _updatable )
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
	void Node::update( float _timing )
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
	void Node::loader( XmlElement * _xml )
	{
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
	void Node::_update( float _timing )
	{
		//Empty
	}
	//////////////////////////////////////////////////////////////////////////
	void Node::_destroy()
	{
		//Empty
	}
	//////////////////////////////////////////////////////////////////////////
	bool Node::updatable()
	{
		return m_updatable;
	}
}