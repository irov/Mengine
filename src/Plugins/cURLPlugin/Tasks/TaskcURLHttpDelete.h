#pragma once

#include "TaskcURLHttpBase.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class TaskcURLHttpDelete
        : public TaskcURLHttpBase
    {
    public:
        TaskcURLHttpDelete( GOAP::Allocator * _allocator, const String & _url, const cURLHeaders & _headers, int32_t _timeout, bool _receiveHeaders, const cURLTaskReceiverInterfacePtr & _receiver );
        ~TaskcURLHttpDelete() override;

    protected:
        bool _onRun( GOAP::NodeInterface * _node ) override;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef GOAP::IntrusivePtr<TaskcURLHttpDelete> TaskcURLHttpDeletePtr;
    //////////////////////////////////////////////////////////////////////////
}
