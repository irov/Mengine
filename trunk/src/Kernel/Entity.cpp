#	include "Entity.h"

#   include "Interface/NodeInterface.h"

#	include "Logger/Logger.h"

namespace Menge
{
    //////////////////////////////////////////////////////////////////////////
    Entity::Entity()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    Entity::~Entity()
    {
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
		EVENTABLE_CALL(m_serviceProvider, this, EVENT_PREPARATION)("()");

		bool successful = Node::_activate();

		return successful;
	}
	//////////////////////////////////////////////////////////////////////////
	void Entity::_afterActivate()
	{
		Node::_afterActivate();

		EVENTABLE_CALL(m_serviceProvider, this, EVENT_ACTIVATE)("()");
	}
    //////////////////////////////////////////////////////////////////////////
    void Entity::_deactivate()
    {
        Node::_deactivate();

        EVENTABLE_CALL(m_serviceProvider, this, EVENT_PREPARATION_DEACTIVATE)("()");
    }
	//////////////////////////////////////////////////////////////////////////
	void Entity::_afterDeactivate()
	{
		Node::_afterDeactivate();

		EVENTABLE_CALL(m_serviceProvider, this, EVENT_DEACTIVATE)("()");
	}
	//////////////////////////////////////////////////////////////////////////
	bool Entity::_compile()
	{
		EVENTABLE_CALL(m_serviceProvider, this, EVENT_COMPILE)("()");
		
		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void Entity::_release()
	{
		EVENTABLE_CALL(m_serviceProvider, this, EVENT_RELEASE)("()");
	}
	//////////////////////////////////////////////////////////////////////////
	void Entity::_embedding( PyObject * _embed )
	{
		Node::_embedding( _embed );

		this->registerEventMethod( EVENT_CREATE, "onCreate", _embed );
		this->registerEventMethod( EVENT_DESTROY, "onDestroy", _embed );
		
		this->registerEventMethod( EVENT_PREPARATION, "onPreparation", _embed );
		this->registerEventMethod( EVENT_ACTIVATE, "onActivate", _embed );
        this->registerEventMethod( EVENT_PREPARATION_DEACTIVATE, "onPreparationDeactivate", _embed );
		this->registerEventMethod( EVENT_DEACTIVATE, "onDeactivate", _embed );
		this->registerEventMethod( EVENT_COMPILE, "onCompile", _embed );
		this->registerEventMethod( EVENT_RELEASE, "onRelease", _embed );
	}
	//////////////////////////////////////////////////////////////////////////
	void Entity::onCreate()
	{
		EVENTABLE_CALL(m_serviceProvider, this, EVENT_CREATE)("()");
	}
	//////////////////////////////////////////////////////////////////////////
	void Entity::destroy()
	{
        if( this->isShallowGrave() == true )
        {
            NODE_SERVICE(m_serviceProvider)
                ->addHomeless( this );

            return;
        }

        EVENTABLE_CALL(m_serviceProvider, this, EVENT_DESTROY)("()");

        Factorable::destroy();
	}
}