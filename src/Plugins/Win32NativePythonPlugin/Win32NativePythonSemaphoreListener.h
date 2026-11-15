#pragma once

#include "Interface/SemaphoreListenerInterface.h"

#include "Environment/Python/PythonCallbackProvider.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class Win32NativePythonSemaphoreListener
        : public SemaphoreListenerInterface
        , public PythonCallbackProvider
    {
        DECLARE_FACTORABLE( Win32NativePythonSemaphoreListener );

    public:
        Win32NativePythonSemaphoreListener( const pybind::object & _cb, const pybind::args & _args );
        ~Win32NativePythonSemaphoreListener() override;

    protected:
        void invoke( const ParamVariant & _value ) override;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<Win32NativePythonSemaphoreListener> Win32NativePythonSemaphoreListenerPtr;
    //////////////////////////////////////////////////////////////////////////
}
