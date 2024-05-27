#pragma once

#include "Kernel/BaseHttpRequest.h"

#import "Environment/Apple/AppleHttpNetwork.h"

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
        virtual MengineHttpResponseParam * _onHttp( MengineHttpRequestParam * request ) = 0;

    protected:
        void _onThreadTaskComplete( bool _successful ) override;

    protected:
        ThreadMutexInterfacePtr m_mutex;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<AppleHttpRequest, HttpRequestInterface> AppleHttpRequestPtr;
    //////////////////////////////////////////////////////////////////////////
}
