#   pragma once

#   include "GOAP/CallbackProvider.h"

#   include "pybind/object.hpp"

namespace Menge
{
    class PybindCallbackProvider
        : public GOAP::CallbackProvider
    {
    public:
        PybindCallbackProvider( const pybind::object & _obj, const pybind::args & _args );

    protected:
        void onCallback( const GOAP::CallbackObserverPtr & _callback, bool _skip ) override;

    protected:
        pybind::object m_obj;
        pybind::args m_args;
    };
}