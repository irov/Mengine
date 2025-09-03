#pragma once

#include "AndroidHttpRequest.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class AndroidHttpRequestHeaderData
        : public AndroidHttpRequest
    {
        DECLARE_FACTORABLE( AndroidHttpRequestHeaderData );

    public:
        AndroidHttpRequestHeaderData();
        ~AndroidHttpRequestHeaderData() override;

    public:
        void setData( const Data & _data );
        const Data & getData() const;

    protected:
        jobject _onHttp( JNIEnv * _jenv, jobject _basses ) override;

    protected:
        Data m_data;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<AndroidHttpRequestHeaderData, HttpRequestInterface> AndroidHttpRequestHeaderDataPtr;
    //////////////////////////////////////////////////////////////////////////
}