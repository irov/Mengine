#pragma once

#include "Interface/EventationInterface.h"
#include "Interface/EventReceiverInterface.h"

#include "Kernel/EntityBehaviorInterface.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    enum EEntityEventFlag
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
    class Entity;
    //////////////////////////////////////////////////////////////////////////
    class EntityEventReceiverInterface
        : public EventReceiverInterface
    {
    public:
        virtual bool onEntityCreate( const EntityBehaviorInterfacePtr & _behavior, Entity * _entity ) = 0;
        virtual void onEntityDestroy( const EntityBehaviorInterfacePtr & _behavior ) = 0;
        virtual bool onEntityPreparation( const EntityBehaviorInterfacePtr & _behavior ) = 0;
        virtual bool onEntityActivate( const EntityBehaviorInterfacePtr & _behavior ) = 0;
        virtual void onEntityPreparationDeactivate( const EntityBehaviorInterfacePtr & _behavior ) = 0;
        virtual void onEntityDeactivate( const EntityBehaviorInterfacePtr & _behavior ) = 0;
        virtual void onEntityCompile( const EntityBehaviorInterfacePtr & _behavior ) = 0;
        virtual void onEntityRelease( const EntityBehaviorInterfacePtr & _behavior ) = 0;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<EntityEventReceiverInterface> EntityEventReceiverInterfacePtr;
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
}