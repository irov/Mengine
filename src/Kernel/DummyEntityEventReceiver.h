#pragma once

#include "Kernel/Entity.h"

namespace Mengine
{
    class DummyEntityEventReceiver
        : public EntityEventReceiverInterface
    {
    public:
        DummyEntityEventReceiver();
        ~DummyEntityEventReceiver() override;

    public:
        void onEntityCreate( const EntityBehaviorInterfacePtr & _behavior, class Entity * _entity ) override;
        void onEntityDestroy( const EntityBehaviorInterfacePtr & _behavior ) override;
        void onEntityPreparation( const EntityBehaviorInterfacePtr & _behavior ) override;
        void onEntityActivate( const EntityBehaviorInterfacePtr & _behavior ) override;
        void onEntityPreparationDeactivate( const EntityBehaviorInterfacePtr & _behavior ) override;
        void onEntityDeactivate( const EntityBehaviorInterfacePtr & _behavior ) override;
        void onEntityCompile( const EntityBehaviorInterfacePtr & _behavior ) override;
        void onEntityRelease( const EntityBehaviorInterfacePtr & _behavior ) override;
    };
}