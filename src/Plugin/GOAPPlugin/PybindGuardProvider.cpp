#   include "PybindGuardProvider.h"

namespace Menge
{
    //////////////////////////////////////////////////////////////////////////
    PybindGuardProvider::PybindGuardProvider( const pybind::object & _obj, const pybind::detail::args_operator_t & _args )
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