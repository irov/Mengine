#pragma once

#include "Interface/MemoryInterface.h"
#include "Interface/WebViewPlatformExtensionInterface.h"

#include "Environment/Windows/WindowsIncluder.h"

namespace Mengine
{
    class Win32WebViewPlatformServiceExtensionInterface
        : public WebViewPlatformExtensionInterface
    {
    public:
        virtual bool attachWindow( HWND _window, const WChar * _profile, const MemoryInterfacePtr & _bridge ) = 0;
        virtual void resizeBrowser() = 0;
        virtual void retryBrowser() = 0;
    };
}
