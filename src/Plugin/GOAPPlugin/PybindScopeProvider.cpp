#   include "PybindScopeProvider.h"

namespace Menge
{
    //////////////////////////////////////////////////////////////////////////
    PybindScopeProvider::PybindScopeProvider( const pybind::object & _obj, const pybind::args & _args )
        : m_obj( _obj )
        , m_args( _args )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool PybindScopeProvider::onScope( const GOAP::SourcePtr & _source )
    {
        bool successful = m_obj.call_args( _source, m_args );

        return successful;
    }
}