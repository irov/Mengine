#pragma once

#include "Environment/Android/AndroidIncluder.h"
#include "Environment/Python/PythonIncluder.h"

#include "Kernel/Factorable.h"

#include "Config/StdInt.h"

namespace Mengine
{
    ///////////////////////////////////////////////////////////////////////
    class AndroidNativePythonCallback
        : public Factorable
    {
        DECLARE_FACTORABLE( AndroidNativePythonCallback );

    public:
        AndroidNativePythonCallback();
        ~AndroidNativePythonCallback() override;

    public:
        void setResponseCb( jobject _responseId );
        jobject getResponseCb() const;

    public:
        void callback( const pybind::object & _result );

    protected:
        jobject m_responseCb;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<AndroidNativePythonCallback> AndroidNativePythonCallbackPtr;
    //////////////////////////////////////////////////////////////////////////
}
