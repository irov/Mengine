#pragma once

#include "Environment/Android/AndroidIncluder.h"
#include "Environment/Python/PythonIncluder.h"

#include "Kernel/Factorable.h"
#include "Kernel/Scriptable.h"

#include "Config/StdInt.h"

namespace Mengine
{
    ///////////////////////////////////////////////////////////////////////
    class AndroidNativePythonFunctor
        : public Factorable
        , public Scriptable
    {
        DECLARE_FACTORABLE( AndroidNativePythonFunctor );

    public:
        AndroidNativePythonFunctor();
        ~AndroidNativePythonFunctor() override;

    public:
        void setKernel( pybind::kernel_interface * _kernel );
        pybind::kernel_interface * getKernel() const;

    public:
        void setJavaFunctor( jobject _functor );
        jobject getJavaFunctor() const;

    protected:
        pybind::kernel_interface * m_kernel;
        jobject m_functor;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<AndroidNativePythonFunctor> AndroidNativePythonFunctorPtr;
    //////////////////////////////////////////////////////////////////////////
}
