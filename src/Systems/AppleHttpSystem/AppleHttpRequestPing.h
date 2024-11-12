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
        MengineHttpResponseParam * _onHttp( MengineHttpRequestParam * request ) override;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<AppleHttpRequestPing, HttpRequestInterface> AppleHttpRequestPingPtr;
    //////////////////////////////////////////////////////////////////////////
}
