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
        void call();
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<AndroidNativePythonFunctorVoid, AndroidNativePythonFunctor> AndroidNativePythonFunctorVoidPtr;
    //////////////////////////////////////////////////////////////////////////
}
