#pragma once

#include "TaskcURLHttpBase.h"

namespace Mengine
{
    class TaskcURLHttpPost
        : public TaskcURLHttpBase
    {
    public:
        TaskcURLHttpPost( const String & _url, const cURLPostParams & _params, int32_t _timeout, const cURLTaskReceiverInterfacePtr & _receiver );
        ~TaskcURLHttpPost() override;

    protected:
        bool _onRun( GOAP::NodeInterface * _node ) override;

    protected:
        cURLPostParams m_params;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef GOAP::IntrusivePtr<TaskcURLHttpPost> TaskHttpPostPtr;
    //////////////////////////////////////////////////////////////////////////
}
