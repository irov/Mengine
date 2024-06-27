#pragma once

#include "Interface/SchedulerInterface.h"

#include "Environment/Python/PythonIncluder.h"

#include "Kernel/Factorable.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class PythonScheduleEvent
        : public SchedulerEventInterface
        , public Factorable
    {
        DECLARE_FACTORABLE( PythonScheduleEvent );

    public:
        PythonScheduleEvent();
        ~PythonScheduleEvent() override;

    public:
        void initialize( const pybind::object & _cb, const pybind::args & _args );

    protected:
        void onSchedulerComplete( UniqueId _id ) override;
        void onSchedulerStop( UniqueId _id ) override;

    protected:
        pybind::object m_cb;
        pybind::args m_args;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<PythonScheduleEvent> PythonScheduleEventPtr;
    //////////////////////////////////////////////////////////////////////////
}