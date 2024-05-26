#pragma once

#include "cURLHttpRequest.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class cURLHttpRequestGetMessage
        : public cURLHttpRequest
    {
        DECLARE_FACTORABLE( cURLHttpRequestGetMessage );

    public:
        cURLHttpRequestGetMessage();
        ~cURLHttpRequestGetMessage() override;

    protected:
        void _onCURL( CURL * _curl ) override;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<cURLHttpRequestGetMessage, HttpRequestInterface> cURLHttpRequestGetMessagePtr;
    //////////////////////////////////////////////////////////////////////////
}