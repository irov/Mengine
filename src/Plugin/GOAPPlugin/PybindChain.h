#   pragma once

#   include "GOAP/GOAP.h"

#   include "pybind/bindable.hpp"

namespace Menge
{
    class PybindChain
        : public GOAP::Chain
        , public pybind::bindable
    {
    public:
        PybindChain( const GOAP::SourcePtr & _source );

    protected:
        PyObject * _embedded() override;
    };

    typedef GOAP::IntrusivePtr<PybindChain> PybindChainPtr;
}