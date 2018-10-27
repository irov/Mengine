#pragma once

#include "Interface/ScheduleManagerInterface.h"

#include "pybind/pybind.hpp"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class PythonScheduleTiming
        : public ScheduleTimingInterface
    {
    public:
        PythonScheduleTiming();
        ~PythonScheduleTiming() override;

    public:
        void initialize( const pybind::object & _cb, const pybind::args & _args );

    protected:
        void onScheduleUpdate( uint32_t _id, uint32_t _iterate, float _delay ) override;
        void onScheduleComplete( uint32_t _id ) override;
        void onScheduleStop( uint32_t _id ) override;

    protected:
        pybind::object m_cb;
        pybind::args m_args;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<PythonScheduleTiming> PythonScheduleTimerPtr;
}