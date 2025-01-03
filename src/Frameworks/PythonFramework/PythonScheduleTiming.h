#pragma once

#include "Interface/SchedulerInterface.h"

#include "Environment/Python/PythonIncluder.h"
#include "Environment/Python/PythonCallbackProvider.h"

#include "Kernel/Factorable.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class PythonScheduleTiming
        : public SchedulerTimingInterface
        , public PythonCallbackProvider
        , public Factorable
    {
        DECLARE_FACTORABLE( PythonScheduleTiming );

    public:
        PythonScheduleTiming();
        ~PythonScheduleTiming() override;

    protected:
        void onSchedulerTiming( UniqueId _id, uint32_t _iterate, float _delay ) override;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<PythonScheduleTiming, SchedulerTimingInterface> PythonScheduleTimingPtr;
    //////////////////////////////////////////////////////////////////////////
}