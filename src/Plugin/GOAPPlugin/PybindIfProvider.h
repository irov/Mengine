#   pragma once

#   include "GOAP/IfProvider.h"

#   include "pybind/object.hpp"

namespace Menge
{
    class PybindIfProvider
        : public GOAP::IfProvider
    {
    public:
        PybindIfProvider( const pybind::object & _obj, const pybind::args & _args );

    protected:
        bool onIf() override;

    protected:
        pybind::object m_obj;
        pybind::args m_args;
    };
}