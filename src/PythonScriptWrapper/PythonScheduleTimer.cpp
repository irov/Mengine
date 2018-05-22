#include "PythonScheduleTimer.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    PythonScheduleTimer::PythonScheduleTimer()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    PythonScheduleTimer::~PythonScheduleTimer()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    void PythonScheduleTimer::initialize( const pybind::object & _cb, const pybind::args & _args )
    {
        m_cb = _cb;
        m_args = _args;
    }
    //////////////////////////////////////////////////////////////////////////
    void PythonScheduleTimer::onScheduleUpdate( uint32_t _id, uint32_t _iterate, float _delay )
    {
        m_cb.call_args( _id, _iterate, _delay, false, false, m_args );
    }
    //////////////////////////////////////////////////////////////////////////
    void PythonScheduleTimer::onScheduleComplete( uint32_t _id )
    {
        m_cb.call_args( _id, 0, 0.f, true, false, m_args );
    }
    //////////////////////////////////////////////////////////////////////////
    void PythonScheduleTimer::onScheduleStop( uint32_t _id )
    {
        m_cb.call_args( _id, 0, 0.f, false, true, m_args );
    }
}