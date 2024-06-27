#include "PythonSchedulePipe.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    PythonSchedulePipe::PythonSchedulePipe()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    PythonSchedulePipe::~PythonSchedulePipe()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    void PythonSchedulePipe::initialize( const pybind::object & _cb, const pybind::args & _args )
    {
        m_cb = _cb;
        m_args = _args;
    }
    //////////////////////////////////////////////////////////////////////////
    float PythonSchedulePipe::onSchedulerPipe( UniqueId _id, uint32_t _index )
    {
        float delay = m_cb.call_args( _id, _index, m_args );

        return delay;
    }
}