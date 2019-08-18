#pragma once

#include "Environment/Python/PythonEventReceiver.h"

#include "Kernel/Entity.h"
#include "Kernel/Factorable.h"

namespace Mengine
{
    class PythonEntityEventReceiver
        : public EntityEventReceiverInterface
        , public PythonEventReceiver
        , public Factorable
    {
    public:
        PythonEntityEventReceiver();
        ~PythonEntityEventReceiver() override;

    protected:
        void onEntityPreparation( const EntityBehaviorInterfacePtr & _behavior ) override;
        void onEntityActivate( const EntityBehaviorInterfacePtr & _behavior ) override;
        void onEntityPreparationDeactivate( const EntityBehaviorInterfacePtr & _behavior ) override;
        void onEntityDeactivate( const EntityBehaviorInterfacePtr & _behavior ) override;
        void onEntityCompile( const EntityBehaviorInterfacePtr & _behavior ) override;
        void onEntityRelease( const EntityBehaviorInterfacePtr & _behavior ) override;
        void onEntityCreate( const EntityBehaviorInterfacePtr & _behavior, Entity * _entity ) override;
        void onEntityDestroy( const EntityBehaviorInterfacePtr & _behavior ) override;
    };
}