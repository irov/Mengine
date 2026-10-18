#pragma once

#include "Kernel/Mixin.h"
#include "Kernel/JSON.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class WebViewBridgeSenderInterface
        : public Mixin
    {
    public:
        virtual bool onWebViewBridgeSenderSendJSON( const jpp::object & _message ) = 0;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<WebViewBridgeSenderInterface> WebViewBridgeSenderInterfacePtr;
    //////////////////////////////////////////////////////////////////////////
}
