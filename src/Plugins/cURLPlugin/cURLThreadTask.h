    #pragma once

#include "cURLInterface.h"

#include "cURLResponse.h"

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

        void setReponse( const cURLResponsePtr & _response );
        const cURLResponseInterfacePtr & getReponse() const;

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
        void writeResponse( const Char * _ptr, size_t _size );
        void writeHeader( const Char * _ptr, size_t _size );

    protected:
        HttpRequestID m_id;

        String m_url;
        String m_cookies;
        cURLHeaders m_headers;        
        int32_t m_timeout;
        bool m_receiveHeaders;

        cURLResponsePtr m_response;
        cURLReceiverInterfacePtr m_receiver;

    protected:
        struct curl_slist * m_curl_header_list;        
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<cURLThreadTask, ThreadTaskInterface> cURLThreadTaskPtr;
    //////////////////////////////////////////////////////////////////////////
}