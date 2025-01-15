#pragma once

#include "Kernel/BaseHttpRequest.h"

#import "AppleHttpNetwork.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class AppleHttpRequest
        : public BaseHttpRequest
    {
    public:
        AppleHttpRequest( bool _isJSON );
        ~AppleHttpRequest() override;

    protected:
        void _onThreadTaskPreparation() override;
        bool _onThreadTaskRun() override;
        bool _onThreadTaskProcess() override;
        
    protected:
        virtual AppleHttpResponseParam * _onHttp( AppleHttpRequestParam * request ) = 0;

    protected:
        void _onThreadTaskComplete( bool _successful ) override;

    protected:
        bool m_isJSON;
        
        ThreadMutexInterfacePtr m_mutex;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<AppleHttpRequest, HttpRequestInterface> AppleHttpRequestPtr;
    //////////////////////////////////////////////////////////////////////////
}
