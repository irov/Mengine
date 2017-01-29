#   pragma once

#   include "GOAP/ScopeProvider.h"

#   include "pybind/object.hpp"

namespace Menge
{
    class PybindScopeProvider
        : public GOAP::ScopeProvider
    {
    public:
        PybindScopeProvider( const pybind::object & _obj, const pybind::detail::args_operator_t & _args );

    protected:
        bool onScope( const GOAP::SourcePtr & _source ) override;

    protected:
        pybind::object m_obj;
        pybind::detail::args_operator_t m_args;
    };
}