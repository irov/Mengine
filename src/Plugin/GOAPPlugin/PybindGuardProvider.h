#   pragma once

#   include "GOAP/GuardProvider.h"

#   include "pybind/object.hpp"

namespace Menge
{
    class PybindGuardProvider
        : public GOAP::GuardProvider
    {
    public:
        PybindGuardProvider( const pybind::object & _obj, const pybind::detail::args_operator_t & _args );

    protected:
        void onGuard() override;

    protected:
        pybind::object m_obj;
        pybind::detail::args_operator_t m_args;
    };
}