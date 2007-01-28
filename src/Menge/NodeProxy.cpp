#	include "NodeProxy.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	NodeProxy::NodeProxy( Node *_node)
	: m_node(_node)
	{
	}
	//////////////////////////////////////////////////////////////////////////
	NodeProxy::~NodeProxy()
	{
		
	}
	//////////////////////////////////////////////////////////////////////////
	bool NodeProxy::activate()
	{
		return m_node->activate();
	};
	//////////////////////////////////////////////////////////////////////////
	void NodeProxy::deactivate()
	{
		return m_node->deactivate();
	};
	//////////////////////////////////////////////////////////////////////////
	bool NodeProxy::isActive()
	{
		return m_node->isActive();
	}
	//////////////////////////////////////////////////////////////////////////
	bool NodeProxy::compile()
	{
		return m_node->compile();
	}
	//////////////////////////////////////////////////////////////////////////
	void NodeProxy::release()
	{
		return m_node->release();
	}
	//////////////////////////////////////////////////////////////////////////
	bool NodeProxy::isCompile()
	{
		return m_node->isCompile();
	}
	//////////////////////////////////////////////////////////////////////////
	void NodeProxy::setName(const std::string &_name)
	{
		return m_node->setName(_name);
	}
	//////////////////////////////////////////////////////////////////////////
	const std::string & NodeProxy::getName()const
	{
		return m_node->getName();
	}
	//////////////////////////////////////////////////////////////////////////
	void NodeProxy::setType(const std::string &type)
	{
		return m_node->setType(type);
	}
	//////////////////////////////////////////////////////////////////////////
	const std::string & NodeProxy::getType()const
	{
		return m_node->getType();
	}
	//////////////////////////////////////////////////////////////////////////
	size_t NodeProxy::getTypeId()const
	{
		return m_node->getTypeId();
	}
	//////////////////////////////////////////////////////////////////////////
	Node * NodeProxy::getParent()
	{
		return m_node->getParent();
	};
	//////////////////////////////////////////////////////////////////////////
	bool NodeProxy::isRoot()
	{
		return m_node->isRoot();
	};
	//////////////////////////////////////////////////////////////////////////
	bool NodeProxy::addChildren(Node *_node)
	{
		return m_node->addChildren(_node);
	}
	//////////////////////////////////////////////////////////////////////////
	bool NodeProxy::isChildren(Node *_node)
	{
		return m_node->isChildren(_node);
	}
	//////////////////////////////////////////////////////////////////////////
	void NodeProxy::visitChildren( Visitor *_visitor)
	{
		return m_node->visitChildren( _visitor );
	}
	//////////////////////////////////////////////////////////////////////////
	void NodeProxy::removeChildren(Node *_node)
	{
		return m_node->removeChildren(_node);
	}
	//////////////////////////////////////////////////////////////////////////
	Node * NodeProxy::nextChildren()
	{
		return m_node->nextChildren();
	}
	//////////////////////////////////////////////////////////////////////////
	Node * NodeProxy::beginChildren()
	{
		return m_node->beginChildren();
	}
	//////////////////////////////////////////////////////////////////////////
	void NodeProxy::update(float _timing)
	{
		return m_node->update(_timing);
	};
	//////////////////////////////////////////////////////////////////////////
	void NodeProxy::loader(TiXmlElement *xml)
	{
		return m_node->loader(xml);
	}
	//////////////////////////////////////////////////////////////////////////
	void NodeProxy::visit( Visitor * _visitor)
	{
		return m_node->visit(_visitor);
	}
	//////////////////////////////////////////////////////////////////////////
	void NodeProxy::setParent(Node *_node)
	{
		return m_node->setParent(_node);
	}
	//////////////////////////////////////////////////////////////////////////
	bool NodeProxy::isExternal()const
	{
		return m_node->isExternal();
	}
	//////////////////////////////////////////////////////////////////////////
	void NodeProxy::setResource(const std::string &_file)
	{
		return m_node->setResource(_file);
	};
	//////////////////////////////////////////////////////////////////////////
	const std::string & NodeProxy::getResource()const
	{
		return m_node->getResource();
	}
	//////////////////////////////////////////////////////////////////////////
	void NodeProxy::registerEvent( const std::string &_name, const lua_boost::lua_functor * _func  )
	{
		return m_node->registerEvent(_name,_func);
	}
	//////////////////////////////////////////////////////////////////////////
	const lua_boost::lua_functor * NodeProxy::event( const std::string &_name )
	{
		return m_node->event(_name);
	}
	//////////////////////////////////////////////////////////////////////////
	void NodeProxy::debugRender()
	{
		return m_node->debugRender();
	}
	//////////////////////////////////////////////////////////////////////////
	void NodeProxy::_lostChildren(Node *_node, bool _valid)
	{
		return m_node->_lostChildren(_node,_valid);
	}

};