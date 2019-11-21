#pragma once

#include "cURLInterface.h"

#include "GOAP/Task.h"
#include "GOAP/NodeInterface.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class TaskcURLHttpBase
        : public GOAP::Task
    {
    public:
        TaskcURLHttpBase( const String & _url, const cURLHeaders & _headers, int32_t _timeout, bool _receiveHeaders, const cURLTaskReceiverInterfacePtr & _receiver );
        ~TaskcURLHttpBase() override;

    protected:
        void _onFinally() override;

    protected:
        cURLReceiverInterfacePtr createHttpReceiver_( GOAP::NodeInterface * _node );

    protected:
        void requestComplete_( HttpRequestID _requestId );

    protected:
        String m_url;
        cURLHeaders m_headers;

        int32_t m_timeout;
        bool m_receiveHeaders;

        cURLTaskReceiverInterfacePtr m_receiver;

        HttpRequestID m_requestId;

        class TaskcURLReceiver;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef GOAP::IntrusivePtr<TaskcURLHttpBase> TaskcURLHttpBasePtr;
    //////////////////////////////////////////////////////////////////////////
}
