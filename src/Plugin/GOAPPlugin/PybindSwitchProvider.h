#   pragma once

#   include "GOAP/SwitchProvider.h"

#   include "pybind/object.hpp"

namespace Menge
{
    class PybindSwitchProvider
        : public GOAP::SwitchProvider
    {
    public:
        PybindSwitchProvider( const pybind::object & _obj, const pybind::args & _args );

    protected:
        uint32_t onSwitch() override;

    protected:
        pybind::object m_obj;
        pybind::args m_args;
    };
}