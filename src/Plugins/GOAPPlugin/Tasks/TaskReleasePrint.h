#pragma once

#include "Kernel/IntrusivePtr.h"
#include "Kernel/String.h"

#include "Config/StdArg.h"

#include "GOAP/TaskInterface.h"

namespace Mengine
{
    class TaskReleasePrint
        : public GOAP::TaskInterface
    {
    public:
        TaskReleasePrint( GOAP::Allocator * _allocator, const Char * _format, ... );
        TaskReleasePrint( GOAP::Allocator * _allocator, const Char * _format, MENGINE_VA_LIST_TYPE _args );
        ~TaskReleasePrint() override;

    protected:
        bool _onRun( GOAP::NodeInterface * _node ) override;

    protected:
        String m_message;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef GOAP::IntrusivePtr<TaskReleasePrint> TaskReleasePrintPtr;
    //////////////////////////////////////////////////////////////////////////
}