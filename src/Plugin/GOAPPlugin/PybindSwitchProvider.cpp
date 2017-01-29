#   include "PybindSwitchProvider.h"

namespace Menge
{
    //////////////////////////////////////////////////////////////////////////
    PybindSwitchProvider::PybindSwitchProvider( const pybind::object & _obj, const pybind::detail::args_operator_t & _args )
        : m_obj( _obj )
        , m_args( _args )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t PybindSwitchProvider::onSwitch()
    {
        uint32_t index = m_obj.call_args( m_args );

        return index;
    }
}