#include "PythonScheduleEvent.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    PythonScheduleEvent::PythonScheduleEvent()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    PythonScheduleEvent::~PythonScheduleEvent()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    void PythonScheduleEvent::initialize( const pybind::object & _cb, const pybind::args & _args )
    {
        m_cb = _cb;
        m_args = _args;
    }
    //////////////////////////////////////////////////////////////////////////
    void PythonScheduleEvent::onSchedulerComplete( uint32_t _id )
    {
        m_cb.call_args( _id, true, m_args );
    }
    //////////////////////////////////////////////////////////////////////////
    void PythonScheduleEvent::onSchedulerStop( uint32_t _id )
    {
        m_cb.call_args( _id, false, m_args );
    }
}