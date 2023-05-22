#pragma once

#include "AndroidNativePythonFunctor.h"

namespace Mengine
{
    ///////////////////////////////////////////////////////////////////////
    class AndroidNativePythonFunctorVoid
        : public AndroidNativePythonFunctor
    {
        DECLARE_FACTORABLE( AndroidNativePythonFunctorVoid );

    public:
        AndroidNativePythonFunctorVoid();
        ~AndroidNativePythonFunctorVoid() override;

    public:
        PyObject * call( PyObject * _args, PyObject * _kwds ) override;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<AndroidNativePythonFunctorVoid, AndroidNativePythonFunctor> AndroidNativePythonFunctorVoidPtr;
    //////////////////////////////////////////////////////////////////////////
}
