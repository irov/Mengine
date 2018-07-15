#include "Entity.h"

#include "Interface/NodeInterface.h"

#include "Kernel/Logger.h"
#include "Kernel/BaseEvent.h"

namespace Mengine
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
	void Entity::setScriptEventable( const EventablePtr & _eventable )
	{
		m_scriptEventable = _eventable;
	}
	//////////////////////////////////////////////////////////////////////////
    const EventablePtr & Entity::getScriptEventable() const
	{
		return m_scriptEventable;
	}
    //////////////////////////////////////////////////////////////////////////
    EventInterfacePtr Entity::getScriptEvent() const
    {
        if( m_scriptEventable == nullptr )
        {
            return nullptr;
        }

        EventInterfacePtr event = m_scriptEventable->getEvent();

        return event;
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
        EventInterfacePtr event = this->getScriptEvent();

        if( event != nullptr )
        {
            EVENTABLE_METHODT( event, EVENT_ENTITY_PREPARATION, EntityEventReceiver )
                ->onEntityPreparation( m_object );
        }

		bool successful = Node::_activate();

		return successful;
	}
	//////////////////////////////////////////////////////////////////////////
	void Entity::_afterActivate()
	{
		Node::_afterActivate();

        EventInterfacePtr event = this->getScriptEvent();

        if( event != nullptr )
        {
            EVENTABLE_METHODT( event, EVENT_ENTITY_ACTIVATE, EntityEventReceiver )
                ->onEntityActivate( m_object );
        }
	}
    //////////////////////////////////////////////////////////////////////////
    void Entity::_deactivate()
    {
        EventInterfacePtr event = this->getScriptEvent();

        if( event != nullptr )
        {
            EVENTABLE_METHODT( event, EVENT_ENTITY_PREPARATION_DEACTIVATE, EntityEventReceiver )
                ->onEntityPreparationDeactivate( m_object );
        }

        Node::_deactivate();		
    }
	//////////////////////////////////////////////////////////////////////////
	void Entity::_afterDeactivate()
	{
		Node::_afterDeactivate();

        EventInterfacePtr event = this->getScriptEvent();

        if( event != nullptr )
        {
            EVENTABLE_METHODT( event, EVENT_ENTITY_DEACTIVATE, EntityEventReceiver )
                ->onEntityDeactivate( m_object );
        }
	}
	//////////////////////////////////////////////////////////////////////////
	bool Entity::_compile()
	{
        EventInterfacePtr event = this->getScriptEvent();

        if( event != nullptr )
        {
            EVENTABLE_METHODT( event, EVENT_ENTITY_COMPILE, EntityEventReceiver )
                ->onEntityCompile( m_object );
        }
		
		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void Entity::_release()
	{
        EventInterfacePtr event = this->getScriptEvent();

        if( event != nullptr )
        {
            EVENTABLE_METHODT( event, EVENT_ENTITY_RELEASE, EntityEventReceiver )
                ->onEntityRelease( m_object );
        }
	}
    //////////////////////////////////////////////////////////////////////////
    void Entity::_update( float _current, float _timing )
    {
        Node::_update( _current, _timing );

        EventInterfacePtr event = this->getScriptEvent();

        if( event != nullptr )
        {
            EVENTABLE_METHODT( event, EVENT_ENTITY_UPDATE, EntityEventReceiver )
                ->onEntityUpdate( m_object, _current, _timing );
        }
    }
	//////////////////////////////////////////////////////////////////////////
	void Entity::onCreate()
	{
        EventInterfacePtr event = this->getScriptEvent();

        if( event != nullptr )
        {
            EVENTABLE_METHODT( event, EVENT_ENTITY_CREATE, EntityEventReceiver )
                ->onEntityCreate( m_object, this );
        }
	}
	//////////////////////////////////////////////////////////////////////////
	void Entity::_destroy()
	{
		this->release();

		Node * old_parent = this->getParent();

        EventInterfacePtr event = this->getScriptEvent();

        if( event != nullptr )
        {
            EVENTABLE_METHODT( event, EVENT_ENTITY_DESTROY, EntityEventReceiver )
                ->onEntityDestroy( m_object );
        }
		
		m_object.reset();

		Node * new_parent = this->getParent();

		if( old_parent != new_parent )
		{
			LOGGER_ERROR("Entity::destroy %s:%s script event EVENT_DESTROY replace node to other hierarchy"
				, this->getType().c_str()
				, this->getName().c_str()
				);

			return;
		}

		this->destroyAllChild();
		this->removeFromParent();

		this->unwrap();
	}
}