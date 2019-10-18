#pragma once

#include "Interface/Interface.h"
#include "Interface/ServantInterface.h"
#include "Interface/ServiceInterface.h"
#include "Interface/FileGroupInterface.h"

#include "Kernel/ConstString.h"
#include "Kernel/FilePath.h"
#include "Kernel/FactorableUnique.h"

#include "Config/String.h"
#include "Config/VectorString.h"

#include "GOAP/Source.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    typedef uint32_t HttpRequestID;
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
        virtual void onHttpRequestComplete( HttpRequestID _id, uint32_t _status, const String & _error, const String & _response, uint32_t _code, bool _successful ) = 0;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<cURLReceiverInterface> cURLReceiverInterfacePtr;
    /////////////////////////////////////////////////////////////////////////
    /************************************************************************
    uint32_t _status, const String & _error, const String & _response, uint32_t _code, bool _successful
    ************************************************************************/
    //////////////////////////////////////////////////////////////////////////
    class cURLTaskReceiverInterface
        : public ServantInterface
    {
    public:
        virtual void onResponse( const GOAP::SourcePtr & _source, uint32_t _status, const String & _error, const String & _response, uint32_t _code, bool _successful ) = 0;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<cURLTaskReceiverInterface> cURLTaskReceiverInterfacePtr;
    //////////////////////////////////////////////////////////////////////////
    typedef Lambda<void( const GOAP::SourcePtr &, uint32_t, const String &, const String &, uint32_t, bool )> LambdaTaskReceiver;
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
        MENGINE_INLINE void onResponse( const GOAP::SourcePtr & _source, uint32_t _status, const String & _error, const String & _response, uint32_t _code, bool _successful ) override
        {
            m_f( _source, _status, _error, _response, _code, _successful );
        }

    protected:
        LambdaTaskReceiver m_f;
    };
    //////////////////////////////////////////////////////////////////////////
    class cURLSourceInterface
        : public ServantInterface
    {
    protected:
        virtual void addHttpGet( const String & _url, int32_t _timeout, const cURLTaskReceiverInterfacePtr & _receiver ) = 0;
        virtual void addHttpHeaderData( const String & _url, const VectorString & _headers, const String & _data, int32_t _timeout, const cURLTaskReceiverInterfacePtr & _receiver ) = 0;
        virtual void addHttpPost( const String & _url, const cURLPostParams & _params, int32_t _timeout, const cURLTaskReceiverInterfacePtr & _receiver ) = 0;

    public:
        MENGINE_INLINE void addHttpGet( const String & _url, int32_t _timeout, const LambdaTaskReceiver & _receiver )
        {
            this->addHttpGet( _url, _timeout, Helper::makeFactorableUnique<cURLTaskReceiverF>( _receiver ) );
        }

        MENGINE_INLINE void addHttpHeaderData( const String & _url, const VectorString & _headers, const String & _data, int32_t _timeout, const LambdaTaskReceiver & _receiver )
        {
            this->addHttpHeaderData( _url, _headers, _data, _timeout, Helper::makeFactorableUnique<cURLTaskReceiverF>( _receiver ) );
        }

        MENGINE_INLINE void addHttpPost( const String & _url, const cURLPostParams & _params, int32_t _timeout, const LambdaTaskReceiver & _receiver )
        {
            this->addHttpPost( _url, _params, _timeout, Helper::makeFactorableUnique<cURLTaskReceiverF>( _receiver ) );
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
        virtual HttpRequestID getMessage( const String & _url, int32_t _timeout, const cURLReceiverInterfacePtr & _receiver ) = 0;
        virtual HttpRequestID postMessage( const String & _url, const cURLPostParams & _params, int32_t _timeout, const cURLReceiverInterfacePtr & _receiver ) = 0;
        virtual HttpRequestID headerData( const String & _url, const VectorString & _headers, const String & _data, int32_t _timeout, const cURLReceiverInterfacePtr & _receiver ) = 0;

    public:
        virtual HttpRequestID downloadAsset( const String & _url, const String & _login, const String & _password, const FileGroupInterfacePtr & _fileGroup, const FilePath & _filePath, int32_t _timeout, const cURLReceiverInterfacePtr & _receiver ) = 0;

    public:
        virtual bool cancelRequest( HttpRequestID _id ) = 0;

    public:
        virtual cURLSourceInterfacePtr makeSource( const GOAP::SourcePtr & _source ) = 0;
    };
}
//////////////////////////////////////////////////////////////////////////
#define CURL_SERVICE()\
    ((Mengine::cURLServiceInterface *)SERVICE_GET(Mengine::cURLServiceInterface))
//////////////////////////////////////////////////////////////////////////
