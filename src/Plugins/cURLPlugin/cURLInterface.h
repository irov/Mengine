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

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    typedef uint32_t HttpRequestID;
    //////////////////////////////////////////////////////////////////////////
    typedef VectorString cURLHeaders;
    //////////////////////////////////////////////////////////////////////////
    struct cURLPostParam
    {
        String key;
        String value;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef Vector<cURLPostParam> cURLPostParams;
    //////////////////////////////////////////////////////////////////////////
    struct cURLResponseData
    {
        HttpRequestID id;
        uint32_t status;
        cURLHeaders headers;
        String data;
        uint32_t code;
        String error;
        bool successful;
    };
    //////////////////////////////////////////////////////////////////////////
    class cURLReceiverInterface
        : public Interface
    {
    public:
        virtual void onHttpRequestComplete( const cURLResponseData & _response ) = 0;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<cURLReceiverInterface> cURLReceiverInterfacePtr;
    //////////////////////////////////////////////////////////////////////////
    class cURLRequestListenerInterface
        : public Interface
    {
    public:
        virtual void request( HttpRequestID  _id, const String & _url ) = 0;
        virtual void response( HttpRequestID  _id, const String & _url ) = 0;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<cURLRequestListenerInterface> cURLRequestListenerInterfacePtr;
    //////////////////////////////////////////////////////////////////////////
    class cURLServiceInterface
        : public ServiceInterface
    {
        SERVICE_DECLARE( "cURLService" );

    public:
        virtual HttpRequestID getMessage( const String & _url, const cURLHeaders & _headers, int32_t _timeout, bool _receiveHeaders, const cURLReceiverInterfacePtr & _receiver ) = 0;
        virtual HttpRequestID postMessage( const String & _url, const cURLHeaders & _headers, int32_t _timeout, bool _receiveHeaders, const cURLPostParams & _params, const cURLReceiverInterfacePtr & _receiver ) = 0;
        virtual HttpRequestID headerData( const String & _url, const cURLHeaders & _headers, int32_t _timeout, bool _receiveHeaders, const String & _data, const cURLReceiverInterfacePtr & _receiver ) = 0;

    public:
        virtual HttpRequestID downloadAsset( const String & _url, const String & _login, const String & _password, const FileGroupInterfacePtr & _fileGroup, const FilePath & _filePath, int32_t _timeout, const cURLReceiverInterfacePtr & _receiver ) = 0;

    public:
        virtual bool cancelRequest( HttpRequestID _id ) = 0;
        
    public:
        virtual int32_t addRequestListener( const cURLRequestListenerInterfacePtr & _listener, const DocumentPtr & _doc ) = 0;
        virtual void removeRequestListener( int32_t id ) = 0;
    };
    //////////////////////////////////////////////////////////////////////////
}
//////////////////////////////////////////////////////////////////////////
#define CURL_SERVICE()\
    ((Mengine::cURLServiceInterface *)SERVICE_GET(Mengine::cURLServiceInterface))
//////////////////////////////////////////////////////////////////////////
