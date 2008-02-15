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
		NodeEventable::loader( _xml );

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

			//XML_CHECK_NODE("External")
			//{
			//	std::string file;

			//	XML_FOR_EACH_ATTRIBUTES()
			//	{
			//		XML_CHECK_ATTRIBUTE("File", file );
			//	}
			//	
			//	TiXmlDocument * document = Holder<FileEngine>::hostage()
			//		->loadXml( file );

			//	XML_FOR_EACH_DOCUMENT( document )
			//	{
			//		XML_CHECK_NODE("Node")
			//		{
			//			XML_DEF_ATTRIBUTES_NODE(Name);
			//			XML_DEF_ATTRIBUTES_NODE(Type);

			//			Node *node = SceneManager::createNode( Type );

			//			if(node == 0)
			//			{
			//				continue;
			//			}

			//			addChildren( node );

			//			node->setName( Name );
			//			node->loader( XML_CURRENT_NODE );
			//		}				
			//	}
			//	XML_INVALID_PARSE()
			//	{
			//		MENGE_LOG("Invalid parse external node %s for %s\n"
			//			, File.c_str()
			//			, m_name.c_str()
			//			);
			//	}
			//}
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