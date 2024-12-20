#pragma once

#include "Interface/ServiceInterface.h"
#include "Interface/ContentInterface.h"

#include "Interface/HttpReceiverInterface.h"

#include "Kernel/HttpRequestFlags.h"
#include "Kernel/HttpLambdaPing.h"
#include "Kernel/Data.h"

namespace Mengine
{
    class HttpServiceInterface
        : public ServiceInterface
    {
        SERVICE_DECLARE( "HttpService" );

    public:
        virtual void setProxy( const String & _proxy ) = 0;
        virtual const String & getProxy() const = 0;

    public:
        virtual const HttpHeaders & getApplicationJSONHeaders() const = 0;

    public:
        virtual HttpRequestId ping( const String & _url, int32_t _timeout, const HttpLambdaPing & _lambda, const DocumentInterfacePtr & _doc ) = 0;

    public:
        virtual HttpRequestId getMessage( const String & _url, const HttpHeaders & _headers, int32_t _timeout, uint32_t _flags, const HttpReceiverInterfacePtr & _receiver, const DocumentInterfacePtr & _doc ) = 0;
        virtual HttpRequestId postMessage( const String & _url, const HttpHeaders & _headers, int32_t _timeout, uint32_t _flags, const HttpRequestPostProperties & _params, const HttpReceiverInterfacePtr & _receiver, const DocumentInterfacePtr & _doc ) = 0;
        virtual HttpRequestId deleteMessage( const String & _url, const HttpHeaders & _headers, int32_t _timeout, uint32_t _flags, const HttpReceiverInterfacePtr & _receiver, const DocumentInterfacePtr & _doc ) = 0;
        virtual HttpRequestId headerData( const String & _url, const HttpHeaders & _headers, const Data & _data, int32_t _timeout, uint32_t _flags, const HttpReceiverInterfacePtr & _receiver, const DocumentInterfacePtr & _doc ) = 0;
        virtual HttpRequestId getAsset( const String & _url, const String & _login, const String & _password, const ContentInterfacePtr & _content, int32_t _timeout, uint32_t _flags, const HttpReceiverInterfacePtr & _receiver, const DocumentInterfacePtr & _doc ) = 0;

    public:
        virtual bool cancelRequest( HttpRequestId _id ) = 0;
    };
}
//////////////////////////////////////////////////////////////////////////
#define HTTP_SERVICE()\
    ((Mengine::HttpServiceInterface *)SERVICE_GET(Mengine::HttpServiceInterface))
//////////////////////////////////////////////////////////////////////////
