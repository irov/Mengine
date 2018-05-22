#pragma once

#include "Interface/ScheduleManagerInterface.h"

#include "pybind/pybind.hpp"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class PythonSchedulePipe
        : public SchedulePipeInterface
    {
    public:
        PythonSchedulePipe();
        ~PythonSchedulePipe() override;

    public:
        void initialize( const pybind::object & _cb, const pybind::args & _args );

    protected:
        float onSchedulePipe( uint32_t _id, uint32_t _index ) override;

    protected:
        pybind::object m_cb;
        pybind::args m_args;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<PythonSchedulePipe> PythonSchedulePipePtr;
    //////////////////////////////////////////////////////////////////////////
}