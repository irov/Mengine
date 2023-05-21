#pragma once

#include "Environment/Android/AndroidIncluder.h"
#include "Environment/Python/PythonIncluder.h"

#include "Kernel/Factorable.h"
#include "Kernel/Scriptable.h"

#include "Config/StdInt.h"

namespace Mengine
{
    ///////////////////////////////////////////////////////////////////////
    class AndroidNativePythonRunnable
        : public Factorable
        , public Scriptable
    {
        DECLARE_FACTORABLE( AndroidNativePythonRunnable );

    public:
        AndroidNativePythonRunnable();
        ~AndroidNativePythonRunnable() override;

    public:
        void setResponseCb( jobject _responseId );
        jobject getResponseCb() const;

    public:
        void run();

    protected:
        jobject m_responseCb;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<AndroidNativePythonRunnable> AndroidNativePythonRunnablePtr;
    //////////////////////////////////////////////////////////////////////////
}
