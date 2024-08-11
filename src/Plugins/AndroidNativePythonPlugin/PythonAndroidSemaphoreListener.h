#pragma once

#include "Environment/Android/AndroidSemaphoreListenerInterface.h"
#include "Environment/Python/PythonIncluder.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class PythonAndroidSemaphoreListener
            : public AndroidSemaphoreListenerInterface
    {
    public:
        PythonAndroidSemaphoreListener( const pybind::object & _cb, const pybind::args & _args );

    protected:
        void invoke() override;

    protected:
        pybind::object m_cb;
        pybind::args m_args;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<PythonAndroidSemaphoreListener, AndroidSemaphoreListenerInterface> PythonAndroidSemaphoreListenerPtr;
    //////////////////////////////////////////////////////////////////////////
}