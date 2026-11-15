#include "Win32NativePythonSemaphoreListener.h"

#include "Kernel/ThreadHelper.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    Win32NativePythonSemaphoreListener::Win32NativePythonSemaphoreListener( const pybind::object & _cb, const pybind::args & _args )
        : PythonCallbackProvider( _cb, _args )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    Win32NativePythonSemaphoreListener::~Win32NativePythonSemaphoreListener()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    void Win32NativePythonSemaphoreListener::invoke( const ParamVariant & _value )
    {
        Win32NativePythonSemaphoreListenerPtr keep = Win32NativePythonSemaphoreListenerPtr::from( this );
        ParamVariant value = _value;

        Helper::dispatchMainThreadEvent( [keep, value]() {
            keep->call_cb( value );
        } );
    }
    //////////////////////////////////////////////////////////////////////////
}
