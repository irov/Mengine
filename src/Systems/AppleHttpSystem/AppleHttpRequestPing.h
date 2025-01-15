#pragma once

#include "AppleHttpRequest.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class AppleHttpRequestPing
        : public AppleHttpRequest
    {
        DECLARE_FACTORABLE( AppleHttpRequestPing );

    public:
        AppleHttpRequestPing();
        ~AppleHttpRequestPing() override;

    protected:
        AppleHttpResponseParam * _onHttp( AppleHttpRequestParam * request ) override;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<AppleHttpRequestPing, HttpRequestInterface> AppleHttpRequestPingPtr;
    //////////////////////////////////////////////////////////////////////////
}
