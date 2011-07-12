#	include "Entity.h"

#	include "LogEngine.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	void Entity::setPrototype( const ConstString & _prototype )
	{
		m_prototype = _prototype;
	}
	//////////////////////////////////////////////////////////////////////////
	const ConstString & Entity::getPrototype() const
	{
		return m_prototype;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Entity::_activate()
	{
		this->callEvent( EVENT_PREPARATION, "()" );

		return Node::_activate();
	}
	//////////////////////////////////////////////////////////////////////////
	void Entity::_afterActivate()
	{
		Node::_afterActivate();

		this->callEvent( EVENT_ACTIVATE, "()" );
	}
	//////////////////////////////////////////////////////////////////////////
	void Entity::_afterDeactivate()
	{
		Node::_afterDeactivate();

		this->callEvent( EVENT_DEACTIVATE, "()" );
	}
	//////////////////////////////////////////////////////////////////////////
	bool Entity::_compile()
	{
		bool result = false;

		result = Node::_compile();

		if( result )
		{
			this->callEvent( EVENT_COMPILE, "()" );
		}

		return result;
	}
	//////////////////////////////////////////////////////////////////////////
	void Entity::_release()
	{
		Node::_release();

		this->callEvent( EVENT_RELEASE, "()" );
	}
	//////////////////////////////////////////////////////////////////////////
	void Entity::_embedding( PyObject * _embed )
	{
		Node::_embedding( _embed );

		Eventable::registerEvent( EVENT_LOADER, ("onLoader"), _embed );
		
		Eventable::registerEvent( EVENT_PREPARATION, ("onPreparation"), _embed );
		Eventable::registerEvent( EVENT_ACTIVATE, ("onActivate"), _embed );
		Eventable::registerEvent( EVENT_DEACTIVATE, ("onDeactivate"), _embed );
		Eventable::registerEvent( EVENT_COMPILE, ("onCompile"), _embed );
		Eventable::registerEvent( EVENT_RELEASE, ("onRelease"), _embed );
	}
	//////////////////////////////////////////////////////////////////////////
	void Entity::loader( BinParser * _parser )
	{
		Node::loader( _parser );
	}
	//////////////////////////////////////////////////////////////////////////
	void Entity::_loaded()
	{
		this->callEvent( EVENT_LOADER, "()" );
	}
}