#   include "PybindCallbackProvider.h"

#   include "pybind/functor.hpp"

namespace Menge
{
    //////////////////////////////////////////////////////////////////////////
    PybindCallbackProvider::PybindCallbackProvider( const pybind::object & _obj, const pybind::args & _args )
        : m_obj( _obj )
        , m_args( _args )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    void PybindCallbackProvider::onCallback( const GOAP::CallbackObserverPtr & _callback, bool _skip )
    {
        pybind::kernel_interface * kernel = pybind::get_kernel();

        PyObject * cb = pybind::create_functor_ptr( kernel, "PybindCallbackProvider::onCallback", _callback.get()
            , &GOAP::CallbackObserver::IntrusivePtrAddRef
            , &GOAP::CallbackObserver::IntrusivePtrDecRef
            , &GOAP::CallbackObserver::onCallback 
        );

        m_obj.call_args( cb, _skip, m_args );
    }
}