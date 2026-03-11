#pragma once

#include "Environment/Python/PythonEventReceiver.h"

#include "Kernel/LayerEventReceiverInterface.h"
#include "Kernel/Factorable.h"

namespace Mengine
{
    class PythonLayerEventReceiver
        : public LayerEventReceiverInterface
        , public PythonEventReceiver
        , public Factorable
    {
    public:
        PythonLayerEventReceiver();
        ~PythonLayerEventReceiver() override;

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