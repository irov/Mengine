#include "Entity.h"

#include "Interface/UpdateInterface.h"

#include "Kernel/Logger.h"
#include "Kernel/BaseEventation.h"

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
    EventationInterfacePtr Entity::getScriptEventation() const
    {
        if( m_scriptEventable == nullptr )
        {
            return nullptr;
        }

        EventationInterfacePtr event = m_scriptEventable->getEventation();

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
        EventationInterfacePtr event = this->getScriptEventation();

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

        EventationInterfacePtr event = this->getScriptEventation();

        if( event != nullptr )
        {
            EVENTABLE_METHODT( event, EVENT_ENTITY_ACTIVATE, EntityEventReceiver )
                ->onEntityActivate( m_object );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void Entity::_deactivate()
    {
        EventationInterfacePtr event = this->getScriptEventation();

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

        EventationInterfacePtr event = this->getScriptEventation();

        if( event != nullptr )
        {
            EVENTABLE_METHODT( event, EVENT_ENTITY_DEACTIVATE, EntityEventReceiver )
                ->onEntityDeactivate( m_object );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    bool Entity::_compile()
    {
        EventationInterfacePtr event = this->getScriptEventation();

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
        EventationInterfacePtr event = this->getScriptEventation();

        if( event != nullptr )
        {
            EVENTABLE_METHODT( event, EVENT_ENTITY_RELEASE, EntityEventReceiver )
                ->onEntityRelease( m_object );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void Entity::onCreate()
    {
        EventationInterfacePtr event = this->getScriptEventation();

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

        EventationInterfacePtr event = this->getScriptEventation();

        if( event != nullptr )
        {
            EVENTABLE_METHODT( event, EVENT_ENTITY_DESTROY, EntityEventReceiver )
                ->onEntityDestroy( m_object );
        }

        m_object.reset();

        Node * new_parent = this->getParent();

        if( old_parent != new_parent )
        {
            LOGGER_ERROR( "Entity::destroy %s:%s script event EVENT_DESTROY replace node to other hierarchy"
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