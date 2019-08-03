#pragma once

#include "Kernel/IntrusivePtr.h"

#include "Config/String.h"

#include "GOAP/Task.h"

namespace Mengine
{
    class TaskPrint
        : public GOAP::Task
    {
    public:
        TaskPrint( const Char * _format, ... );
        ~TaskPrint() override;

    protected:
        bool _onRun() override;

    protected:
        String m_message;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef GOAP::IntrusivePtr<TaskPrint> TaskPrintPtr;
    //////////////////////////////////////////////////////////////////////////
}