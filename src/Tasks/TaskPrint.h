#pragma once

#include "Kernel/IntrusivePtr.h"

#include "Config/String.h"
#include "Config/StdArg.h"

#include "GOAP/Task.h"

namespace Mengine
{
    class TaskPrint
        : public GOAP::Task
    {
    public:
        TaskPrint( const Char * _format, ... );
        TaskPrint( const Char * _format, MENGINE_VA_LIST_TYPE _args );
        ~TaskPrint() override;

    protected:
        bool _onRun( GOAP::NodeInterface * _node ) override;

    protected:
        String m_message;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef GOAP::IntrusivePtr<TaskPrint> TaskPrintPtr;
    //////////////////////////////////////////////////////////////////////////
}