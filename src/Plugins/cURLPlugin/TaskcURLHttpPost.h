#pragma once

#include "TaskcURLHttpBase.h"

namespace Mengine
{
    class TaskcURLHttpPost
        : public TaskcURLHttpBase
    {
    public:
        TaskcURLHttpPost( const String & _url, const cURLHeaders & _headers, int32_t _timeout, bool _receiveHeaders, const String & _cookies, const cURLPostParams & _params, const cURLTaskReceiverInterfacePtr & _receiver );
        ~TaskcURLHttpPost() override;

    protected:
        bool _onRun( GOAP::NodeInterface * _node ) override;

    protected:
        String m_cookies;
        cURLPostParams m_params;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef GOAP::IntrusivePtr<TaskcURLHttpPost> TaskHttpPostPtr;
    //////////////////////////////////////////////////////////////////////////
}
