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
        TaskcURLHttpBase( const String & _url, int32_t _timeout, const cURLTaskReceiverInterfacePtr & _receiver );
        ~TaskcURLHttpBase() override;

    protected:
        void _onFinally() override;

    protected:
        cURLReceiverInterfacePtr createHttpReceiver_( GOAP::NodeInterface * _node );

    protected:
        void requestComplete_( HttpRequestID _requestId );

    protected:
        String m_url;
        int32_t m_timeout;

        cURLTaskReceiverInterfacePtr m_receiver;

        HttpRequestID m_requestId;

        class TaskcURLReceiver;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef GOAP::IntrusivePtr<TaskcURLHttpBase> TaskcURLHttpBasePtr;
    //////////////////////////////////////////////////////////////////////////
}
