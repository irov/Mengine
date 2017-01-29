#   pragma once

#   include "GOAP/IfProvider.h"

#   include "pybind/object.hpp"

namespace Menge
{
    class PybindIfProvider
        : public GOAP::IfProvider
    {
    public:
        PybindIfProvider( const pybind::object & _obj, const pybind::detail::args_operator_t & _args );

    protected:
        bool onIf() override;

    protected:
        pybind::object m_obj;
        pybind::detail::args_operator_t m_args;
    };
}