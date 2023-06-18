#pragma once

#include "TaskHttpBase.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class TaskHttpDelete
        : public TaskHttpBase
    {
    public:
        TaskHttpDelete( GOAP::Allocator * _allocator, const String & _url, const HttpRequestHeaders & _headers, int32_t _timeout, bool _receiveHeaders, const HttpTaskReceiverInterfacePtr & _receiver );
        ~TaskHttpDelete() override;

    protected:
        bool _onRun( GOAP::NodeInterface * _node ) override;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef GOAP::IntrusivePtr<TaskHttpDelete> TaskHttpDeletePtr;
    //////////////////////////////////////////////////////////////////////////
}
