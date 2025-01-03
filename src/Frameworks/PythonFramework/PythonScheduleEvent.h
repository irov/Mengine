#pragma once

#include "Interface/SchedulerInterface.h"

#include "Environment/Python/PythonIncluder.h"
#include "Environment/Python/PythonCallbackProvider.h"

#include "Kernel/Factorable.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class PythonScheduleEvent
        : public SchedulerEventInterface
        , public PythonCallbackProvider
        , public Factorable
    {
        DECLARE_FACTORABLE( PythonScheduleEvent );

    public:
        PythonScheduleEvent();
        ~PythonScheduleEvent() override;

    protected:
        void onSchedulerComplete( UniqueId _id ) override;
        void onSchedulerStop( UniqueId _id ) override;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<PythonScheduleEvent> PythonScheduleEventPtr;
    //////////////////////////////////////////////////////////////////////////
}