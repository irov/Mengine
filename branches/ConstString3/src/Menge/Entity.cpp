#	include "Entity.h"

#	include "LogEngine.h"

namespace Menge
{
    size_t Entity::s_enum = 0;
    //////////////////////////////////////////////////////////////////////////
    Entity::Entity()
    {
        Entity::s_enum++;
    }
    //////////////////////////////////////////////////////////////////////////
    Entity::~Entity()
    {
        Entity::s_enum--;
    }
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
		//this->callEvent( EVENT_PREPARATION, "()" );
		EVENTABLE_CALL(this, EVENT_PREPARATION)("()");

		return Node::_activate();
	}
	//////////////////////////////////////////////////////////////////////////
	void Entity::_afterActivate()
	{
		Node::_afterActivate();

		//this->callEvent( EVENT_ACTIVATE, "()" );
		EVENTABLE_CALL(this, EVENT_ACTIVATE)("()");
	}
	//////////////////////////////////////////////////////////////////////////
	void Entity::_afterDeactivate()
	{
		Node::_afterDeactivate();

		//this->callEvent( EVENT_DEACTIVATE, "()" );
		EVENTABLE_CALL(this, EVENT_DEACTIVATE)("()");
	}
	//////////////////////////////////////////////////////////////////////////
	bool Entity::_compile()
	{
		if( Node::_compile() == false )
		{
			return false;
		}

		//this->callEvent( EVENT_COMPILE, "()" );
		EVENTABLE_CALL(this, EVENT_COMPILE)("()");
		
		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void Entity::_release()
	{
		Node::_release();

		//this->callEvent( EVENT_RELEASE, "()" );
		EVENTABLE_CALL(this, EVENT_RELEASE)("()");
	}
	//////////////////////////////////////////////////////////////////////////
	void Entity::_embedding( PyObject * _embed )
	{
		Node::_embedding( _embed );

		Eventable::registerEvent( EVENT_CREATE, ("onCreate"), _embed );
		Eventable::registerEvent( EVENT_DESTROY, ("onDestroy"), _embed );
		
		Eventable::registerEvent( EVENT_PREPARATION, ("onPreparation"), _embed );
		Eventable::registerEvent( EVENT_ACTIVATE, ("onActivate"), _embed );
		Eventable::registerEvent( EVENT_DEACTIVATE, ("onDeactivate"), _embed );
		Eventable::registerEvent( EVENT_COMPILE, ("onCompile"), _embed );
		Eventable::registerEvent( EVENT_RELEASE, ("onRelease"), _embed );		
	}
	//////////////////////////////////////////////////////////////////////////
	void Entity::_create()
	{
		Node::_create();

		//this->callEvent( EVENT_CREATE, "()" );
		EVENTABLE_CALL(this, EVENT_CREATE)("()");
	}
	//////////////////////////////////////////////////////////////////////////
	bool Entity::_destroy()
	{
		//this->callEvent( EVENT_DESTROY, "()" );
		EVENTABLE_CALL(this, EVENT_DESTROY)("()");

		bool result = Node::_destroy();

		return result;
	}
}