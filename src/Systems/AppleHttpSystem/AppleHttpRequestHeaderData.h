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
        bool initialize( const Data & _data );

    protected:
        jobject _onHttp( JNIEnv * _jenv, jobject _basses ) override;

    protected:
        Data m_data;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<AppleHttpRequestHeaderData, ThreadTaskInterface> AppleHttpRequestHeaderDataPtr;
    //////////////////////////////////////////////////////////////////////////
}