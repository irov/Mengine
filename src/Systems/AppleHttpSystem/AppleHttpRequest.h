#pragma once

#include "Interface/HttpSystemInterface.h"

#include "Kernel/ThreadTask.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class AppleHttpRequest
        : public BaseHttpRequest
    {
    public:
        AppleHttpRequest();
        ~AppleHttpRequest() override;

    protected:
        void _onThreadTaskPreparation() override;
        bool _onThreadTaskRun() override;
        bool _onThreadTaskProcess() override;

    protected:
        virtual jobject _onHttp( JNIEnv * _jenv, jobject _basses ) = 0;

    protected:
        void _onThreadTaskComplete( bool _successful ) override;

    protected:
        ThreadMutexInterfacePtr m_mutex;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<AppleHttpRequest, HttpRequestInterface> AppleHttpRequestPtr;
    //////////////////////////////////////////////////////////////////////////
}