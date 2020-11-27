#pragma once

#include "TaskcURLHttpBase.h"

namespace Mengine
{
    class TaskcURLHttpHeaderData
        : public TaskcURLHttpBase
    {
    public:
        TaskcURLHttpHeaderData( GOAP::Allocator * _allocator, const String & _url, const cURLHeaders & _headers, int32_t _timeout, bool _receiveHeaders, const String & _data, const cURLTaskReceiverInterfacePtr & _receiver );
        ~TaskcURLHttpHeaderData() override;

    protected:
        bool _onRun( GOAP::NodeInterface * _node ) override;

    protected:
        String m_data;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef GOAP::IntrusivePtr<TaskcURLHttpHeaderData> TaskHttpHeaderDataPtr;
    //////////////////////////////////////////////////////////////////////////
}
