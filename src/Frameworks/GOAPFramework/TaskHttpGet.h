#pragma once

#include "TaskHttpBase.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class TaskHttpGet
        : public TaskHttpBase
    {
    public:
        TaskHttpGet( GOAP::Allocator * _allocator, const String & _url, const HttpRequestHeaders & _headers, int32_t _timeout, bool _receiveHeaders, const HttpTaskReceiverInterfacePtr & _receiver );
        ~TaskHttpGet() override;

    protected:
        bool _onRun( GOAP::NodeInterface * _node ) override;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef GOAP::IntrusivePtr<TaskHttpGet> TaskHttpGetPtr;
    //////////////////////////////////////////////////////////////////////////
}
