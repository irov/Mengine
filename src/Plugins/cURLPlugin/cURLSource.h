#pragma once

#include "cURLInterface.h"

#include "Tasks/EngineSource.h"

#include "Config/String.h"
#include "Config/VectorString.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<class cURLSource> cURLSourcePtr;
    //////////////////////////////////////////////////////////////////////////
    class cURLSource
        : public cURLSourceInterface
    {
    public:
        cURLSource();
        ~cURLSource();

    public:
        void setSource( const EngineSourcePtr & _source );
        const EngineSourcePtr & getSource() const;

    public:
        typedef Lambda<void( const cURLSourcePtr &, uint32_t, const String &, const String &, uint32_t, bool )> LambdacURLReceiver;

    public:
        void addHttpGet( const String & _url, const cURLHeaders & _headers, int32_t _timeout, bool _receiveHeaders, const LambdaTaskReceiver & _lambda, const DocumentPtr & _doc ) override;
        void addHttpHeaderData( const String & _url, const cURLHeaders & _headers, int32_t _timeout, bool _receiveHeaders, const String & _data, const LambdaTaskReceiver & _lambda, const DocumentPtr & _doc ) override;
        void addHttpPost( const String & _url, const cURLHeaders & _headers, int32_t _timeout, bool _receiveHeaders, const cURLPostParams & _params, const LambdaTaskReceiver & _lambda, const DocumentPtr & _doc ) override;

    protected:
        void addHttpGet_( const String & _url, const cURLHeaders & _headers, int32_t _timeout, bool _receiveHeaders, const cURLTaskReceiverInterfacePtr & _receiver );
        void addHttpHeaderData_( const String & _url, const cURLHeaders & _headers, int32_t _timeout, bool _receiveHeaders, const String & _data, const cURLTaskReceiverInterfacePtr & _receiver );
        void addHttpPost_( const String & _url, const cURLHeaders & _headers, int32_t _timeout, bool _receiveHeaders, const cURLPostParams & _params, const cURLTaskReceiverInterfacePtr & _receiver );

    protected:
        EngineSourcePtr m_source;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<cURLSource> cURLSourcePtr;
    //////////////////////////////////////////////////////////////////////////
}