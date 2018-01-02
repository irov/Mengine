#   pragma once

#   include "GOAP/FunctionProvider.h"

#   include "pybind/object.hpp"

namespace Menge
{
    class PybindFunctionProvider
        : public GOAP::FunctionProvider
    {
    public:
        PybindFunctionProvider( const pybind::object & _obj, const pybind::args & _args );

    protected:
        void onFunction() override;

    protected:
        pybind::object m_obj;
        pybind::args m_args;
    };
}