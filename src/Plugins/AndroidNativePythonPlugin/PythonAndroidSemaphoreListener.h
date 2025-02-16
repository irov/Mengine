#pragma once

#include "Environment/Android/AndroidSemaphoreListenerInterface.h"
#include "Environment/Python/PythonCallbackProvider.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class PythonAndroidSemaphoreListener
        : public AndroidSemaphoreListenerInterface
        , public PythonCallbackProvider
    {
    public:
        PythonAndroidSemaphoreListener( const pybind::object & _cb, const pybind::args & _args );
        ~PythonAndroidSemaphoreListener() override;

    protected:
        void invoke() override;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<PythonAndroidSemaphoreListener, AndroidSemaphoreListenerInterface> PythonAndroidSemaphoreListenerPtr;
    //////////////////////////////////////////////////////////////////////////
}