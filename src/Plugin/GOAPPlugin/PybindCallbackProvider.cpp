#   include "PybindCallbackProvider.h"

#   include "pybind/functor.hpp"

namespace Menge
{
    //////////////////////////////////////////////////////////////////////////
    PybindCallbackProvider::PybindCallbackProvider( const pybind::object & _obj, const pybind::detail::args_operator_t & _args )
        : m_obj( _obj )
        , m_args( _args )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    void PybindCallbackProvider::onCallback( GOAP::CallbackObserver * _callback, bool _skip )
    {
        pybind::kernel_interface * kernel = pybind::get_kernel();

        PyObject * cb = pybind::create_functor( kernel, "PybindCallbackProvider::onCallback", _callback, &GOAP::CallbackObserver::onCallback );

        m_obj.call_args( cb, _skip, m_args );
    }
}