#pragma once

#include "NodeAreaOfInterestTriggerEventReceiverInterface.h"

#include "Environment/Python/PythonEventReceiver.h"

#include "Kernel/Factorable.h"

namespace Mengine
{
    class PythonNodeAreaOfInterestTriggerEventReceiver
        : public PythonEventReceiver
        , public NodeAreaOfInterestTriggerEventReceiverInterface
        , public Factorable
    {
    public:
        PythonNodeAreaOfInterestTriggerEventReceiver();
        ~PythonNodeAreaOfInterestTriggerEventReceiver() override;

    public:
        void onAreaOfInterestTriggerEnter( const AreaOfInterestActorProviderInterfacePtr & _enemy, uint32_t _iff1, uint32_t _iff2 ) override;
        void onAreaOfInterestTriggerLeave( const AreaOfInterestActorProviderInterfacePtr & _enemy, uint32_t _iff1, uint32_t _iff2 ) override;
    };
}
