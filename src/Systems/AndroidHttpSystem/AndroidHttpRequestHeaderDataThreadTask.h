#pragma once

#include "AndroidHttpRequestThreadTask.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class AndroidHttpRequestHeaderDataThreadTask
        : public AndroidHttpRequestThreadTask
    {
        DECLARE_FACTORABLE( AndroidHttpRequestHeaderDataThreadTask );

    public:
        AndroidHttpRequestHeaderDataThreadTask();
        ~AndroidHttpRequestHeaderDataThreadTask() override;

    public:
        bool initialize( const Data & _data );

    protected:
        jobject _onHttp( JNIEnv * _jenv, jobject _basses ) override;

    protected:
        Data m_data;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<AndroidHttpRequestHeaderDataThreadTask, ThreadTaskInterface> AndroidHttpRequestHeaderDataThreadTaskPtr;
    //////////////////////////////////////////////////////////////////////////
}