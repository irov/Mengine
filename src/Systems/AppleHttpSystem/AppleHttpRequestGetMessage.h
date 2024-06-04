#pragma once

#include "AppleHttpRequest.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class AppleHttpRequestGetMessage
        : public AppleHttpRequest
    {
        DECLARE_FACTORABLE( AppleHttpRequestGetMessage );

    public:
        AppleHttpRequestGetMessage();
        ~AppleHttpRequestGetMessage() override;

    protected:
        MengineHttpResponseParam * _onHttp( MengineHttpRequestParam * request ) override;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<AppleHttpRequestGetMessage, HttpRequestInterface> AppleHttpRequestGetMessagePtr;
    //////////////////////////////////////////////////////////////////////////
}
