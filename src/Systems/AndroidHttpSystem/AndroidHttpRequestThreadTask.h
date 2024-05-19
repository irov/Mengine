#pragma once

#include "Interface/HttpSystemInterface.h"

#include "Environment/Android/AndroidIncluder.h"

#include "AndroidHttpResponse.h"

#include "Kernel/ThreadTask.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class AndroidHttpRequestThreadTask
        : public ThreadTask
    {
    public:
        AndroidHttpRequestThreadTask();
        ~AndroidHttpRequestThreadTask() override;

    public:
        void setURL( const String & _url );
        const String & getURL() const;

        void setProxy( const String & _proxy );
        const String & getProxy() const;

        void setCookies( const String & _cookies );
        const String & getCookies() const;

        void setHeaders( const HttpRequestHeaders & _headers );
        const HttpRequestHeaders & getHeaders() const;

        void setRequestId( HttpRequestId _id );
        HttpRequestId getRequestId() const;

        void setTimeout( int32_t _timeout );
        int32_t getTimeout() const;

        void setReceiveHeaders( bool _receiveHeaders );
        bool getReceiveHeaders() const;

        void setReponse( const HttpResponseInterfacePtr & _response );
        const HttpResponseInterfacePtr & getReponse() const;

        void setReceiver( const HttpReceiverInterfacePtr & _receiver );
        const HttpReceiverInterfacePtr & getReceiver() const;

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
    typedef IntrusivePtr<AndroidHttpRequestThreadTask, ThreadTaskInterface> AndroidHttpRequestThreadTaskPtr;
    //////////////////////////////////////////////////////////////////////////
}