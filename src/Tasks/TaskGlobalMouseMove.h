#pragma once

#include "Interface/InputHandlerInterface.h"

#include "Config/Lambda.h"

#include "GOAP/Task.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    typedef Lambda<bool( const InputMouseMoveEvent & )> LambdaMouseMoveEvent;
    //////////////////////////////////////////////////////////////////////////
    class TaskGlobalMouseMove
        : public GOAP::Task
    {
    public:
        explicit TaskGlobalMouseMove( const LambdaMouseMoveEvent & _filter );
        ~TaskGlobalMouseMove() override;

    protected:
        bool _onRun() override;
        void _onFinally() override;

    protected:
        LambdaMouseMoveEvent m_filter;

        uint32_t m_id;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<TaskGlobalMouseMove> TaskGlobalMouseMovePtr;
    //////////////////////////////////////////////////////////////////////////
}