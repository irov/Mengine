#pragma once

#include "Interface/HttpResponseInterface.h"
#include "Interface/HttpReceiverInterface.h"
#include "Interface/ThreadTaskInterface.h"

#include "Kernel/Mixin.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class HttpRequestInterface
        : public Mixin
    {
    public:
        virtual void setURL( const String & _url ) = 0;
        virtual const String & getURL() const = 0;

        virtual void setProxy( const String & _proxy ) = 0;
        virtual const String & getProxy() const = 0;

        virtual void setCookies( const String & _cookies ) = 0;
        virtual const String & getCookies() const = 0;

        virtual void setHeaders( const HttpHeaders & _headers ) = 0;
        virtual const HttpHeaders & getHeaders() const = 0;

        virtual void setRequestId( HttpRequestId _id ) = 0;
        virtual HttpRequestId getRequestId() const = 0;

        virtual void setTimeout( int32_t _timeout ) = 0;
        virtual int32_t getTimeout() const = 0;

        virtual void setFlags( uint32_t _flags ) = 0;
        virtual uint32_t getFlags() const = 0;

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
