#pragma once

#include "Tasks/TaskHttpBase.h"

namespace Mengine
{
    class TaskHttpGet
        : public TaskHttpBase
    {
    public:
        TaskHttpGet( const String & _url, int32_t _timeout, const LambdacURLReceiver & _lambda );
        ~TaskHttpGet() override;

    protected:
        bool _onRun() override;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef GOAP::IntrusivePtr<TaskHttpGet> TaskHttpGetPtr;
    //////////////////////////////////////////////////////////////////////////
}
