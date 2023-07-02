#pragma once

#include "Interface/Interface.h"
#include "Interface/ServantInterface.h"
#include "Interface/ServiceInterface.h"
#include "Interface/FileGroupInterface.h"

#include "Kernel/ConstString.h"
#include "Kernel/FilePath.h"
#include "Kernel/FactorableUnique.h"
#include "Kernel/String.h"
#include "Kernel/VectorString.h"
#include "Kernel/HttpCode.h"

#include "Config/UniqueId.h"

#define MENGINE_HTTP_REQUEST_TIMEOUT_INFINITY (-1)
#define MENGINE_HTTP_REQUEST_INVALID INVALID_UNIQUE_ID

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    typedef UniqueId HttpRequestId;
    //////////////////////////////////////////////////////////////////////////
    typedef VectorString HttpRequestHeaders;
    //////////////////////////////////////////////////////////////////////////
    struct HttpRequestPostParam
    {
        String key;
        String value;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef Vector<HttpRequestPostParam> HttpRequestPostParams;
    //////////////////////////////////////////////////////////////////////////
    class HttpResponseInterface
        : public Interface
    {
    public:
        virtual HttpRequestId getRequestId() const = 0;
        virtual uint32_t getStatus() const = 0;
        virtual const HttpRequestHeaders & getHeaders() const = 0;
        virtual const String & getData() const = 0;
        virtual EHttpCode getCode() const = 0;
        virtual const String & getError() const = 0;
        virtual bool isSuccessful() const = 0;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<HttpResponseInterface> HttpResponseInterfacePtr;
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
    class HttpSystemInterface
        : public ServiceInterface
    {
        SERVICE_DECLARE( "HttpSystem" );

    public:
        virtual HttpRequestId getMessage( const String & _url, const HttpRequestHeaders & _headers, int32_t _timeout, bool _receiveHeaders, const HttpReceiverInterfacePtr & _receiver, const DocumentInterfacePtr & _doc ) = 0;
        virtual HttpRequestId postMessage( const String & _url, const HttpRequestHeaders & _headers, int32_t _timeout, bool _receiveHeaders, const HttpRequestPostParams & _params, const HttpReceiverInterfacePtr & _receiver, const DocumentInterfacePtr & _doc ) = 0;
        virtual HttpRequestId deleteMessage( const String & _url, const HttpRequestHeaders & _headers, int32_t _timeout, bool _receiveHeaders, const HttpReceiverInterfacePtr & _receiver, const DocumentInterfacePtr & _doc ) = 0;

    public:
        virtual HttpRequestId headerData( const String & _url, const HttpRequestHeaders & _headers, int32_t _timeout, bool _receiveHeaders, const String & _data, const HttpReceiverInterfacePtr & _receiver, const DocumentInterfacePtr & _doc ) = 0;

    public:
        virtual HttpRequestId downloadAsset( const String & _url, const String & _login, const String & _password, const FileGroupInterfacePtr & _fileGroup, const FilePath & _filePath, int32_t _timeout, const HttpReceiverInterfacePtr & _receiver, const DocumentInterfacePtr & _doc ) = 0;

    public:
        virtual bool cancelRequest( HttpRequestId _id ) = 0;
    };
    //////////////////////////////////////////////////////////////////////////
}
//////////////////////////////////////////////////////////////////////////
#define HTTP_SYSTEM()\
    ((Mengine::HttpSystemInterface *)SERVICE_GET(Mengine::HttpSystemInterface))
//////////////////////////////////////////////////////////////////////////
