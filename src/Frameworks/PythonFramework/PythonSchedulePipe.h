#pragma once

#include "Interface/SchedulerInterface.h"

#include "Environment/Python/PythonIncluder.h"
#include "Environment/Python/PythonCallbackProvider.h"

#include "Kernel/Factorable.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class PythonSchedulePipe
        : public SchedulerPipeInterface
        , public PythonCallbackProvider
        , public Factorable
    {
        DECLARE_FACTORABLE( PythonSchedulePipe );

    public:
        PythonSchedulePipe();
        ~PythonSchedulePipe() override;

    protected:
        float onSchedulerPipe( UniqueId _id, uint32_t _index ) override;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<PythonSchedulePipe, SchedulerPipeInterface> PythonSchedulePipePtr;
    //////////////////////////////////////////////////////////////////////////
}