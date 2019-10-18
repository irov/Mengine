#pragma once

#include "cURLInterface.h"

#include "Config/String.h"
#include "Config/VectorString.h"

#include "GOAP/Source.h"

namespace Mengine
{
    class cURLSource
        : public cURLSourceInterface
    {
    public:
        cURLSource();
        ~cURLSource();

    public:
        void setSource( const GOAP::SourcePtr & _source );
        const GOAP::SourcePtr & getSource() const;

    public:
        typedef Lambda<void( const GOAP::SourcePtr &, uint32_t, const String &, const String &, uint32_t, bool )> LambdacURLReceiver;

    public:
        void addHttpGet( const String & _url, int32_t _timeout, const cURLTaskReceiverInterfacePtr & _receiver ) override;
        void addHttpHeaderData( const String & _url, const VectorString & _headers, const String & _data, int32_t _timeout, const cURLTaskReceiverInterfacePtr & _receiver ) override;
        void addHttpPost( const String & _url, const cURLPostParams & _params, int32_t _timeout, const cURLTaskReceiverInterfacePtr & _receiver ) override;

    protected:
        GOAP::SourcePtr m_source;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<cURLSource> cURLSourcePtr;
    //////////////////////////////////////////////////////////////////////////
}