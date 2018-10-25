#include "Entity.h"

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
    void Entity::setBehaviorEventable( const EventablePtr & _behaviorEventable )
    {
        m_behaviorEventable = _behaviorEventable;
    }
    //////////////////////////////////////////////////////////////////////////
    const EventablePtr & Entity::getBehaviorEventable() const
    {
        return m_behaviorEventable;
    }
    //////////////////////////////////////////////////////////////////////////
    void Entity::setBehavior( const EntityBehaviorInterfacePtr & _behavior )
    {
        m_behavior = _behavior;
    }
    //////////////////////////////////////////////////////////////////////////
    const EntityBehaviorInterfacePtr & Entity::getBehavior() const
    {
        return m_behavior;
    }
    //////////////////////////////////////////////////////////////////////////
    EventationInterface * Entity::getEventation()
    {
        if( m_behaviorEventable == nullptr )
        {
            return nullptr;
        }

        EventationInterface * event = m_behaviorEventable->getEventation();

        return event;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Entity::_activate()
    {
        EVENTABLE_METHOD( this, EVENT_ENTITY_PREPARATION )
            ->onEntityPreparation( m_behavior );

        bool successful = Node::_activate();

        return successful;
    }
    //////////////////////////////////////////////////////////////////////////
    void Entity::_afterActivate()
    {
        Node::_afterActivate();

        EVENTABLE_METHOD( this, EVENT_ENTITY_ACTIVATE )
            ->onEntityActivate( m_behavior );
    }
    //////////////////////////////////////////////////////////////////////////
    void Entity::_deactivate()
    {
        EVENTABLE_METHOD( this, EVENT_ENTITY_PREPARATION_DEACTIVATE )
            ->onEntityPreparationDeactivate( m_behavior );

        Node::_deactivate();
    }
    //////////////////////////////////////////////////////////////////////////
    void Entity::_afterDeactivate()
    {
        Node::_afterDeactivate();

        EVENTABLE_METHOD( this, EVENT_ENTITY_DEACTIVATE )
            ->onEntityDeactivate( m_behavior );
    }
    //////////////////////////////////////////////////////////////////////////
    bool Entity::_compile()
    {
        EVENTABLE_METHOD( this, EVENT_ENTITY_COMPILE )
            ->onEntityCompile( m_behavior );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void Entity::_release()
    {
        EVENTABLE_METHOD( this, EVENT_ENTITY_RELEASE )
            ->onEntityRelease( m_behavior );
    }
    //////////////////////////////////////////////////////////////////////////
    void Entity::onCreate()
    {
        EVENTABLE_METHOD( this, EVENT_ENTITY_CREATE )
            ->onEntityCreate( m_behavior, this );
    }
    //////////////////////////////////////////////////////////////////////////
    void Entity::_destroy()
    {
        this->release();

        Node * old_parent = this->getParent();

        EVENTABLE_METHOD( this, EVENT_ENTITY_DESTROY )
            ->onEntityDestroy( m_behavior );

        m_behavior = nullptr;

        Node * new_parent = this->getParent();

        if( old_parent != new_parent )
        {
            LOGGER_ERROR( "entity %s:%s script event EVENT_DESTROY replace node to other hierarchy"
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
    void Entity::_render( const RenderContext * _context )
    {
        (void)_context;

        //Empty
    }
}