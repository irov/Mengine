#pragma once

#include "Interface/WebViewPlatformExtensionInterface.h"

#import <AppKit/AppKit.h>

@class WKWebView;

namespace Mengine
{
    class MacOSWebViewPlatformServiceExtensionInterface
        : public WebViewPlatformExtensionInterface
    {
    public:
        virtual bool createWindow( NSString * _title, bool _hidden ) = 0;
        virtual NSWindow * getNSWindow() const = 0;
        virtual WKWebView * getWKWebView() const = 0;
    };
}
