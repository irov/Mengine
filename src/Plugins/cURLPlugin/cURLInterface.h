#pragma once

#include "Interface/Interface.h"
#include "Interface/ServantInterface.h"
#include "Interface/ServiceInterface.h"
#include "Interface/FileGroupInterface.h"

#include "Tasks/EngineSource.h"

#include "Kernel/ConstString.h"
#include "Kernel/FilePath.h"
#include "Kernel/FactorableUnique.h"

#include "Config/String.h"
#include "Config/VectorString.h"

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
    class cURLReceiverInterface
        : public Interface
    {
    public:
        virtual void onHttpRequestComplete( HttpRequestID _id, uint32_t _status, const String & _error, const cURLHeaders & _headers, const String & _response, uint32_t _code, bool _successful ) = 0;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<cURLReceiverInterface> cURLReceiverInterfacePtr;
    /////////////////////////////////////////////////////////////////////////
    /************************************************************************
    uint32_t _status, const String & _error, const String & _response, uint32_t _code, bool _successful
    ************************************************************************/
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<class cURLSourceInterface> cURLSourceInterfacePtr;
    //////////////////////////////////////////////////////////////////////////
    class cURLTaskReceiverInterface
        : public ServantInterface
    {
    public:
        virtual void onResponse( const EngineSourcePtr & _source, uint32_t _status, const String & _error, const cURLHeaders & _headers, const String & _response, uint32_t _code, bool _successful ) = 0;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<cURLTaskReceiverInterface> cURLTaskReceiverInterfacePtr;
    //////////////////////////////////////////////////////////////////////////
    typedef Lambda<void( const EngineSourcePtr & _source, uint32_t _status, const String & _error, const cURLHeaders & _headers, const String & _response, uint32_t _code, bool _successful )> LambdaTaskReceiver;
    //////////////////////////////////////////////////////////////////////////
    class cURLSourceInterface
        : public ServantInterface
    {
    public:
        virtual void addHttpGet( const String & _url, const cURLHeaders & _headers, int32_t _timeout, bool _receiveHeaders, const LambdaTaskReceiver & _lambda, const DocumentPtr & _doc ) = 0;
        virtual void addHttpHeaderData( const String & _url, const cURLHeaders & _headers, int32_t _timeout, bool _receiveHeaders, const String & _data, const LambdaTaskReceiver & _lambda, const DocumentPtr & _doc ) = 0;
        virtual void addHttpPost( const String & _url, const cURLHeaders & _headers, int32_t _timeout, bool _receiveHeaders, const cURLPostParams & _params, const LambdaTaskReceiver & _lambda, const DocumentPtr & _doc ) = 0;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<cURLSourceInterface> cURLSourceInterfacePtr;
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
        virtual cURLSourceInterfacePtr makeSource( const EngineSourcePtr & _source ) = 0;
    };
}
//////////////////////////////////////////////////////////////////////////
#define CURL_SERVICE()\
    ((Mengine::cURLServiceInterface *)SERVICE_GET(Mengine::cURLServiceInterface))
//////////////////////////////////////////////////////////////////////////
