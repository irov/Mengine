#   include "PybindIfProvider.h"

namespace Menge
{
    //////////////////////////////////////////////////////////////////////////
    PybindIfProvider::PybindIfProvider( const pybind::object & _obj, const pybind::args & _args )
        : m_obj( _obj )
        , m_args( _args )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool PybindIfProvider::onIf()
    {
        bool result = m_obj.call_args( m_args );

        return result;
    }
}