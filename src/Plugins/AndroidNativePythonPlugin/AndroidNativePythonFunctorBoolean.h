#pragma once

#include "AndroidNativePythonFunctor.h"

namespace Mengine
{
    ///////////////////////////////////////////////////////////////////////
    class AndroidNativePythonFunctorBoolean
        : public AndroidNativePythonFunctor
    {
        DECLARE_FACTORABLE( AndroidNativePythonFunctorBoolean );

    public:
        AndroidNativePythonFunctorBoolean();
        ~AndroidNativePythonFunctorBoolean() override;

    public:
        PyObject * call( PyObject * _args, PyObject * _kwds ) override;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<AndroidNativePythonFunctorBoolean, AndroidNativePythonFunctor> AndroidNativePythonFunctorBooleanPtr;
    //////////////////////////////////////////////////////////////////////////
}
