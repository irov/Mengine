#pragma once

#include "Win32HttpRequest.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class Win32HttpRequestPing
        : public Win32HttpRequest
    {
        DECLARE_FACTORABLE( Win32HttpRequestPing );

    public:
        Win32HttpRequestPing();
        ~Win32HttpRequestPing() override;

    protected:
        bool _onHttp() override;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<Win32HttpRequestPing, HttpRequestInterface> Win32HttpRequestPingPtr;
    //////////////////////////////////////////////////////////////////////////
}
