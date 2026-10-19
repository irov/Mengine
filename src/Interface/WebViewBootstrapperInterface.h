#pragma once

#include "Interface/ServiceInterface.h"

namespace Mengine
{
    class WebViewBootstrapperInterface
        : public ServiceInterface
    {
        SERVICE_DECLARE( "WebViewBootstrapper" );

    public:
        virtual bool initialize() = 0;
        virtual bool run() = 0;
        virtual void tick( float _time ) = 0;
        virtual void stop() = 0;
        virtual void finalize() = 0;
    };
}
//////////////////////////////////////////////////////////////////////////
#define WEBVIEW_BOOTSTRAPPER_SERVICE()\
    ((Mengine::WebViewBootstrapperInterface *)SERVICE_GET(Mengine::WebViewBootstrapperInterface))
//////////////////////////////////////////////////////////////////////////
