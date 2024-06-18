#pragma once

#include "Interface/Interface.h"
#include "Interface/HttpResponseInterface.h"
#include "Interface/HttpReceiverInterface.h"
#include "Interface/ThreadTaskInterface.h"

#define MENGINE_HTTP_REQUEST_TIMEOUT_INFINITY (-1)

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class HttpRequestInterface
        : public Interface
    {
    public:
        virtual void setURL( const String & _url ) = 0;
        virtual const String & getURL() const = 0;

        virtual void setProxy( const String & _proxy ) = 0;
        virtual const String & getProxy() const = 0;

        virtual void setCookies( const String & _cookies ) = 0;
        virtual const String & getCookies() const = 0;

        virtual void setHeaders( const HttpRequestHeaders & _headers ) = 0;
        virtual const HttpRequestHeaders & getHeaders() const = 0;

        virtual void setRequestId( HttpRequestId _id ) = 0;
        virtual HttpRequestId getRequestId() const = 0;

        virtual void setTimeout( int32_t _timeout ) = 0;
        virtual int32_t getTimeout() const = 0;

        virtual void setReceiveHeaders( bool _receiveHeaders ) = 0;
        virtual bool getReceiveHeaders() const = 0;

        virtual void setReponse( const HttpResponseInterfacePtr & _response ) = 0;
        virtual const HttpResponseInterfacePtr & getReponse() const = 0;

        virtual void setReceiver( const HttpReceiverInterfacePtr & _receiver ) = 0;
        virtual const HttpReceiverInterfacePtr & getReceiver() const = 0;

    public:
        virtual ThreadTaskInterfacePtr getThreadTask() = 0;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<HttpRequestInterface> HttpRequestInterfacePtr;
    //////////////////////////////////////////////////////////////////////////
}
