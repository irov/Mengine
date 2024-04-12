#pragma once

#include "Interface/HttpSystemInterface.h"

#include "cURLHttpResponse.h"

#include "Kernel/ThreadTask.h"

#include "curl/curl.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class cURLHttpRequestThreadTask
        : public ThreadTask
    {
    public:
        cURLHttpRequestThreadTask();
        ~cURLHttpRequestThreadTask() override;

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

        void setReponse( const cURLHttpResponsePtr & _response );
        const HttpResponseInterfacePtr & getReponse() const;

        void setReceiver( const HttpReceiverInterfacePtr & _receiver );
        const HttpReceiverInterfacePtr & getReceiver() const;

    protected:
        void _onThreadTaskPreparation() override;
        bool _onThreadTaskRun() override;
        bool _onThreadTaskProcess() override;

    protected:
        virtual void _onCURL( CURL * _curl ) = 0;
        virtual void _onCURLCleanup( CURL * _curl );

    protected:
        void setupWriteResponse( CURL * _curl );

    protected:
        void _onThreadTaskComplete( bool _successful ) override;

    public:
        void writeResponse( const Char * _ptr, size_t _size );
        void writeHeader( const Char * _ptr, size_t _size );

    protected:
        HttpRequestId m_id;

        String m_url;
        String m_proxy;
        String m_cookies;
        HttpRequestHeaders m_headers;        
        int32_t m_timeout;
        bool m_receiveHeaders;

        cURLHttpResponsePtr m_response;
        HttpReceiverInterfacePtr m_receiver;

        ThreadMutexInterfacePtr m_mutex;

    protected:
        struct curl_slist * m_curl_header_list;        
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<cURLHttpRequestThreadTask, ThreadTaskInterface> cURLHttpRequestThreadTaskPtr;
    //////////////////////////////////////////////////////////////////////////
}