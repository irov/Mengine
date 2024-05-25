#pragma once

#include "AppleHttpRequestThreadTask.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class AppleHttpRequestHeaderDataThreadTask
        : public AppleHttpRequestThreadTask
    {
        DECLARE_FACTORABLE( AppleHttpRequestHeaderDataThreadTask );

    public:
        AppleHttpRequestHeaderDataThreadTask();
        ~AppleHttpRequestHeaderDataThreadTask() override;

    public:
        bool initialize( const Data & _data );

    protected:
        jobject _onHttp( JNIEnv * _jenv, jobject _basses ) override;

    protected:
        Data m_data;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<AppleHttpRequestHeaderDataThreadTask, ThreadTaskInterface> AppleHttpRequestHeaderDataThreadTaskPtr;
    //////////////////////////////////////////////////////////////////////////
}