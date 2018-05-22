#pragma once

#include "Interface/ScheduleManagerInterface.h"

#include "pybind/pybind.hpp"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class PythonScheduleEvent
        : public ScheduleEventInterface
    {
    public:
        PythonScheduleEvent();
        ~PythonScheduleEvent() override;

    public:
        void initialize( const pybind::object & _cb, const pybind::args & _args );

    protected:
        void onScheduleComplete( uint32_t _id ) override;
        void onScheduleStop( uint32_t _id ) override;

    protected:
        pybind::object m_cb;
        pybind::args m_args;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<PythonScheduleEvent> PythonScheduleEventPtr;
    //////////////////////////////////////////////////////////////////////////
}