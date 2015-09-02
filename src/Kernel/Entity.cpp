#	include "Entity.h"

#   include "Interface/NodeInterface.h"

#	include "Logger/Logger.h"

namespace Menge
{
    //////////////////////////////////////////////////////////////////////////
    Entity::Entity()
		: m_scriptEventable(nullptr)
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
	void Entity::setScriptEventable( Eventable * _eventable )
	{
		m_scriptEventable = _eventable;
	}
	//////////////////////////////////////////////////////////////////////////
	Eventable * Entity::getScriptEventable() const
	{
		return m_scriptEventable;
	}
	//////////////////////////////////////////////////////////////////////////
	void Entity::setScriptObject( const pybind::object & _object )
	{
		m_object = _object;
	}
	//////////////////////////////////////////////////////////////////////////
	const pybind::object & Entity::getScriptObject() const
	{
		return m_object;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Entity::_activate()
	{
		EVENTABLE_CALL( m_serviceProvider, m_scriptEventable, EVENT_PREPARATION )(m_object);

		bool successful = Node::_activate();

		return successful;
	}
	//////////////////////////////////////////////////////////////////////////
	void Entity::_afterActivate()
	{
		Node::_afterActivate();

		EVENTABLE_CALL( m_serviceProvider, m_scriptEventable, EVENT_ACTIVATE )(m_object);
	}
    //////////////////////////////////////////////////////////////////////////
    void Entity::_deactivate()
    {
        Node::_deactivate();

		EVENTABLE_CALL( m_serviceProvider, m_scriptEventable, EVENT_PREPARATION_DEACTIVATE )(m_object);
    }
	//////////////////////////////////////////////////////////////////////////
	void Entity::_afterDeactivate()
	{
		Node::_afterDeactivate();

		EVENTABLE_CALL( m_serviceProvider, m_scriptEventable, EVENT_DEACTIVATE )(m_object);
	}
	//////////////////////////////////////////////////////////////////////////
	bool Entity::_compile()
	{
		EVENTABLE_CALL( m_serviceProvider, m_scriptEventable, EVENT_COMPILE )(m_object);
		
		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void Entity::_release()
	{
		EVENTABLE_CALL( m_serviceProvider, m_scriptEventable, EVENT_RELEASE )(m_object);
	}
	//////////////////////////////////////////////////////////////////////////
	void Entity::onCreate()
	{
		EVENTABLE_CALL( m_serviceProvider, m_scriptEventable, EVENT_CREATE )(m_object, this);
	}
	//////////////////////////////////////////////////////////////////////////
	void Entity::destroy()
	{
        if( this->isShallowGrave() == true )
        {
            NODE_SERVICE(m_serviceProvider)
                ->addHomeless( this );

			this->release();

			this->setShallowGravePropagate( true );
			
            return;
        }

        Factorable::destroy();
	}
	//////////////////////////////////////////////////////////////////////////
	void Entity::_destroy()
	{
		this->release();

		Node * old_parent = this->getParent();

		EVENTABLE_CALL( m_serviceProvider, m_scriptEventable, EVENT_DESTROY )(m_object);

		m_object.reset();

		Node * new_parent = this->getParent();

		if( old_parent != new_parent )
		{
			LOGGER_ERROR( m_serviceProvider )("Entity::destroy %s:%s script event EVENT_DESTROY replace node to other hierarchy"
				, this->getType().c_str()
				, this->getName().c_str()
				);

			return;
		}

		this->destroyAllChild();
		this->removeFromParent();

		this->unwrap();
	}
	//////////////////////////////////////////////////////////////////////////
	void Entity::_unshallowGrave()
	{
		this->setShallowGravePropagate( false );

		this->removeFromParent();

		EVENTABLE_CALL( m_serviceProvider, m_scriptEventable, EVENT_DESTROY )(m_object);

		Factorable::destroy();
	}
}