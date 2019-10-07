#pragma once

#include "Interface/ServantInterface.h"
#include "Interface/EventationInterface.h"

#include "Kernel/Node.h"
#include "Kernel/DummyRender.h"
#include "Kernel/DummyPicker.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class EntityBehaviorInterface
        : public ServantInterface
    {
    public:
        virtual bool initialize() = 0;
        virtual void finalize() = 0;
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
    class EntityEventReceiverInterface
        : public EventReceiverInterface
    {
    public:
        virtual void onEntityCreate( const EntityBehaviorInterfacePtr & _behavior, class Entity * _entity ) = 0;
        virtual void onEntityDestroy( const EntityBehaviorInterfacePtr & _behavior ) = 0;
        virtual void onEntityPreparation( const EntityBehaviorInterfacePtr & _behavior ) = 0;
        virtual void onEntityActivate( const EntityBehaviorInterfacePtr & _behavior ) = 0;
        virtual void onEntityPreparationDeactivate( const EntityBehaviorInterfacePtr & _behavior ) = 0;
        virtual void onEntityDeactivate( const EntityBehaviorInterfacePtr & _behavior ) = 0;
        virtual void onEntityCompile( const EntityBehaviorInterfacePtr & _behavior ) = 0;
        virtual void onEntityRelease( const EntityBehaviorInterfacePtr & _behavior ) = 0;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<EntityEventReceiverInterface> EntityEventReceiverPtr;
    //////////////////////////////////////////////////////////////////////////
    EVENTATION_TYPEID( EntityEventReceiverInterface, EVENT_ENTITY_CREATE );
    EVENTATION_TYPEID( EntityEventReceiverInterface, EVENT_ENTITY_DESTROY );
    EVENTATION_TYPEID( EntityEventReceiverInterface, EVENT_ENTITY_PREPARATION );
    EVENTATION_TYPEID( EntityEventReceiverInterface, EVENT_ENTITY_ACTIVATE );
    EVENTATION_TYPEID( EntityEventReceiverInterface, EVENT_ENTITY_PREPARATION_DEACTIVATE );
    EVENTATION_TYPEID( EntityEventReceiverInterface, EVENT_ENTITY_DEACTIVATE );
    EVENTATION_TYPEID( EntityEventReceiverInterface, EVENT_ENTITY_COMPILE );
    EVENTATION_TYPEID( EntityEventReceiverInterface, EVENT_ENTITY_RELEASE );
    //////////////////////////////////////////////////////////////////////////
    class Entity
        : public Node
        , public DummyRender
        , public DummyPicker
    {
        DECLARE_VISITABLE( Node );
        DECLARE_RENDERABLE();
        DECLARE_PICKERABLE();

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

    protected:
        const RenderViewportInterfacePtr & getPickerViewport() const override;
        const RenderCameraInterfacePtr & getPickerCamera() const override;

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
    typedef IntrusiveNodePtr<Entity> EntityPtr;
    //////////////////////////////////////////////////////////////////////////
}