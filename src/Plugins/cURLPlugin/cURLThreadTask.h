#pragma once

#include "cURLInterface.h"

#include "Kernel/ThreadTask.h"

#include "curl/curl.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class cURLThreadTask
        : public ThreadTask
    {
    public:
        cURLThreadTask();
        ~cURLThreadTask() override;

    public:
        void setURL( const String & _url );
        const String & getURL() const;

        void setCookies( const String & _cookies );
        const String & getCookies() const;

        void setHeaders( const cURLHeaders & _headers );
        const cURLHeaders & getHeaders() const;

        void setRequestId( HttpRequestID _id );
        HttpRequestID getRequestId() const;

        void setTimeout( int32_t _timeout );
        int32_t getTimeout() const;

        void setReceiveHeaders( bool _receiveHeaders );
        bool getReceiveHeaders() const;

        void setReceiver( const cURLReceiverInterfacePtr & _receiver );
        const cURLReceiverInterfacePtr & getReceiver() const;

    protected:
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
        void writeResponse( Char * const _ptr, size_t _size );
        void writeHeader( Char * const _ptr, size_t _size );

    protected:
        String m_url;
        String m_cookies;
        cURLHeaders m_headers;
        HttpRequestID m_id;
        int32_t m_timeout;
        bool m_receiveHeaders;
        cURLReceiverInterfacePtr m_receiver;

        struct curl_slist * m_curl_header_list;

        uint32_t m_responseCode;
        CURLcode m_responseStatus;
        String m_responseError;
        String m_responseData;
        cURLHeaders m_responseHeaders;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<cURLThreadTask> cURLThreadTaskPtr;
    //////////////////////////////////////////////////////////////////////////
}