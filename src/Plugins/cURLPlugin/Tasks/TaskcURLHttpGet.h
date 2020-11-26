#pragma once

#include "TaskcURLHttpBase.h"

namespace Mengine
{
    class TaskcURLHttpGet
        : public TaskcURLHttpBase
    {
    public:
        TaskcURLHttpGet( GOAP::Allocator * _allocator, const String & _url, const cURLHeaders & _headers, int32_t _timeout, bool _receiveHeaders, const cURLTaskReceiverInterfacePtr & _receiver );
        ~TaskcURLHttpGet() override;

    protected:
        bool _onRun( GOAP::NodeInterface * _node ) override;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef GOAP::IntrusivePtr<TaskcURLHttpGet> TaskcURLHttpGetPtr;
    //////////////////////////////////////////////////////////////////////////
}
