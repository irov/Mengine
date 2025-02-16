#pragma once

#include "Environment/Android/AndroidIncluder.h"
#include "Environment/Python/PythonIncluder.h"

#include "Kernel/Factorable.h"
#include "Kernel/Scriptable.h"
#include "Kernel/Params.h"

namespace Mengine
{
    ///////////////////////////////////////////////////////////////////////
    class AndroidNativePythonCallback
        : public Factorable
        , public Scriptable
    {
        DECLARE_FACTORABLE( AndroidNativePythonCallback );

    public:
        AndroidNativePythonCallback();
        ~AndroidNativePythonCallback() override;

    public:
        void setKernel( pybind::kernel_interface * _kernel );
        pybind::kernel_interface * getKernel() const;

    public:
        void setJavaFunctor( JNIEnv * _jenv, jobject _functor );
        MENGINE_NODISCARD jobject getJavaFunctor() const;

    public:
        void call( bool _result, const Params & _params );

    protected:
        pybind::kernel_interface * m_kernel;
        jobject m_functor;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<AndroidNativePythonCallback> AndroidNativePythonCallbackPtr;
    //////////////////////////////////////////////////////////////////////////
}
