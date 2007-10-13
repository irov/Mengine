#	include "NodeCore.h"

#	include "ObjectImplement.h"

#	include "NodeForeach.h"

#	include "SceneManager.h"

#	include "FileEngine.h"

#	include "ScriptEngine.h"

#	include "ErrorMessage.h"

#	include "XmlParser/XmlParser.h"

#	include <algorithm>

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	NodeCore::NodeCore()
		: m_active(false)
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
			void apply( Node * children ) const override
			{
				children->destroy();
			}
		};

		foreachChildren( ForeachDestroy() );

		delete this;
	}
	//////////////////////////////////////////////////////////////////////////
	bool NodeCore::activate()
	{
		struct ForeachActivate
			: public NodeForeach
		{
			void apply( Node * children ) const override
			{
				children->activate();
			}
		};

		foreachChildren( ForeachActivate() );

		if( m_active )
		{
			return true;
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
			void apply( Node * children ) const override
			{
				children->deactivate();
			}
		};

		foreachChildren( ForeachDeactivate() );

		if( m_active )
		{
			_deactivate();
		}

		m_active = false;
	}
	//////////////////////////////////////////////////////////////////////////
	bool NodeCore::isActive()
	{
		return m_active;
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
	void NodeCore::update( size_t _timing, const Viewport & _viewport )
	{
		if( m_active == false )
		{
			return;
		}
		
		_update( _timing, _viewport );

		struct ForeachUpdate
			: public NodeForeach
		{
			ForeachUpdate( size_t _timing, const Viewport & _viewport )
				: m_timing(_timing)
				, m_viewport(_viewport)
			{

			}

			void apply( Node * children ) const override
			{
				children->update( m_timing, m_viewport );
			}

			size_t m_timing;
			const Viewport & m_viewport;
		};

		foreachChildren( ForeachUpdate( _timing, _viewport ) );
	}
	//////////////////////////////////////////////////////////////////////////
	void NodeCore::loader(TiXmlElement * _xml)
	{
		Eventable::loader( _xml );

		XML_FOR_EACH_TREE(_xml)
		{
			XML_CHECK_NODE("Node")
			{
				XML_DEF_ATTRIBUTES_NODE(Name);
				XML_DEF_ATTRIBUTES_NODE(Type);

				Node *node = SceneManager::createNode( Type );

				if(node == 0)
				{
					continue;
				}

				addChildren( node );

				node->setName( Name );
				node->loader(XML_CURRENT_NODE);
			}

			XML_CHECK_NODE("External")
			{
				XML_DEF_ATTRIBUTES_NODE(File);
				
				TiXmlDocument * document = Holder<FileEngine>::hostage()
					->loadXml( File );

				XML_FOR_EACH_DOCUMENT( document )
				{
					XML_CHECK_NODE("Node")
					{
						XML_DEF_ATTRIBUTES_NODE(Name);
						XML_DEF_ATTRIBUTES_NODE(Type);

						Node *node = SceneManager::createNode( Type );

						if(node == 0)
						{
							continue;
						}

						addChildren( node );

						node->setName( Name );

						node->loader( XML_CURRENT_NODE );
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
			void apply( Node * children ) const override
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
	void NodeCore::_update( size_t _timing, const Viewport & _viewport )
	{
		//Empty
	}
}