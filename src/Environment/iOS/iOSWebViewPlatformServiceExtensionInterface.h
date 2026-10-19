#pragma once

#include "Interface/WebViewPlatformExtensionInterface.h"

#import <UIKit/UIKit.h>

@class WKWebView;

namespace Mengine
{
    class iOSWebViewPlatformServiceExtensionInterface
        : public WebViewPlatformExtensionInterface
    {
    public:
        virtual bool attachScene( UIWindowScene * _scene ) = 0;
        virtual UIWindow * getUIWindow() const = 0;
        virtual WKWebView * getWKWebView() const = 0;
    };
}
