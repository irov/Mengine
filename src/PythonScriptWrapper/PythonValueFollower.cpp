#include "PythonValueFollower.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    PythonValueFollower::PythonValueFollower()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    PythonValueFollower::~PythonValueFollower()
    {
    }   
    //////////////////////////////////////////////////////////////////////////        
    void PythonValueFollower::setCb( const pybind::object & _cb, const pybind::args & _args )
    {
        m_cb = _cb;
        m_args = _args;
    }
    //////////////////////////////////////////////////////////////////////////
    void PythonValueFollower::callCb( float _value )
    {
        m_cb.call_args( _value, m_args );
    }
}