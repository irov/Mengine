#pragma once

#include "Environment/Android/AndroidPluginCallbackInterface.h"
#include "Environment/Python/PythonIncluder.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class PythonAndroidPluginCallback
        : public AndroidPluginCallbackInterface
    {
    public:
        PythonAndroidPluginCallback( pybind::kernel_interface * _kernel, const pybind::object & _cb, const pybind::args & _args );
        ~PythonAndroidPluginCallback() override;

    protected:
        void invoke( JNIEnv * _jenv, jobjectArray _args ) override;

    protected:
        pybind::kernel_interface * m_kernel;

        pybind::object m_cb;
        pybind::args m_args;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<PythonAndroidPluginCallback, AndroidPluginCallbackInterface> PythonAndroidPluginCallbackPtr;
    //////////////////////////////////////////////////////////////////////////
}