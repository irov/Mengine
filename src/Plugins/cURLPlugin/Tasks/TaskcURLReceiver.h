#pragma once

#include "Plugins/cURLPlugin/cURLInterface.h"

#include "TaskcURLHttpBase.h"

#include "GOAP/NodeInterface.h"

#include "Kernel/Factorable.h"

#include "Config/Lambda.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class TaskcURLReceiver
        : public cURLReceiverInterface
        , public Factorable
    {
    public:
        TaskcURLReceiver();
        ~TaskcURLReceiver() override;

    public:
        typedef Lambda<void( HttpRequestID _requestId )> LambdaRequestId;

    public:
        void initialize( GOAP::NodeInterface * _node, const cURLTaskReceiverInterfacePtr & _receiver, const LambdaRequestId & _lambda );
        void finalize();

    protected:
        void onHttpRequestComplete( HttpRequestID _id, uint32_t _status, const String & _error, const cURLHeaders & _headers, const String & _response, uint32_t _code, bool _successful ) override;

    protected:
        GOAP::NodeInterfacePtr m_node;
        cURLTaskReceiverInterfacePtr m_receiver;
        LambdaRequestId m_lambda;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<TaskcURLReceiver, cURLReceiverInterface> TaskcURLReceiverPtr;
    //////////////////////////////////////////////////////////////////////////
}