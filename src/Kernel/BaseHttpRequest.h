#pragma once

#include "Interface/HttpRequestInterface.h"
#include "Interface/HttpResponseInterface.h"

#include "Kernel/ThreadTask.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class BaseHttpRequest
        : public HttpRequestInterface
        , public ThreadTask
    {
    public:
        BaseHttpRequest();
        ~BaseHttpRequest() override;

    public:
        void setURL( const String & _url ) override;
        const String & getURL() const override;

        void setProxy( const String & _proxy ) override;
        const String & getProxy() const override;

        void setCookies( const String & _cookies ) override;
        const String & getCookies() const override;

        void setHeaders( const HttpHeaders & _headers ) override;
        const HttpHeaders & getHeaders() const override;

        void setRequestId( HttpRequestId _id ) override;
        HttpRequestId getRequestId() const override;

        void setTimeout( int32_t _timeout ) override;
        int32_t getTimeout() const override;

        void setFlags( uint32_t _flags ) override;
        uint32_t getFlags() const override;

        void setReponse( const HttpResponseInterfacePtr & _response ) override;
        const HttpResponseInterfacePtr & getReponse() const override;

        void setReceiver( const HttpReceiverInterfacePtr & _receiver ) override;
        const HttpReceiverInterfacePtr & getReceiver() const override;

    public:
        ThreadTaskInterfacePtr getThreadTask() override;

    protected:
        HttpRequestId m_id;

        String m_url;
        String m_proxy;
        String m_cookies;
        HttpHeaders m_headers;
        int32_t m_timeout;
        uint32_t m_flags;

        HttpResponseInterfacePtr m_response;
        HttpReceiverInterfacePtr m_receiver;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<BaseHttpRequest, HttpRequestInterface> BaseHttpRequestPtr;
    //////////////////////////////////////////////////////////////////////////
}