#pragma once

#include "Interface/HttpSystemInterface.h"

#include "GOAP/TaskInterface.h"
#include "GOAP/NodeInterface.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class HttpTaskReceiverInterface
        : public ServantInterface
    {
    public:
        virtual void onResponse( const GOAP::SourceInterfacePtr & _source, const HttpResponseInterfacePtr & _response ) = 0;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<HttpTaskReceiverInterface> HttpTaskReceiverInterfacePtr;
    //////////////////////////////////////////////////////////////////////////
    class TaskHttpBase
        : public GOAP::TaskInterface
    {
    public:
        TaskHttpBase( GOAP::Allocator * _allocator, const String & _url, const HttpRequestHeaders & _headers, int32_t _timeout, bool _receiveHeaders, const HttpTaskReceiverInterfacePtr & _receiver );
        ~TaskHttpBase() override;

    protected:
        void _onFinally() override;

    protected:
        HttpReceiverInterfacePtr createHttpReceiver_( GOAP::NodeInterface * _node );

    protected:
        String m_url;
        HttpRequestHeaders m_headers;

        int32_t m_timeout;
        bool m_receiveHeaders;

        HttpTaskReceiverInterfacePtr m_receiver;

        HttpRequestId m_requestId;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef GOAP::IntrusivePtr<TaskHttpBase> TaskHttpBasePtr;
    //////////////////////////////////////////////////////////////////////////
}
