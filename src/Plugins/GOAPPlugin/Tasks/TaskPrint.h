#pragma once

#include "Kernel/IntrusivePtr.h"
#include "Kernel/String.h"

#include "Config/StdArg.h"

#include "GOAP/TaskInterface.h"

namespace Mengine
{
    class TaskPrint
        : public GOAP::TaskInterface
    {
    public:
        TaskPrint( GOAP::Allocator * _allocator, const Char * _format, ... );
        TaskPrint( GOAP::Allocator * _allocator, const Char * _format, MENGINE_VA_LIST_TYPE _args );
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