#   include "PybindFunctionProvider.h"

namespace Menge
{
    //////////////////////////////////////////////////////////////////////////
    PybindFunctionProvider::PybindFunctionProvider( const pybind::object & _obj, const pybind::args & _args )
        : m_obj( _obj )
        , m_args( _args )
    {
    }    
    //////////////////////////////////////////////////////////////////////////
    void PybindFunctionProvider::onFunction()
    {
        m_obj.call_args( m_args );
    }
}