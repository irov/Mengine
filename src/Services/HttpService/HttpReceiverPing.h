#pragma once

#include "Interface/HttpReceiverInterface.h"

#include "Kernel/Factorable.h"
#include "Kernel/HttpLambdaPing.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class HttpReceiverPing
        : public HttpReceiverInterface
        , public Factorable
    {
        DECLARE_FACTORABLE( HttpReceiverPing );

    public:
        HttpReceiverPing();
        ~HttpReceiverPing() override;

    public:
        void setLambda( const HttpLambdaPing & _lambda );
        const HttpLambdaPing & getLambda() const;

    protected:
        void onHttpRequestComplete( const HttpResponseInterfacePtr & _response ) override;

    protected:
        HttpLambdaPing m_lambda;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<HttpReceiverPing, HttpReceiverInterface> HttpReceiverPingPtr;
    //////////////////////////////////////////////////////////////////////////
}