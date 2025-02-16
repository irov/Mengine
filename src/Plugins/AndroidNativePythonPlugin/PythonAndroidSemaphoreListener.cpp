#include "PythonAndroidSemaphoreListener.h"

#include "Kernel/ThreadHelper.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    PythonAndroidSemaphoreListener::PythonAndroidSemaphoreListener( const pybind::object & _cb, const pybind::args & _args )
        : PythonCallbackProvider( _cb, _args )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    PythonAndroidSemaphoreListener::~PythonAndroidSemaphoreListener()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    void PythonAndroidSemaphoreListener::invoke()
    {
        PythonAndroidSemaphoreListenerPtr keep = PythonAndroidSemaphoreListenerPtr::from(this);

        Helper::dispatchMainThreadEvent([keep]() {
            keep->call_cb();
        });
    }
    //////////////////////////////////////////////////////////////////////////
}