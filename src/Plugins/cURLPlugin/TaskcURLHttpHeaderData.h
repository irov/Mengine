#pragma once

#include "TaskcURLHttpBase.h"

namespace Mengine
{
    class TaskcURLHttpHeaderData
        : public TaskcURLHttpBase
    {
    public:
        TaskcURLHttpHeaderData( const String & _url, const VectorString & _headers, const String & _data, int32_t _timeout, const cURLTaskReceiverInterfacePtr & _receiver );
        ~TaskcURLHttpHeaderData() override;

    protected:
        bool _onRun( GOAP::NodeInterface * _node ) override;

    protected:
        VectorString m_headers;
        String m_data;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef GOAP::IntrusivePtr<TaskcURLHttpHeaderData> TaskHttpHeaderDataPtr;
    //////////////////////////////////////////////////////////////////////////
}
