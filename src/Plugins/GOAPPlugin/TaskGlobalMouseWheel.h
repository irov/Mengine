#pragma once

#include "Interface/InputHandlerInterface.h"

#include "Config/Lambda.h"

#include "GOAP/Task.h"


namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    typedef Lambda<bool( const InputMouseWheelEvent & )> LambdaInputMouseWheelEvent;
    //////////////////////////////////////////////////////////////////////////
    class TaskGlobalMouseWheel
        : public GOAP::Task
    {
    public:
        explicit TaskGlobalMouseWheel( const LambdaInputMouseWheelEvent & _filter );
        ~TaskGlobalMouseWheel() override;

    protected:
        bool _onRun( GOAP::NodeInterface * _node ) override;
        void _onFinally() override;

    protected:
        LambdaInputMouseWheelEvent m_filter;

        uint32_t m_id;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef GOAP::IntrusivePtr<TaskGlobalMouseWheel> TaskGlobalMouseWheelPtr;
    //////////////////////////////////////////////////////////////////////////
}