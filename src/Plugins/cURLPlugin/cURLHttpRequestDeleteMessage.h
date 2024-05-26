#pragma once

#include "cURLHttpRequest.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class cURLHttpRequestDeleteMessage
        : public cURLHttpRequest
    {
        DECLARE_FACTORABLE( cURLHttpRequestDeleteMessage );

    public:
        cURLHttpRequestDeleteMessage();
        ~cURLHttpRequestDeleteMessage() override;

    protected:
        void _onCURL( CURL * _curl ) override;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<cURLHttpRequestDeleteMessage, HttpRequestInterface> cURLHttpRequestDeleteMessagePtr;
    //////////////////////////////////////////////////////////////////////////
}