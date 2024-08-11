#include "PythonAndroidSemaphoreListener.h"

#include "Kernel/ThreadHelper.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    PythonAndroidSemaphoreListener::PythonAndroidSemaphoreListener( const pybind::object & _cb, const pybind::args & _args )
        : m_cb( _cb )
        , m_args( _args )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    void PythonAndroidSemaphoreListener::invoke()
    {
        PythonAndroidSemaphoreListenerPtr keep = PythonAndroidSemaphoreListenerPtr::from(this);

        Helper::dispatchMainThreadEvent([keep]() {
            pybind::object cb = keep->m_cb;
            pybind::args args = keep->m_args;

            cb.call_args(args);
        });
    }
    //////////////////////////////////////////////////////////////////////////
}