#	include "Entity.h"

#   include "Interface/NodeInterface.h"

#	include "Logger/Logger.h"

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

		this->registerEvent( EVENT_CREATE, ("onCreate"), _embed );
		this->registerEvent( EVENT_DESTROY, ("onDestroy"), _embed );
		
		this->registerEvent( EVENT_PREPARATION, ("onPreparation"), _embed );
		this->registerEvent( EVENT_ACTIVATE, ("onActivate"), _embed );
		this->registerEvent( EVENT_DEACTIVATE, ("onDeactivate"), _embed );
		this->registerEvent( EVENT_COMPILE, ("onCompile"), _embed );
		this->registerEvent( EVENT_RELEASE, ("onRelease"), _embed );		
	}
	//////////////////////////////////////////////////////////////////////////
	void Entity::onCreate()
	{
		//this->callEvent( EVENT_CREATE, "()" );
		EVENTABLE_CALL(this, EVENT_CREATE)("()");
	}
	//////////////////////////////////////////////////////////////////////////
	bool Entity::_destroy()
	{
        if( this->isShallowGrave() == true )
        {
            NODE_SERVICE(m_serviceProvider)
                ->addHomeless( this );

            return false;
        }

        EVENTABLE_CALL(this, EVENT_DESTROY)("()");

        bool result = Node::_destroy();

		return result;
	}
}