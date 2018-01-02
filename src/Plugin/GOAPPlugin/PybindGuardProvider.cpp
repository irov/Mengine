#   include "PybindGuardProvider.h"

namespace Menge
{
    //////////////////////////////////////////////////////////////////////////
    PybindGuardProvider::PybindGuardProvider( const pybind::object & _obj, const pybind::args & _args )
        : m_obj( _obj )
        , m_args( _args )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    void PybindGuardProvider::onGuard()
    {
        m_obj.call_args( m_args );
    }
}