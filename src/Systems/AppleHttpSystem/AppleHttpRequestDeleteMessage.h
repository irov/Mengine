#pragma once

#include "AppleHttpRequest.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class AppleHttpRequestDeleteMessage
        : public AppleHttpRequest
    {
        DECLARE_FACTORABLE( AppleHttpRequestDeleteMessage );

    public:
        AppleHttpRequestDeleteMessage();
        ~AppleHttpRequestDeleteMessage() override;

    protected:
        AppleHttpResponseParam * _onHttp( AppleHttpRequestParam * request ) override;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<AppleHttpRequestDeleteMessage, HttpRequestInterface> AppleHttpRequestDeleteMessagePtr;
    //////////////////////////////////////////////////////////////////////////
}
