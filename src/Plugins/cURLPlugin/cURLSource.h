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
        void addHttpGet( const String & _url, int32_t _timeout, const cURLTaskReceiverInterfacePtr & _receiver ) override;
        void addHttpHeaderData( const String & _url, const VectorString & _headers, const String & _data, int32_t _timeout, const cURLTaskReceiverInterfacePtr & _receiver ) override;
        void addHttpPost( const String & _url, const cURLPostParams & _params, int32_t _timeout, const cURLTaskReceiverInterfacePtr & _receiver ) override;

    protected:
        EngineSourcePtr m_source;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<cURLSource> cURLSourcePtr;
    //////////////////////////////////////////////////////////////////////////
}