#pragma once

#include "Interface/HttpResponseInterface.h"

#include "Kernel/Mixin.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class HttpReceiverInterface
        : public Mixin
    {
    public:
        virtual void onHttpRequestComplete( const HttpResponseInterfacePtr & _response ) = 0;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<HttpReceiverInterface> HttpReceiverInterfacePtr;
    //////////////////////////////////////////////////////////////////////////
}
