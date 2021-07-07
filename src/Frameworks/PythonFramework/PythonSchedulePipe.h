#pragma once

#include "Interface/SchedulerInterface.h"

#include "Kernel/Factorable.h"

#include "pybind/pybind.hpp"

namespace Mengine
{
    class PythonSchedulePipe
        : public SchedulerPipeInterface
        , public Factorable
    {
        DECLARE_FACTORABLE( PythonSchedulePipe );

    public:
        PythonSchedulePipe();
        ~PythonSchedulePipe() override;

    public:
        void initialize( const pybind::object & _cb, const pybind::args & _args );

    protected:
        float onSchedulerPipe( uint32_t _id, uint32_t _index ) override;

    protected:
        pybind::object m_cb;
        pybind::args m_args;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<PythonSchedulePipe> PythonSchedulePipePtr;
    //////////////////////////////////////////////////////////////////////////
}