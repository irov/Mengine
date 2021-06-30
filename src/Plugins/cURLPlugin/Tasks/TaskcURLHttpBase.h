#pragma once

#include "Plugins/cURLPlugin/cURLInterface.h"

#include "GOAP/TaskInterface.h"
#include "GOAP/NodeInterface.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class cURLTaskReceiverInterface
        : public ServantInterface
    {
    public:
        virtual void onResponse( const GOAP::SourceInterfacePtr & _source, uint32_t _status, const String & _error, const cURLHeaders & _headers, const String & _response, uint32_t _code, bool _successful ) = 0;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<cURLTaskReceiverInterface> cURLTaskReceiverInterfacePtr;
    //////////////////////////////////////////////////////////////////////////
    class TaskcURLHttpBase
        : public GOAP::TaskInterface
    {
    public:
        TaskcURLHttpBase( GOAP::Allocator * _allocator, const String & _url, const cURLHeaders & _headers, int32_t _timeout, bool _receiveHeaders, const cURLTaskReceiverInterfacePtr & _receiver );
        ~TaskcURLHttpBase() override;

    protected:
        void _onFinally() override;

    protected:
        cURLReceiverInterfacePtr createHttpReceiver_( GOAP::NodeInterface * _node );

    protected:
        String m_url;
        cURLHeaders m_headers;

        int32_t m_timeout;
        bool m_receiveHeaders;

        cURLTaskReceiverInterfacePtr m_receiver;

        HttpRequestID m_requestId;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef GOAP::IntrusivePtr<TaskcURLHttpBase> TaskcURLHttpBasePtr;
    //////////////////////////////////////////////////////////////////////////
}
