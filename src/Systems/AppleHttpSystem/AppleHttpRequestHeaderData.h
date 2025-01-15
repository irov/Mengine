#pragma once

#include "AppleHttpRequest.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class AppleHttpRequestHeaderData
        : public AppleHttpRequest
    {
        DECLARE_FACTORABLE( AppleHttpRequestHeaderData );

    public:
        AppleHttpRequestHeaderData();
        ~AppleHttpRequestHeaderData() override;

    public:
        void setData( const Data & _data );
        const Data & getData() const;

    protected:
        AppleHttpResponseParam * _onHttp( AppleHttpRequestParam * request ) override;

    protected:
        Data m_data;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<AppleHttpRequestHeaderData, ThreadTaskInterface> AppleHttpRequestHeaderDataPtr;
    //////////////////////////////////////////////////////////////////////////
}
