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
		//: m_active(false)
		//, m_enable(true)
		//: m_updatable(true)
	{

	}
	//////////////////////////////////////////////////////////////////////////
	NodeCore::~NodeCore()
	{
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
		}
	}
	//////////////////////////////////////////////////////////////////////////
	Scriptable * NodeCore::getScriptable()
	{
		return this;
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
	bool NodeCore::updatable()
	{
		return m_updatable;
	}
}