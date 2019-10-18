#pragma once

#include "TaskcURLHttpBase.h"

namespace Mengine
{
    class TaskcURLHttpGet
        : public TaskcURLHttpBase
    {
    public:
        TaskcURLHttpGet( const String & _url, int32_t _timeout, const cURLTaskReceiverInterfacePtr & _receiver );
        ~TaskcURLHttpGet() override;

    protected:
        bool _onRun( GOAP::NodeInterface * _node ) override;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef GOAP::IntrusivePtr<TaskcURLHttpGet> TaskcURLHttpGetPtr;
    //////////////////////////////////////////////////////////////////////////
}
