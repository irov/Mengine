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
        void call( bool _result );
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<AndroidNativePythonFunctorBoolean, AndroidNativePythonFunctor> AndroidNativePythonFunctorBooleanPtr;
    //////////////////////////////////////////////////////////////////////////
}
