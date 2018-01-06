#   pragma once

#   include "GOAP/ScopeProvider.h"

#   include "pybind/object.hpp"

namespace Menge
{
    class PybindScopeProvider
        : public GOAP::ScopeProvider
    {
    public:
        PybindScopeProvider( const pybind::object & _obj, const pybind::args & _args );

    protected:
        bool onScope( const GOAP::SourcePtr & _source ) override;

    protected:
        pybind::object m_obj;
        pybind::args m_args;
    };
}