#pragma once

#include "Win32HttpRequest.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class Win32HttpRequestDeleteMessage
        : public Win32HttpRequest
    {
        DECLARE_FACTORABLE( Win32HttpRequestDeleteMessage );

    public:
        Win32HttpRequestDeleteMessage();
        ~Win32HttpRequestDeleteMessage() override;

    protected:
        bool _onHttp() override;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<Win32HttpRequestDeleteMessage, HttpRequestInterface> Win32HttpRequestDeleteMessagePtr;
    //////////////////////////////////////////////////////////////////////////
}
