#pragma once

#include "TaskHttpBase.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class TaskHttpHeaderData
        : public TaskHttpBase
    {
    public:
        TaskHttpHeaderData( GOAP::Allocator * _allocator, const String & _url, const HttpRequestHeaders & _headers, int32_t _timeout, bool _receiveHeaders, const String & _data, const HttpTaskReceiverInterfacePtr & _receiver );
        ~TaskHttpHeaderData() override;

    protected:
        bool _onRun( GOAP::NodeInterface * _node ) override;

    protected:
        String m_data;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef GOAP::IntrusivePtr<TaskHttpHeaderData> TaskHttpHeaderDataPtr;
    //////////////////////////////////////////////////////////////////////////
}
