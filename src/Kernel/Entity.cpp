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
    const EventationInterface * Entity::getEventation() const
    {
        if( m_behaviorEventable == nullptr )
        {
            return nullptr;
        }

        const EventationInterface * event = m_behaviorEventable->getEventation();

        return event;
    }
    //////////////////////////////////////////////////////////////////////////
    const RenderViewportInterfacePtr & Entity::getPickerViewport() const
    {
        const RenderViewportInterfacePtr & viewport = this->getRenderViewport();

        return viewport;
    }
    //////////////////////////////////////////////////////////////////////////
    const RenderCameraInterfacePtr & Entity::getPickerCamera() const
    {
        const RenderCameraInterfacePtr & camera = this->getRenderCamera();

        return camera;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Entity::_activate()
    {
        EVENTABLE_METHOD( EVENT_ENTITY_PREPARATION )
            ->onEntityPreparation( m_behavior );

        bool successful = Node::_activate();

        return successful;
    }
    //////////////////////////////////////////////////////////////////////////
    void Entity::_afterActivate()
    {
        Node::_afterActivate();

        EVENTABLE_METHOD( EVENT_ENTITY_ACTIVATE )
            ->onEntityActivate( m_behavior );
    }
    //////////////////////////////////////////////////////////////////////////
    void Entity::_deactivate()
    {
        EVENTABLE_METHOD( EVENT_ENTITY_PREPARATION_DEACTIVATE )
            ->onEntityPreparationDeactivate( m_behavior );

        Node::_deactivate();
    }
    //////////////////////////////////////////////////////////////////////////
    void Entity::_afterDeactivate()
    {
        Node::_afterDeactivate();

        EVENTABLE_METHOD( EVENT_ENTITY_DEACTIVATE )
            ->onEntityDeactivate( m_behavior );
    }
    //////////////////////////////////////////////////////////////////////////
    bool Entity::_compile()
    {
        EVENTABLE_METHOD( EVENT_ENTITY_COMPILE )
            ->onEntityCompile( m_behavior );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void Entity::_release()
    {
        EVENTABLE_METHOD( EVENT_ENTITY_RELEASE )
            ->onEntityRelease( m_behavior );
    }
    //////////////////////////////////////////////////////////////////////////
    void Entity::onCreate()
    {
        EVENTABLE_METHOD( EVENT_ENTITY_CREATE )
            ->onEntityCreate( m_behavior, this );
    }
    //////////////////////////////////////////////////////////////////////////
    void Entity::onDestroy()
    {
        EVENTABLE_METHOD( EVENT_ENTITY_DESTROY )
            ->onEntityDestroy( m_behavior );

        m_behavior = nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
    void Entity::_destroy()
    {
        this->release();

        Node * old_parent = this->getParent();

        if( m_behavior != nullptr )
        {
            EVENTABLE_METHOD( EVENT_ENTITY_DESTROY )
                ->onEntityDestroy( m_behavior );

            m_behavior = nullptr;
        }

        Node * new_parent = this->getParent();

        if( old_parent != new_parent )
        {
            LOGGER_ERROR( "entity '%s:%s' script event EVENT_DESTROY replace node to other hierarchy"
                , this->getType().c_str()
                , this->getName().c_str()
            );

            return;
        }

        this->removeFromParent();
        this->destroyChildren( []( const NodePtr & ) {} );        

        this->unwrap();
    }
}