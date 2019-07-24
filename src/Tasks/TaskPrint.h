#pragma once

#include "Config/String.h"

#include "GOAP/Task.h"

#include <stdarg.h>

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
    using TaskPrintPtr = GOAP::IntrusivePtr<TaskPrint>;
    //////////////////////////////////////////////////////////////////////////
}