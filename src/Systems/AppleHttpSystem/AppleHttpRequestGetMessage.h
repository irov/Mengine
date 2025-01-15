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
        AppleHttpResponseParam * _onHttp( AppleHttpRequestParam * request ) override;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<AppleHttpRequestGetMessage, HttpRequestInterface> AppleHttpRequestGetMessagePtr;
    //////////////////////////////////////////////////////////////////////////
}
