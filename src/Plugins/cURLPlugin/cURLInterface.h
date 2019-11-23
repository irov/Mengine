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
    class cURLTaskReceiverF
        : public cURLTaskReceiverInterface
    {
    public:
        MENGINE_INLINE cURLTaskReceiverF( const LambdaTaskReceiver & _f )
            : m_f( _f )
        {
        }

        MENGINE_INLINE ~cURLTaskReceiverF() override
        {
        }

    protected:
        MENGINE_INLINE void onResponse( const EngineSourcePtr & _source, uint32_t _status, const String & _error, const cURLHeaders & _headers, const String & _response, uint32_t _code, bool _successful ) override
        {
            m_f( _source, _status, _error, _headers, _response, _code, _successful );
        }

    protected:
        LambdaTaskReceiver m_f;
    };
    //////////////////////////////////////////////////////////////////////////
    class cURLSourceInterface
        : public ServantInterface
    {
    protected:
        virtual void addHttpGet( const String & _url, const cURLHeaders & _headers, int32_t _timeout, bool _receiveHeaders, const cURLTaskReceiverInterfacePtr & _lambda ) = 0;
        virtual void addHttpHeaderData( const String & _url, const cURLHeaders & _headers, int32_t _timeout, bool _receiveHeaders, const String & _data, const cURLTaskReceiverInterfacePtr & _receiver ) = 0;
        virtual void addHttpPost( const String & _url, const cURLHeaders & _headers, int32_t _timeout, bool _receiveHeaders, const String & _cookies, const cURLPostParams & _params, const cURLTaskReceiverInterfacePtr & _receiver ) = 0;

    public:
        MENGINE_INLINE void addHttpGet( const String & _url, const cURLHeaders & _headers, int32_t _timeout, bool _receiveHeaders, const LambdaTaskReceiver & _lambda )
        {
            cURLTaskReceiverInterfacePtr receiver = Helper::makeFactorableUnique<cURLTaskReceiverF>( _lambda );

            this->addHttpGet( _url, _headers, _timeout, _receiveHeaders, receiver );
        }

        MENGINE_INLINE void addHttpHeaderData( const String & _url, const cURLHeaders & _headers, int32_t _timeout, bool _receiveHeaders, const String & _data, const LambdaTaskReceiver & _lambda )
        {
            cURLTaskReceiverInterfacePtr receiver = Helper::makeFactorableUnique<cURLTaskReceiverF>( _lambda );

            this->addHttpHeaderData( _url, _headers, _timeout, _receiveHeaders, _data, receiver );
        }

        MENGINE_INLINE void addHttpPost( const String & _url, const cURLHeaders & _headers, int32_t _timeout, bool _receiveHeaders, const String & _cookies, const cURLPostParams & _params, const LambdaTaskReceiver & _lambda )
        {
            cURLTaskReceiverInterfacePtr receiver = Helper::makeFactorableUnique<cURLTaskReceiverF>( _lambda );

            this->addHttpPost( _url, _headers, _timeout, _receiveHeaders, _cookies, _params, receiver );
        }
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
        virtual HttpRequestID postMessage( const String & _url, const cURLHeaders & _headers, int32_t _timeout, bool _receiveHeaders, const String & _cookies, const cURLPostParams & _params, const cURLReceiverInterfacePtr & _receiver ) = 0;
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
