#pragma once

#include "TaskHttpBase.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class TaskHttpPost
        : public TaskHttpBase
    {
    public:
        TaskHttpPost( GOAP::Allocator * _allocator, const String & _url, const HttpRequestHeaders & _headers, int32_t _timeout, bool _receiveHeaders, const HttpRequestPostParams & _params, const HttpTaskReceiverInterfacePtr & _receiver );
        ~TaskHttpPost() override;

    protected:
        bool _onRun( GOAP::NodeInterface * _node ) override;

    protected:
        HttpRequestPostParams m_params;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef GOAP::IntrusivePtr<TaskHttpPost> TaskHttpPostPtr;
    //////////////////////////////////////////////////////////////////////////
}
