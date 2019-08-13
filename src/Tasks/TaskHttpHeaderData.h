#pragma once

#include "Tasks/TaskHttpBase.h"

namespace Mengine
{
    class TaskHttpHeaderData
        : public TaskHttpBase
    {
    public:
        TaskHttpHeaderData( const String & _url, const VectorString & _headers, const String & _data, int32_t _timeout, const LambdacURLReceiver & _lambda );
        ~TaskHttpHeaderData() override;

    protected:
        bool _onRun() override;

    protected:
        VectorString m_headers;
        String m_data;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef GOAP::IntrusivePtr<TaskHttpHeaderData> TaskHttpHeaderDataPtr;
    //////////////////////////////////////////////////////////////////////////
}
