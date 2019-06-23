#pragma once

#include "Interface/EventationInterface.h"

#include "Kernel/Node.h"
#include "Kernel/NoneRender.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class EntityBehaviorInterface
        : public Mixin
    {
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<EntityBehaviorInterface> EntityBehaviorInterfacePtr;
    //////////////////////////////////////////////////////////////////////////
    enum EntityEventFlag
    {
        EVENT_ENTITY_CREATE = 0,
        EVENT_ENTITY_DESTROY,
        EVENT_ENTITY_PREPARATION,
        EVENT_ENTITY_ACTIVATE,
        EVENT_ENTITY_PREPARATION_DEACTIVATE,
        EVENT_ENTITY_DEACTIVATE,
        EVENT_ENTITY_COMPILE,
        EVENT_ENTITY_RELEASE,
        __EVENT_ENTITY_LAST__
    };
    //////////////////////////////////////////////////////////////////////////
    class EntityEventReceiver
        : public EventReceiverInterface
    {
    public:
        virtual void onEntityCreate( const EntityBehaviorInterfacePtr & _behavior, Node * _node ) = 0;
        virtual void onEntityDestroy( const EntityBehaviorInterfacePtr & _behavior ) = 0;
        virtual void onEntityPreparation( const EntityBehaviorInterfacePtr & _behavior ) = 0;
        virtual void onEntityActivate( const EntityBehaviorInterfacePtr & _behavior ) = 0;
        virtual void onEntityPreparationDeactivate( const EntityBehaviorInterfacePtr & _behavior ) = 0;
        virtual void onEntityDeactivate( const EntityBehaviorInterfacePtr & _behavior ) = 0;
        virtual void onEntityCompile( const EntityBehaviorInterfacePtr & _behavior ) = 0;
        virtual void onEntityRelease( const EntityBehaviorInterfacePtr & _behavior ) = 0;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<EntityEventReceiver> EntityEventReceiverPtr;
    //////////////////////////////////////////////////////////////////////////
    class Entity
        : public Node
        , public NoneRender
    {
        DECLARE_VISITABLE( Node );
        DECLARE_RENDERABLE();
        DECLARE_EVENTABLE_TYPE( EntityEventReceiver );

    public:
        Entity();
        ~Entity() override;

    public:
        void setPrototype( const ConstString & _prototype );
        const ConstString & getPrototype() const;

    public:
        void setBehaviorEventable( const EventablePtr & _behaviorEventable );
        const EventablePtr & getBehaviorEventable() const;

    public:
        void setBehavior( const EntityBehaviorInterfacePtr & _behavior );
        const EntityBehaviorInterfacePtr & getBehavior() const;

    public:
        EventationInterface * getEventation() override;
        const EventationInterface * getEventation() const override;

    public:
        void onCreate();
        void onDestroy();

    protected:
        bool _activate() override;
        void _afterActivate() override;

        void _deactivate() override;
        void _afterDeactivate() override;

        bool _compile() override;
        void _release() override;

    public:
        void _destroy() override;

    protected:
        ConstString m_prototype;

        EventablePtr m_behaviorEventable;
        EntityBehaviorInterfacePtr m_behavior;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<Entity> EntityPtr;
    //////////////////////////////////////////////////////////////////////////
}