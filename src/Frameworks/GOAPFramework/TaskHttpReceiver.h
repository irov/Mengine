#pragma once

#include "Interface/HttpSystemInterface.h"

#include "TaskHttpBase.h"

#include "GOAP/NodeInterface.h"

#include "Kernel/Factorable.h"

#include "Config/Lambda.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class TaskHttpReceiver
        : public HttpReceiverInterface
        , public Factorable
    {
    public:
        TaskHttpReceiver();
        ~TaskHttpReceiver() override;

    public:
        typedef Lambda<void( const HttpResponseInterfacePtr & _response )> LambdaRequestId;

    public:
        void initialize( GOAP::NodeInterface * _node, const HttpTaskReceiverInterfacePtr & _receiver, const LambdaRequestId & _lambda );
        void finalize();

    protected:
        void onHttpRequestComplete( const HttpResponseInterfacePtr & _response ) override;

    protected:
        GOAP::NodeInterfacePtr m_node;
        HttpTaskReceiverInterfacePtr m_receiver;
        LambdaRequestId m_lambda;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<TaskHttpReceiver, HttpReceiverInterface> TaskHttpReceiverPtr;
    //////////////////////////////////////////////////////////////////////////
}