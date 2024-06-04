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
        MengineHttpResponseParam * _onHttp( MengineHttpRequestParam * request ) override;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<AppleHttpRequestDeleteMessage, HttpRequestInterface> AppleHttpRequestDeleteMessagePtr;
    //////////////////////////////////////////////////////////////////////////
}
