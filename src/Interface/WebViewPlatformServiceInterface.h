#pragma once

#include "Interface/ServiceInterface.h"

#include "Kernel/Unknowable.h"

namespace Mengine
{
    class WebViewPlatformServiceInterface
        : public ServiceInterface
        , public Unknowable
    {
        SERVICE_DECLARE( "WebViewPlatformService" );
    };
}
//////////////////////////////////////////////////////////////////////////
#define WEBVIEW_PLATFORM_SERVICE()\
    ((Mengine::WebViewPlatformServiceInterface *)SERVICE_GET(Mengine::WebViewPlatformServiceInterface))
//////////////////////////////////////////////////////////////////////////
