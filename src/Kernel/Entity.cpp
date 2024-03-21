#include "Entity.h"

#include "Kernel/EntityEventReceiverInterface.h"

#include "Kernel/Logger.h"
#include "Kernel/BaseEventation.h"
#include "Kernel/EventableHelper.h"
#include "Kernel/Assertion.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    Entity::Entity()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    Entity::~Entity()
    {
        MENGINE_ASSERTION_FATAL( m_behaviorEventable == nullptr, "entity behavior eventable not NULL" );
        MENGINE_ASSERTION_FATAL( m_behavior == nullptr, "entity behavior not NULL" );
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
    const RenderScissorInterfacePtr & Entity::getPickerScissor() const
    {
        const RenderScissorInterfacePtr & scissor = this->getRenderScissor();

        return scissor;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Entity::_activate()
    {
        if( Node::_activate() == false )
        {
            return false;
        }

        bool successful = EVENTABLE_METHODR( EVENT_ENTITY_PREPARATION, true )
            ->onEntityPreparation( m_behavior );

        return successful;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Entity::_afterActivate()
    {
        if( Node::_afterActivate() == false )
        {
            return false;
        }

        bool successful = EVENTABLE_METHODR( EVENT_ENTITY_ACTIVATE, true )
            ->onEntityActivate( m_behavior );

        return successful;
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
    bool Entity::create()
    {
        if( this->_create() == false )
        {
            return false;
        }

        bool successful = EVENTABLE_METHODR( EVENT_ENTITY_CREATE, true )
            ->onEntityCreate( m_behavior, this );

        return successful;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Entity::_create()
    {
        //Empty

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void Entity::_dispose()
    {
        Node * old_parent = this->getParent();

        EVENTABLE_METHOD( EVENT_ENTITY_DESTROY )
            ->onEntityDestroy( m_behavior );

        if( m_behavior != nullptr )
        {
            m_behavior->finalize();
            m_behavior = nullptr;
        }

        m_behaviorEventable = nullptr;

        Node * new_parent = this->getParent();

        if( old_parent != new_parent )
        {
            LOGGER_ERROR( "entity type '%s' name '%s' event EVENT_ENTITY_DESTROY replace node to other hierarchy"
                , this->getType().c_str()
                , this->getName().c_str()
            );
        }

        Node::_dispose();
    }
    //////////////////////////////////////////////////////////////////////////
    void Entity::_destroy()
    {
        if( this->isDispose() == true )
        {
            return;
        }

        this->release();

        this->_dispose();

        this->removeFromParent();
        this->removeChildren( []( const NodePtr & )
        {} );

#if defined(MENGINE_USE_SCRIPT_SERVICE)
        this->unwrap();
#endif
    }
    //////////////////////////////////////////////////////////////////////////
}