#pragma once

#include "Environment/Android/AndroidEnv.h"

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
        void setJavaFunctor( JNIEnv * _jenv, jobject _functor );
        MENGINE_NODISCARD jobject getJavaFunctor() const;

    public:
        void call( bool _result, const Params & _params );

    protected:
        jobject m_functor;

        bool m_called;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<AndroidNativePythonCallback> AndroidNativePythonCallbackPtr;
    //////////////////////////////////////////////////////////////////////////
}
