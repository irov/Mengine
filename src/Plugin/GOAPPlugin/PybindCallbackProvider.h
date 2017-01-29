#   pragma once

#   include "GOAP/CallbackProvider.h"

#   include "pybind/object.hpp"

namespace Menge
{
    class PybindCallbackProvider
        : public GOAP::CallbackProvider
    {
    public:
        PybindCallbackProvider( const pybind::object & _obj, const pybind::detail::args_operator_t & _args );

    protected:
        void onCallback( GOAP::CallbackObserver * _callback, bool _skip ) override;

    protected:
        pybind::object m_obj;
        pybind::detail::args_operator_t m_args;
    };
}