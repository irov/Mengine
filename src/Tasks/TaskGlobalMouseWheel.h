#pragma once

#include "Interface/InputHandlerInterface.h"

#include "Config/Lambda.h"

#include "GOAP/Task.h"


namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    typedef Lambda<bool( const InputMouseWheelEvent & )> LambdaMouseWheelEvent;
    //////////////////////////////////////////////////////////////////////////
    class TaskGlobalMouseWheel
        : public GOAP::Task
    {
    public:
        explicit TaskGlobalMouseWheel( const LambdaMouseWheelEvent & _filter );
        ~TaskGlobalMouseWheel() override;

    protected:
        bool _onRun() override;
        void _onFinally() override;

    protected:
        LambdaMouseWheelEvent m_filter;

        uint32_t m_id;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<TaskGlobalMouseWheel> TaskGlobalMouseWheelPtr;
    //////////////////////////////////////////////////////////////////////////
}