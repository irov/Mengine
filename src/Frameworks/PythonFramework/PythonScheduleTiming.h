#pragma once

#include "Interface/SchedulerInterface.h"

#include "Environment/Python/PythonIncluder.h"

#include "Kernel/Factorable.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class PythonScheduleTiming
        : public SchedulerTimingInterface
        , public Factorable
    {
        DECLARE_FACTORABLE( PythonScheduleTiming );

    public:
        PythonScheduleTiming();
        ~PythonScheduleTiming() override;

    public:
        void initialize( const pybind::object & _cb, const pybind::args & _args );

    protected:
        void onSchedulerTiming( uint32_t _id, uint32_t _iterate, float _delay ) override;

    protected:
        pybind::object m_cb;
        pybind::args m_args;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<PythonScheduleTiming> PythonScheduleTimingPtr;
    //////////////////////////////////////////////////////////////////////////
}