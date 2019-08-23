#include "PythonScheduleTiming.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    PythonScheduleTiming::PythonScheduleTiming()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    PythonScheduleTiming::~PythonScheduleTiming()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    void PythonScheduleTiming::initialize( const pybind::object & _cb, const pybind::args & _args )
    {
        m_cb = _cb;
        m_args = _args;
    }
    //////////////////////////////////////////////////////////////////////////
    void PythonScheduleTiming::onSchedulerTiming( uint32_t _id, uint32_t _iterate, float _delay )
    {
        m_cb.call_args( _id, _iterate, _delay, m_args );
    }
    ////////////////////////////////////////////////////////////////////////////
    //void PythonScheduleTiming::onScheduleComplete( uint32_t _id )
    //{
    //    m_cb.call_args( _id, 0, 0.f, true, false, m_args );
    //}
    ////////////////////////////////////////////////////////////////////////////
    //void PythonScheduleTiming::onScheduleStop( uint32_t _id )
    //{
    //    m_cb.call_args( _id, 0, 0.f, false, true, m_args );
    //}
}