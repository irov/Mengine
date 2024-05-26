#pragma once

#include "cURLHttpRequest.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class cURLHttpRequestHeaderData
        : public cURLHttpRequest
    {
        DECLARE_FACTORABLE( cURLHttpRequestHeaderData );

    public:
        cURLHttpRequestHeaderData();
        ~cURLHttpRequestHeaderData() override;

    public:
        void setData( const Data & _data );
        const Data & getData() const;

    protected:
        void _onCURL( CURL * _curl ) override;
        void _onCURLCleanup( CURL * _curl ) override;

    protected:
        Data m_data;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<cURLHttpRequestHeaderData, HttpRequestInterface> cURLHttpRequestHeaderDataPtr;
    //////////////////////////////////////////////////////////////////////////
}