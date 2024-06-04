#pragma once

#include "Win32HttpRequest.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class Win32HttpRequestGetMessage
        : public Win32HttpRequest
    {
        DECLARE_FACTORABLE( Win32HttpRequestGetMessage );

    public:
        Win32HttpRequestGetMessage();
        ~Win32HttpRequestGetMessage() override;

    protected:
        bool _onHttp() override;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<Win32HttpRequestGetMessage, HttpRequestInterface> Win32HttpRequestGetMessagePtr;
    //////////////////////////////////////////////////////////////////////////
}
