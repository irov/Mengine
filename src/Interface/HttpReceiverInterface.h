#pragma once

#include "Interface/Interface.h"
#include "Interface/HttpResponseInterface.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class HttpReceiverInterface
        : public Interface
    {
    public:
        virtual void onHttpRequestComplete( const HttpResponseInterfacePtr & _response ) = 0;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<HttpReceiverInterface> HttpReceiverInterfacePtr;
    //////////////////////////////////////////////////////////////////////////
}
