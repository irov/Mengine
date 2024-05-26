#pragma once

#include "Environment/Android/AndroidIncluder.h"

#include "Kernel/BaseHttpRequest.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class AndroidHttpRequest
        : public BaseHttpRequest
    {
    public:
        AndroidHttpRequest();
        ~AndroidHttpRequest() override;

    protected:
        void _onThreadTaskPreparation() override;
        bool _onThreadTaskRun() override;
        bool _onThreadTaskProcess() override;

    protected:
        virtual jobject _onHttp( JNIEnv * _jenv, jobject _basses ) = 0;

    protected:
        void _onThreadTaskComplete( bool _successful ) override;

    protected:
        HttpRequestId m_id;

        String m_url;
        String m_proxy;
        String m_cookies;
        HttpRequestHeaders m_headers;
        int32_t m_timeout;
        bool m_receiveHeaders;

        HttpResponseInterfacePtr m_response;
        HttpReceiverInterfacePtr m_receiver;

        ThreadMutexInterfacePtr m_mutex;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<AndroidHttpRequest, ThreadTaskInterface> AndroidHttpRequestPtr;
    //////////////////////////////////////////////////////////////////////////
}