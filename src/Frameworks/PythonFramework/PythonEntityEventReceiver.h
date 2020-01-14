#pragma once

#include "Environment/Python/PythonEventReceiver.h"

#include "Kernel/EntityEventReceiverInterface.h"
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
        bool onEntityPreparation( const EntityBehaviorInterfacePtr & _behavior ) override;
        bool onEntityActivate( const EntityBehaviorInterfacePtr & _behavior ) override;
        void onEntityPreparationDeactivate( const EntityBehaviorInterfacePtr & _behavior ) override;
        void onEntityDeactivate( const EntityBehaviorInterfacePtr & _behavior ) override;
        void onEntityCompile( const EntityBehaviorInterfacePtr & _behavior ) override;
        void onEntityRelease( const EntityBehaviorInterfacePtr & _behavior ) override;
        bool onEntityCreate( const EntityBehaviorInterfacePtr & _behavior, Entity * _entity ) override;
        void onEntityDestroy( const EntityBehaviorInterfacePtr & _behavior ) override;
    };
}